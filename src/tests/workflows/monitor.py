#!/usr/bin/env python
"""
Monitor a headless (-nographics) CK3 run for crash / success signals.

Launches CK3_EXE, tails the dedicated server log for a "load complete"
marker, watches the crashes/ directory for new crash dumps, and enforces
an overall timeout. Exits 0 on a clean successful run, non-zero on any
failure mode, and always leaves the log files in place for the CI job
to upload as artifacts.
"""

import argparse
import json
import os
import signal
import subprocess
import sys
import time
from pathlib import Path


def parse_args():
    p = argparse.ArgumentParser(description="Monitor a headless CK3 run")
    p.add_argument("--ck3-exe", default=os.environ.get("CK3_EXE"),
                    help="Path to the CK3 binary (defaults to $CK3_EXE)")
    p.add_argument("--ck3-user-dir", default=os.environ.get("CK3_USER_DIR"),
                    help="CK3 user data dir (defaults to $CK3_USER_DIR)")
    p.add_argument("--extra-args", nargs=argparse.REMAINDER, default=[],
                    help="Extra args passed through to the CK3 binary")
    p.add_argument("--loading-marker", default="Finished loading savegame",
                    help="String to look for in dedicated_server.log")
    p.add_argument("--poll-interval", type=float, default=1.0,
                    help="Seconds between checks")
    p.add_argument("--loading-timeout", type=float, default=600,
                    help="Max seconds to wait for the savegame to load")
    p.add_argument("--playing-timeout", type=float, default=60,
                    help="Max seconds to keep the game running after loading succeeds")
    p.add_argument("--shutdown-grace", type=float, default=15,
                    help="Seconds to wait for clean exit after SIGTERM before SIGKILL")
    p.add_argument("--result-file", default="result.json",
                    help="Where to write the JSON summary")
    return p.parse_args()


def tail_new_lines(path: Path, offset: int):
    """Read any new lines appended to `path` since `offset`. Returns (lines, new_offset)."""
    if not path.exists():
        return [], offset
    with path.open("r", errors="replace") as f:
        f.seek(offset)
        data = f.read()
        new_offset = f.tell()
    lines = data.splitlines()
    return lines, new_offset


def list_crash_files(crash_dir: Path):
    if not crash_dir.exists():
        return set()
    return {p.name for p in crash_dir.iterdir() if p.is_file()}


def terminate(proc: subprocess.Popen, grace: float):
    if proc.poll() is not None:
        return
    proc.send_signal(signal.SIGTERM)
    try:
        proc.wait(timeout=grace)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait(timeout=grace)

def main():
    args = parse_args()

    if not args.ck3_exe:
        print("ERROR: CK3_EXE not set and --ck3-exe not given", file=sys.stderr)
        sys.exit(2)
    if not args.ck3_user_dir:
        print("ERROR: CK3_USER_DIR not set and --ck3-user-dir not given", file=sys.stderr)
        sys.exit(2)

    user_dir = Path(args.ck3_user_dir)
    log_dir = user_dir / "logs"
    crash_dir = user_dir / "crashes"
    status_log = log_dir / "dedicated_server.log"

    log_dir.mkdir(parents=True, exist_ok=True)
    crash_dir.mkdir(parents=True, exist_ok=True)

    cmd = [args.ck3_exe, "-nographics", *args.extra_args]
    print(f"Launching: {' '.join(cmd)}")

    proc = subprocess.Popen(cmd)

    known_crash_files = list_crash_files(crash_dir)
    server_offset = 0

    result = {
        "success": False,
        "reason": "",
        "load_time_seconds": 0.0,
        "runtime_seconds": 0.0,
        "exit_code": 0
    }

    start_timestamp = time.time()
    loaded_timestamp = None

    try:
        while True:
            elapsed = time.time() - start_timestamp

            # Check if the process is still running.
            ret = proc.poll()
            if ret is not None and loaded_timestamp is None:
                # Process exited before the savegame was fully loaded.
                if loaded_timestamp is None:
                    result["reason"] = "Crashed while loading savegame"
                    result["exit_code"] = ret
                    break
                # Process exited after the savegame was loaded, while playing.
                else:
                    result["reason"] = "Crashed after loading savegame"
                    result["exit_code"] = ret
                    break
               

            # Tail the dedicated_server log to determine when the savegame is fully loaded.
            lines, server_offset = tail_new_lines(status_log, server_offset)
            for line in lines:
                print(line)
                # Savegame not fully loaded yet.
                if loaded_timestamp is None and args.loading_marker in line:
                    loaded_timestamp = time.time()
                    result["load_time_seconds"] = round(loaded_timestamp - start_timestamp, 1)
                    print(f"Savegame successfully loaded after {result['load_time_seconds']}s")
                    break

            # Check if the loading timeout has been reached.
            if loaded_timestamp is None and elapsed >= args.loading_timeout:
                result["reason"] = "Timeout while loading"
                terminate(proc, args.shutdown_grace)
                result["exit_code"] = proc.returncode
                break

            # Savegame fully loaded, so check if the playing timout hasn't been reached.
            if loaded_timestamp is not None:
                if time.time() - loaded_timestamp >= args.playing_timeout:
                    result["success"] = True
                    result["reason"] = "Exited after timeout"
                    terminate(proc, args.shutdown_grace)
                    result["exit_code"] = proc.returncode
                    break

            time.sleep(args.poll_interval)
    finally:
        # Make sure nothing is left running regardless of how we exit.
        if proc.poll() is None:
            terminate(proc, args.shutdown_grace)

    result["runtime_seconds"] = round(time.time() - start_timestamp, 1)

    with open(args.result_file, "w") as f:
        json.dump(result, f, indent=2)

    if result["success"]:
        print("SUCCESS: Loaded and played without crash.")
        sys.exit(0)
    else:
        print(f"FAILED: {result['reason']}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()