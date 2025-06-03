#ifndef WINUTILSWRAP_HPP
#define WINUTILSWRAP_HPP

#include <string>

std::string removeLeadingWhitespace(const std::string& inputStr);
std::string getFileExtension(const std::string& Url);
std::string getFileName(const std::string& Url);
std::string getDownloadsFolderPath();
std::string toLowercase(const std::string& str);
bool directoryExists(const std::string& dirPath);

#endif