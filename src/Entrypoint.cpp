#include <pch.h>
#include "Configuration.h"
#include "Display.h"
#include "Input.h"
#include "Simulation.h"

using ArgList = std::vector<std::pair<std::string, std::string>>;

int Entrypoint(ArgList arguments)
#ifndef DEBUG
try // Don't catch unhandled exceptions for debug builds
#endif
{
    SPDLOG_INFO("Entrypoint");

    for (const auto& argPair : arguments)
    {
        SPDLOG_INFO("Argument '{}': '{}'", argPair.first, argPair.second);
    }

    // TODO: Load Configuration from file, arguments, and env vars
    Configuration configuration;
    VideoConfiguration resolution{ configuration.GetVideoConfiguration() };

    // Initialize subsystems in their own scope so we can perform additional
    // cleanup after they are destructed.
    {
        Simulation simulation{};
        Display display{ resolution };
        Renderer renderer{ display.GetWindow(), resolution };
        Input input{};

        // Run sim loop
        SPDLOG_INFO("Begin sim loop");
        while (true)
        {
            // Get input
            const auto& inputState{ input.GetInputState() };
            if (inputState.Escape)
            {
                SPDLOG_INFO("Escape pressed, exiting sim loop");
                break;
            }

            // Simulate
            auto const& simulationState{ simulation.Update(inputState) };

            // Render
            renderer.Render(simulationState);
        }
        SPDLOG_INFO("End sim loop, destruct subsystems");
    }

    SPDLOG_INFO("Exit");
    return 0;
}
#ifndef DEBUG
catch (std::exception const& e)
{
    SPDLOG_CRITICAL("!!! Unhandled Exception: {}", e.what());
    // Shut down logger to make sure all messages are flushed before the process
    // is terminated.
    spdlog::shutdown();
    throw;
}
#endif

int main(int argumentsCount, char* arguments[])
{
    SPDLOG_INFO("main");
    // TODO: Parse arguments
    return Entrypoint(ArgList{});
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    SPDLOG_INFO("WinMain");
    // TODO: Parse arguments
    return Entrypoint(ArgList{});
}
#endif