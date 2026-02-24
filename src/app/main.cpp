#include "app/App.hpp"

int main() {
#ifdef _WIN32
    // Temporary use try/catch to print crashes to a
    // file as backward is disabled on Windows.
    try {
        App app;
        app.Init();
        app.Run();
        // Parser::Benchmark();
    }
    catch(std::exception& e) {
        std::ofstream file(CRASH_FILE, std::ios::out);
        file << e.what() << std::endl;
        file.close();
    }
#else
    App app;
    app.Init();
    app.Run();
    // Parser::Tests();
    // Parser::Benchmark();
#endif
    return 0;
}