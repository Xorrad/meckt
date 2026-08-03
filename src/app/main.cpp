#include "app/App.hpp"

#include "core/util/SignalHandler.hpp"
SignalHandler SIGNAL_HANDLER;

int main() {
    App app;
    app.Init();
    app.Run();
    // Parser::Tests();
    // Parser::Benchmark();
    return 0;
}