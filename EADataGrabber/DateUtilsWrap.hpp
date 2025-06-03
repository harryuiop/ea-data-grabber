#ifndef DATESUTILSWRAP_HPP
#define DATESUTILSWRAP_HPP

#include <chrono>

std::optional<std::chrono::year_month_day> StringToDateEAXml(std::string dateStr);
std::optional<std::chrono::year_month_day> StringToDateCLIArg(std::string dateStr);
std::optional<std::chrono::year_month_day> StringToDateEAFileName(std::string dateStr);
std::optional<std::chrono::year_month_day> ParseDate(std::string dateStr);

#endif