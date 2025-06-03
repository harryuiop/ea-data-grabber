/**
 * @file CurlUtilsWrap.cpp
 * 
 *  This file contains utility functions for performing HTTP requests using cURL.
 *  It includes functions for downloading text and binary files, handling cURL responses,
 *  and initializing and cleaning up cURL objects.
 */

#include <fstream>  
#include <sstream>
#include <iostream>
#include "CurlUtilsWrap.hpp"
#include "Constants.hpp"
#include "ConsoleUtils.hpp"

using namespace CONSTANTS;

/**
 * Callback function for writing received data into a string.
 *
 * @param contents Pointer to the data received.
 * @param size Size of each data element.
 * @param nmemb Number of data elements.
 * @param userp Pointer to the user-provided data (in this case, a string).
 * @return size_t Total size of the data written.
 */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), total_size);
    return total_size;
}

/**
 * Callback function for writing received text data into a stringstream.
 *
 * @param contents Pointer to the data received.
 * @param size Size of each data element.
 * @param nmemb Number of data elements.
 * @param buffer Pointer to the stringstream where the data will be written.
 * @return size_t Total size of the data written.
 */
size_t WriteDownloadTextCallback(void* contents, size_t size, size_t nmemb, std::stringstream* buffer) {
    size_t totalSize = size * nmemb;
    buffer->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}

/**
 * Callback function for writing received binary data into a file.
 *
 * @param contents Pointer to the data received.
 * @param size Size of each data element.
 * @param nmemb Number of data elements.
 * @param userp Pointer to the output file stream where the data will be written.
 * @return size_t Total size of the data written.
 */
size_t WriteDownloadBinaryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::ofstream* outputFile = static_cast<std::ofstream*>(userp);
    outputFile->write(static_cast<char*>(contents), totalSize);
    return totalSize;
}

/**
 * Sets common cURL options for a given cURL handle.
 *
 * @param curl Shared pointer to the cURL handle.
 * @param url The URL to request.
 * @param writeData Pointer to the data structure where the response will be written.
 */
void SetCurlOptions(std::shared_ptr<CURL> curl, const std::string& url, void* writeData) {
    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, writeData);
}

/**
 * Downloads and saves a text-based file (e.g., TXT, CSV).
 *
 * @param curl Shared pointer to the cURL handle.
 * @param parameters Struct containing download parameters (URL, file name, output directory, etc.).
 * @return int 0 on success, 1 on failure.
 */
int DownloadTextFile(std::shared_ptr<CURL> curl, const DownloadParameters& parameters) {
    std::stringstream buffer;
    SetCurlOptions(curl, parameters.URL, &buffer);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteDownloadTextCallback);

    CURLcode res = curl_easy_perform(curl.get());
    if (res != CURLE_OK) {
        std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    std::ofstream outputFile(parameters.OutputDir + "\\" + parameters.FileName);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open file: " << parameters.FileName << std::endl;
        return 1;
    }

    outputFile << buffer.str();
    outputFile.close();
    return 0;
}

/**
 * Downloads and saves a binary file (e.g., GDX, ZIP, PDF).
 *
 * @param curl Shared pointer to the cURL handle.
 * @param parameters Struct containing download parameters (URL, file name, output directory, etc.).
 * @return int 0 on success, 1 on failure.
 */
int DownloadBinaryFile(std::shared_ptr<CURL> curl, const DownloadParameters& parameters) {
    std::stringstream buffer;
    SetCurlOptions(curl, parameters.URL, &buffer);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteDownloadBinaryCallback);
    curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 60L);

    CURLcode res = curl_easy_perform(curl.get());
    if (res != CURLE_OK) {
        std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    std::ofstream outputFile(parameters.OutputDir + "\\" + parameters.FileName, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open the output file!" << std::endl;
        return 1;
    }

    outputFile << buffer.str();
    outputFile.close();
    return 0;
}

/**
 * Handles the cURL response and logs the result.
 *
 * @param curl Shared pointer to the cURL handle.
 * @param parameters Struct containing download parameters (URL, file name, output directory, etc.).
 */
void HandleCurlResponse(std::shared_ptr<CURL> curl, const DownloadParameters& parameters) {
    long responseCode;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &responseCode);
    std::cout << "    HTTPS Response Code For " << parameters.FileName << ": " << responseCode << std::endl;
}

/**
 * Performs a cURL request to download a file based on the provided parameters.
 *
 * @param curl Shared pointer to the cURL handle.
 * @param parameters Struct containing download parameters (URL, file name, output directory, etc.).
 * @return int 0 on success, 1 on failure.
 */
int PerformCurlRequest(std::shared_ptr<CURL> curl, const DownloadParameters& parameters) {
    std::cout << InformationInsert << "Requesting data from the Electricity Authority API..." << NewLine;

    int result = 0;
    switch (parameters.FileExtension) {
        case TXT:
        case CSV:
            result = DownloadTextFile(curl, parameters);
            break;
        case GDX:
        case ZIP:
        case PDF:
            result = DownloadBinaryFile(curl, parameters);
            break;
        default:
            std::cerr << "Unsupported file type!" << std::endl;
            return 1;
    }

    if (result == 0) {
        HandleCurlResponse(curl, parameters);
    }

    return result;
}

/**
 * Performs a generic cURL request without specific download parameters.
 *
 * @param curl Shared pointer to the cURL handle.
 * @return int 0 on success, 1 on failure.
 */
int PerformCurlRequest(std::shared_ptr<CURL> curl) {
    std::cout << InformationInsert << "Requesting data block from the Electricity Authority API..." << NewLine;
    CURLcode res;

    res = curl_easy_perform(curl.get());

    if (res != CURLE_OK) {
        std::cout << "CURL Request failed with error code " << res << std::endl;
        return 1;
    }

    return 0;
}

/**
 * Initializes a cURL handle with default options.
 *
 * @return CURL* Pointer to the initialized cURL handle, or nullptr if initialization fails.
 */
CURL* InitCurl() {
    CURL* curl = nullptr;
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, AllContainersURL.c_str());

#ifdef SKIP_PEER_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

        curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    }
    return curl;
}

/**
 * Cleans up and deallocates a cURL handle.
 *
 * @param curl Pointer to the cURL handle to be cleaned up.
 */
void CurlDeleter(CURL* curl) {
    std::string h;
    std::cin >> h;
    if (curl) {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
}

/**
 * Converts a string representation of a file extension to the corresponding enum value.
 *
 * @param str The file extension as a string (e.g., "pdf", "txt").
 * @return CONSTANTS::FileExtension The corresponding enum value, or UNSUPPORTED if the extension is not recognized.
 */
CONSTANTS::FileExtension ConvertStrToFileExt(const std::string& str) {
    if (str == "pdf") {
        return PDF;
    }
    if (str == "txt") {
        return TXT;
    }
    if (str == "csv") {
        return CSV;
    }
    if (str == "zip") {
        return ZIP;
    }
    if (str == "gdx") {
        return GDX;
    }
    return UNSUPPORTED;
}