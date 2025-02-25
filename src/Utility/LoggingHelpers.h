#pragma once

#define LOG_AND_THROW(str, ...) \
    SPDLOG_ERROR(str, ##__VA_ARGS__); \
    throw std::runtime_error(fmt::format(str, ##__VA_ARGS__));