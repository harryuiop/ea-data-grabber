#ifndef PROJECT_CURL_UTILS_WRAP_HPP
#define PROJECT_CURL_UTILS_WRAP_HPP

#include <include/curl/curl.h>
#include <string>
#include <chrono>
#include "Constants.hpp"

struct DownloadParameters {
    std::string URL;
    std::string FileName;
    CONSTANTS::FileExtension FileExtension;
    std::string OutputDir;
};

struct BlobContainer {
    std::string URL;
    std::chrono::year_month_day LastModified;
    std::chrono::year_month_day FileNameDate;
};

struct SearchDateParameters {
    std::chrono::year_month_day StartDate;
    std::chrono::year_month_day EndDate;
};

CONSTANTS::FileExtension ConvertStrToFileExt(const std::string& str);
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
size_t WriteDownloadTextCallback(void* contents, size_t size, size_t nmemb, std::stringstream* buffer);
size_t WriteDownloadBinaryCallback(void* contents, size_t size, size_t nmemb, void* userp);
CURL* InitCurl();
void CurlDeleter(CURL* curl);

int PerformCurlRequest(std::shared_ptr<CURL> curl);
int PerformCurlRequest(std::shared_ptr<CURL> curl, const DownloadParameters& parameters);

void SetCurlOptions(std::shared_ptr<CURL> curl, const std::string& url, void* writeData);
int DownloadTextFile(std::shared_ptr<CURL> curl, const DownloadParameters& parameters);
int DownloadBinaryFile(std::shared_ptr<CURL> curl, const DownloadParameters& parameters);
void HandleCurlResponse(std::shared_ptr<CURL> curl, const DownloadParameters& parameters);


#endif