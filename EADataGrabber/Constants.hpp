#pragma once
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include <chrono>
#include <array>
#include <map>

namespace CONSTANTS {
    // Date-related constants
    constexpr int ExpectedDateArgumentLengthShort = 10;
    constexpr int ExpectedDateArgumentLengthLong = 11;
    const std::string StartDateStrArg = "-sd";
    const std::string EndDateStrArg = "-ed";
    constexpr std::chrono::year_month_day DefaultDate = std::chrono::year_month_day{};
    constexpr std::chrono::year_month_day InvalidDate{ std::chrono::year{5000}, std::chrono::month{99}, std::chrono::day{99} };

    // Color escape sequences
    constexpr std::string_view InformationInsert = "\033[38;5;214mInfo: \033[0m";
    constexpr std::string_view UserInputInsert = "\033[34mUser Input: \033[0m";
    constexpr std::string_view SuccessEscape = "\033[32mSuccess: \033[0m";
    constexpr std::string_view LightGreyEscape = "\033[38;5;150m"; 
    constexpr std::string_view RedEscape = "\033[31m";
    constexpr std::string_view PurpleEscape = "\033[35m";
    constexpr std::string_view OrangeEscape = "\033[38;5;214m";
    constexpr std::string_view GreenEscape = "\033[32m";
    constexpr std::string_view BlueEscape = "\033[34m";
    constexpr std::string_view DefaultEscape = "\033[0m";

    // URLs
    const std::string BaseURL = "https://emidatasets.blob.core.windows.net/publicdata/Datasets";
    const std::string AllContainersURL = "https://emidatasets.blob.core.windows.net/publicdata?restype=container&comp=list&prefix=";

    // Newline constant
    constexpr std::string_view NewLine = "\n";

    // File extensions
    enum FileExtension {
        PDF,
        TXT,
        CSV,
        ZIP,
        GDX,
        UNSUPPORTED,
        NONE
    };

    // Days in each month
    constexpr std::array<int, 12> MonthDays = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // cout constants
    const std::string MenuOption1 = "  1. \033[32mDownload Files\033[0m";
    const std::string MenuOption2 = "  2. \033[38;5;214mChange Output Directory\033[0m";
    const std::string MenuOption3 = "  3. \033[31mExit\033[0m";
    const std::string InvalidInputMessage = "   Invalid input, try again.";
}
#endif