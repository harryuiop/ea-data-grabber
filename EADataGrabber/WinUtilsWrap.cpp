/**
 * @file FilePathUtils.cpp
 * 
 *  This file contains utility functions for working with file paths, extensions, and directories.
 *  It includes functions for extracting file extensions, filenames, retrieving the system's Downloads folder path,
 *  and checking if a directory exists.
 */

#include <string>
#include <filesystem>
#include <windows.h>
#include <shlobj.h>
#include <algorithm> 
#include <cctype>    

 /**
  * @brief Converts a string to lowercase.
  *
  * @param str The string to convert.
  * @return std::string The lowercase version of the input string.
  */
std::string toLowercase(const std::string& str) {
    std::string lowerStr = str; // Create a copy of the input string
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

std::string removeLeadingWhitespace(const std::string& inputStr) {
    const size_t startPos = inputStr.find_first_not_of(' ');
    if (startPos == std::string::npos) {
        return inputStr;
    }
    const size_t lastPos = inputStr.find_last_not_of(' ');
    const size_t strRange = lastPos - startPos + 1;
    return inputStr.substr(startPos, lastPos);
}

 /**
  * Extracts the file extension from a given URL or file path.
  *
  * @param Url The URL or file path from which to extract the file extension.
  * @return std::string The file extension (e.g., "txt", "pdf"), or an empty string if no extension is found.
  */
std::string getFileExtension(const std::string& Url) {
    size_t dotPos = Url.rfind('.');

    if (dotPos == std::string::npos || Url.rfind('/') > dotPos) {
        return ""; // No file extension
    }

    return Url.substr(dotPos + 1);
}

/**
 * Extracts the filename from a given URL or file path.
 *
 * @param Url The URL or file path from which to extract the filename.
 * @return std::string The filename (e.g., "example.txt"), or an empty string if no filename is found.
 */
std::string getFileName(const std::string& Url) {
    size_t slashPos = Url.rfind('/');

    if (slashPos == std::string::npos) {
        return "";
    }

    return Url.substr(slashPos + 1);
}

/**
 * Retrieves the path to the system's Downloads folder.
 *
 * @return std::string The path to the Downloads folder, or an empty string if the path cannot be retrieved.
 */
std::string getDownloadsFolderPath() {
    wchar_t* path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &path);
    if (SUCCEEDED(hr)) {
        std::wstring ws(path);  // Convert to wide string
        std::string str(ws.begin(), ws.end());  // Convert to normal string
        CoTaskMemFree(path);  // Free memory allocated by SHGetKnownFolderPath
        return str;
    }
    return "";
}

/**
 * Checks if a directory exists at the specified path.
 *
 * @param dirPath The path to the directory to check.
 * @return bool True if the directory exists, false otherwise.
 */
bool directoryExists(const std::string& dirPath) {
    return std::filesystem::is_directory(dirPath);
}