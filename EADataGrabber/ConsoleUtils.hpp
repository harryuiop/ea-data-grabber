#ifndef CONSOLEUTILS_HPP
#define CONSOLEUTILS_HPP

#include <string>

void PrintPrompt(const std::string& message);
void PrintError(const std::string& message);
void PrintSuccess(const std::string& message);
void PrintInfo(const std::string& message);
void PrintRedEscape(const std::string& message);
void PrintPurpleEscape(const std::string& message);
void PrintGreyEscape(const std::string& message);
void PrintMessage(const std::string& message);
void PrintNewLine();

void PrintDownloadInstructions();

#endif