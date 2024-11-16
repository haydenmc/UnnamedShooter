#include "pch.h"

using ArgList = std::vector<std::pair<std::string, std::string>>;

int Entrypoint(ArgList arguments)
{
    SPDLOG_INFO("Entrypoint");
    for (const auto& argPair : arguments)
    {
        SPDLOG_INFO("Argument '{}': '{}'", argPair.first, argPair.second);
    }

    SPDLOG_INFO("Entrypoint: Exit");
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