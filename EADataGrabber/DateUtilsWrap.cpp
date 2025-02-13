/**
 * @file DateParsingUtils.cpp
 * 
 *  This file contains utility functions for parsing date strings into `std::chrono::year_month_day` objects.
 *  It supports parsing dates from various formats, including CLI arguments, filenames, and XML strings.
 */

#include <chrono>
#include <optional>
#include <regex>
#include <map>
#include <iostream>
#include <charconv>
#include "Constants.hpp"

 /**
  * Parses a date string from an XML format (e.g., "12 Jan 2023") into a `year_month_day` object.
  *
  * @param dateStr The date string to parse.
  * @return std::optional<std::chrono::year_month_day> The parsed date, or `std::nullopt` if parsing fails.
  */
std::optional<std::chrono::year_month_day> StringToDateEAXml(std::string dateStr) {
    using namespace std::chrono;

    std::regex dayPattern(R"(\b\d{1,2}\b)");
    std::regex monthPattern(R"(\b(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\b)");
    std::regex yearPattern(R"(\b\d{4}\b)");

    std::smatch dayMatch, monthMatch, yearMatch;
    std::string dateStrCopy(dateStr);

    int dayNum = 0;
    month monthNum{};
    int yearNum = 0;

    if (std::regex_search(dateStrCopy, dayMatch, dayPattern)) {
        std::string matchedStr = dayMatch.str();

        auto result = std::from_chars(matchedStr.data(), matchedStr.data() + matchedStr.size(), dayNum);
        if (result.ec != std::errc()) {
            return std::nullopt;
        }
    }

    if (std::regex_search(dateStrCopy, monthMatch, monthPattern)) {
        static const std::map<std::string, month> monthToNum = {
            {"Jan", January}, {"Feb", February}, {"Mar", March}, {"Apr", April},
            {"May", May}, {"Jun", June}, {"Jul", July}, {"Aug", August},
            {"Sep", September}, {"Oct", October}, {"Nov", November}, {"Dec", December}
        };
        auto it = monthToNum.find(monthMatch.str());
        if (it != monthToNum.end()) {
            monthNum = it->second;
        }
        else {
            return std::nullopt;
        }
    }

    if (std::regex_search(dateStrCopy, yearMatch, yearPattern)) {
        std::string matchedStr = yearMatch.str();
        auto result = std::from_chars(matchedStr.data(), matchedStr.data() + matchedStr.size(), yearNum);
        if (result.ec != std::errc()) {
            return std::nullopt;
        }
    }

    if (dayNum > 0 && yearNum > 0 && monthNum != month{}) {
        year_month_day ymd = year_month_day(year(yearNum), monthNum, day(dayNum));
        if (ymd.ok()) {
            return ymd;
        }
    }

    return std::nullopt;
}

/**
 * Parses a date string from a CLI argument format (e.g., "2023-01-12") into a `year_month_day` object.
 *
 * @param dateStr The date string to parse.
 * @return std::optional<std::chrono::year_month_day> The parsed date, or `std::nullopt` if parsing fails.
 */
std::optional<std::chrono::year_month_day> StringToDateCLIArg(std::string dateStr) {
    using namespace std::chrono;

    if (dateStr.length() != 10) {
        return std::nullopt;
    }

    int yearNum = 0, monthNum = 0, dayNum = 0;

    auto result = std::from_chars(dateStr.data(), dateStr.data() + 4, yearNum);
    if (result.ec != std::errc()) {
        return std::nullopt;
    }

    result = std::from_chars(dateStr.data() + 5, dateStr.data() + 7, monthNum);
    if (result.ec != std::errc()) {
        return std::nullopt;
    }

    result = std::from_chars(dateStr.data() + 8, dateStr.data() + 10, dayNum);
    if (result.ec != std::errc()) {
        return std::nullopt;
    }

    year_month_day ymd = year_month_day(year(yearNum), month(monthNum), day(dayNum));

    if (ymd.ok()) {
        return ymd;
    }

    return std::nullopt;
}

/**
 * Parses a date string from a filename format (e.g., "20230112" or "202301") into a `year_month_day` object.
 *
 * @param dateStr The date string to parse.
 * @return std::optional<std::chrono::year_month_day> The parsed date, or `std::nullopt` if parsing fails.
 */
std::optional<std::chrono::year_month_day> StringToDateEAFileName(std::string dateStr) {
    using namespace std::chrono;

    if (dateStr.length() < 6 || dateStr.length() > 8) {
        return std::nullopt;
    }

    int yearNum = 0, monthNum = 0, dayNum = 0;

    auto result = std::from_chars(dateStr.data(), dateStr.data() + 4, yearNum);
    if (result.ec != std::errc()) {
        return std::nullopt;
    }

    result = std::from_chars(dateStr.data() + 4, dateStr.data() + 6, monthNum);
    if (result.ec != std::errc()) {
        return std::nullopt;
    }

    if (dateStr.length() == 8) {
        result = std::from_chars(dateStr.data() + 6, dateStr.data() + 8, dayNum);
        if (result.ec != std::errc()) {
            return std::nullopt;
        }
    }
    else {
        if (monthNum >= 1 && monthNum <= 12) dayNum = CONSTANTS::MonthDays.at(monthNum - 1);
    }

    year_month_day ymd = year_month_day(year(yearNum), month(monthNum), day(dayNum));
    if (ymd.ok()) {
        return ymd;
    }

    return std::nullopt;
}

/**
 * Parses a date string using multiple formats (CLI argument, filename, XML) and returns the first valid result.
 *
 * @param dateStr The date string to parse.
 * @return std::optional<std::chrono::year_month_day> The parsed date, or `std::nullopt` if parsing fails for all formats.
 */
std::optional<std::chrono::year_month_day> ParseDate(std::string dateStr) {
    if (dateStr.empty()) {
        return std::nullopt;
    }

    if (auto ymd = StringToDateCLIArg(dateStr); ymd.has_value()) {
        return ymd;
    }

    if (auto ymd = StringToDateEAFileName(dateStr); ymd.has_value()) {
        return ymd;
    }

    if (auto ymd = StringToDateEAXml(dateStr); ymd.has_value()) {
        return ymd;
    }

    std::cerr << "Warning: Failed to parse date: " << dateStr << CONSTANTS::NewLine;
    return std::nullopt;
}