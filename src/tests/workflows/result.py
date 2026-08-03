import json, os

with open("result.json") as f:
    result = json.load(f)

result_message = "✅ Success" if result["success"] else "❌ " + result.get("reason")
load_time = result.get("load_time_seconds")
runtime = result.get("runtime_seconds")
exit_code = result.get("exit_code")

summary = (
    f"Result: {result_message}\n"
    f"Load-time: {load_time if load_time is not None else 'N/A'}s\n"
    f"Runtime: {runtime if runtime is not None else 'N/A'}s\n"
    f"Exit Code: {exit_code if exit_code is not None else 'N/A'}\n"
)

print(summary)

step_summary = os.environ.get("GITHUB_STEP_SUMMARY")
if step_summary:
    with open(step_summary, "a") as f:
        f.write("```\n" + summary + "```\n")