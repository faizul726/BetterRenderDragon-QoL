#pragma once

#include <fmt/color.h>
#include <fmt/core.h>

#include <chrono>
#include <iomanip>
#include <ranges>
#include <sstream>


class Logger {
    enum class LogLevel { INFO, DEBUG, WARN, ERROR_ };

public:
    explicit Logger(std::string name) : name_(std::move(name)) {}

    template <typename... Args>
    void info(const std::string& format, const Args&... args) {
        log(LogLevel::INFO, fmt::color::light_sea_green, format, args...);
    }

    template <typename... Args>
    void debug(const std::string& format, const Args&... args) {
        log(LogLevel::DEBUG, fmt::color::lemon_chiffon, format, args...);
    }

    template <typename... Args>
    void warn(const std::string& format, const Args&... args) {
        log(LogLevel::WARN, fmt::color::yellow, format, args...);
    }

    template <typename... Args>
    void error(const std::string& format, const Args&... args) {
        log(LogLevel::ERROR_, fmt::terminal_color::bright_red, format, args...);
    }

private:
    std::string current_time_string() {
        auto    now   = std::chrono::system_clock::now();
        auto    now_c = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm{};
        localtime_s(&now_tm, &now_c);
        std::stringstream ss;
        ss << std::put_time(&now_tm, "%X");
        return ss.str();
    }

private:
    template <typename... Args>
    void log(LogLevel level, fmt::color color, const std::string& format, const Args&... args) {
        const char* prefix            = "";
        std::string formatted_message = fmt::vformat(format, fmt::make_format_args(args...));
        auto        lines             = formatted_message | std::ranges::views::split('\n');
        for (const auto line : lines) {
            fmt::print(fg(fmt::color::light_blue) | fmt::emphasis::bold, "{} ", current_time_string());

            switch (level) {
            case LogLevel::INFO:
                prefix = "INFO";
                fmt::print(fg(color) | fmt::emphasis::bold, "{} ", prefix);
                break;
            case LogLevel::DEBUG:
                prefix = "DEBUG";
                fmt::print(fg(color) | fmt::emphasis::bold, "{} ", prefix);
                break;
            case LogLevel::WARN:
                prefix = "WARN";
                fmt::print(fg(color) | fmt::emphasis::bold, "{} [{}] {}\n", prefix, name_, formatted_message);
                return;
            case LogLevel::ERROR_:
                prefix = "ERROR";
                fmt::print(fg(color) | fmt::emphasis::bold, "{} [{}] {}\n", prefix, name_, formatted_message);
                return;
            }

            fmt::print("[{}] {}\n", name_, std::string_view(line.begin(), line.end()));
        }
    }

    std::string name_;
};