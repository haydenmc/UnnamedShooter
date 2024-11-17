#include <pch.h>
#include "Configuration.h"
#include "Display.h"
#include "Input.h"

using ArgList = std::vector<std::pair<std::string, std::string>>;

int Entrypoint(ArgList arguments)
{
    SPDLOG_INFO("Entrypoint");

    for (const auto& argPair : arguments)
    {
        SPDLOG_INFO("Argument '{}': '{}'", argPair.first, argPair.second);
    }

    // TODO: Load Configuration from file, arguments, and env vars
    Configuration configuration;
    VideoResolution resolution{ configuration.GetVideoResolution() };

    // Initialize subsystems in their own scope so we can perform additional
    // cleanup after they are destructed.
    {
        Display display{ resolution };
        Renderer renderer{ display.GetWindow(), resolution };
        Input input{};

        // Run sim loop
        SPDLOG_INFO("Begin sim loop");
        while (true)
        {
            const auto& inputState{ input.GetInputState() };
            if (inputState.Escape)
            {
                SPDLOG_INFO("Escape pressed, exiting sim loop");
                break;
            }
        }
        SPDLOG_INFO("End sim loop, destruct subsystems");
    }

    SPDLOG_INFO("Exit");
    return 0;
}

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