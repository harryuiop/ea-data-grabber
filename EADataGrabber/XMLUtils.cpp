#include <string>
#include <regex>
#include "CurlUtilsWrap.hpp"
#include "DateUtilsWrap.hpp"

/*
*  This file makes up 90% of the heavy lifting in the program. If the program inputs have gotten large and it is not running fast 
*    enough, these functions will need to be actually optimised, as the implementations are all currently very primitive.
*/

/* 
* Takes an XML string and adds the entire substring found between a given XML tag for all found matching tags 
* 
* @param tagToMatch - The name of the XML tag we are looking for
* @param inputString - XML file loaded into a standard string
* @param outputVector - The vector to add all of the found tags contents too
*/
void _ExtractValueFromXMLTag(const std::string& tagToMatch, const std::string& xmlString, std::vector<std::string>& outputVector) {

    std::regex pattern("<" + tagToMatch + ">(.*?)</" + tagToMatch + ">", std::regex::icase);

    auto begin = std::sregex_iterator(xmlString.begin(), xmlString.end(), pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        std::string FullString = match.str(1);

        outputVector.push_back(FullString);
    }
}

/*
* Takes an XML string and returns the entire substring found between the first match
*
* @param tagToMatch - The name of the XML tag we are looking for
* @param inputString - XML file loaded into a standard string
*/
std::string _ExtractValueFromXMLTag(const std::string& tagToMatch, const std::string& xmlString) {

    std::regex pattern("<" + tagToMatch + ">(.*?)</" + tagToMatch + ">", std::regex::icase);

    auto begin = std::sregex_iterator(xmlString.begin(), xmlString.end(), pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        std::string fullString = match.str(1);

        return fullString;
    }

    return "";
}

/*
* Takes a URL and finds 6 or 8 consecutive numbers delimited each side by '/', '-', '_' or end/start of string
*   e.g. input /Environment/HydrologicalModellingDataset/2_Flows_20231231 will return "20231221" (21st Dec 2023)
*
* @param input - a URL thats contains an isolated date in the format YYYYMMDD or YYYYMM
* @return a nullopt if we cannot find a date or the date found
*/
std::optional<std::string> _ExtractStrDateFromUrl(const std::string& urlString) {
    std::regex pattern(R"((?:^|[/_-])(\d{6}(?:\d{2})?)(?:[/_-]|$))");

    auto begin = std::sregex_iterator(urlString.begin(), urlString.end(), pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        std::string matchedStr = match.str(1);

        return std::optional<std::string>(matchedStr);
    }

    return std::optional<std::string>();
}
 
/*
* Wrapper to call to get to a std::chrono::year_month_day object from a URL string that contains an isolated 
*   YYYYMMDD or YYYYMM 
* 
* @param input - URL string to search
* @return a std::chrono::year_month_day object containing the data found or a default dated std::chrono::year_month_day object
*/
std::chrono::year_month_day GetDateFromUrl(const std::string& urlString) {
    std::optional<std::string> strDate = _ExtractStrDateFromUrl(urlString);
    if (strDate.has_value()) {
        std::optional<std::chrono::year_month_day> parsedDate = ParseDate(strDate.value());

        return parsedDate.has_value() ? ParseDate(strDate.value()).value() : std::chrono::year_month_day{};
    }
}

/*
* Takes in a XML string and grabs the enitre substring between the tags "Blob" and "NextMarker" then creates a BlobContainer object 
*   using the contents and updates the passed in nextMarker variable.
* 
* @outputVector - The vector storing the created BlobContainers
* @xmlString - the xml string we are parsing
* @nextMarker - pointer to a nextMarker variable to update
*/
void ParseEAXML(std::vector<BlobContainer>& outputVector, std::string& xmlString, std::string* nextMarker) {
    std::vector<BlobContainer> blobs;
    std::regex pattern("(<Blob>(.*?)<\/Blob>|<NextMarker>(.*?)<\/NextMarker>)", std::regex::icase);

    auto begin = std::sregex_iterator(xmlString.begin(), xmlString.end(), pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        std::string innerTagString = match.str(1);
        std::string fullString = match.str(0);

        if (fullString.find("/Blob") != std::string::npos) {
            std::string strDate = _ExtractValueFromXMLTag("Last-Modified", innerTagString);
            std::string url = _ExtractValueFromXMLTag("Url", innerTagString);
            std::chrono::year_month_day fileNameDate = GetDateFromUrl(url);

            outputVector.push_back(
                BlobContainer{
                    .URL = url,
                    .LastModified = ParseDate(strDate).has_value() ? ParseDate(strDate).value() : std::chrono::year_month_day(),
                    .FileNameDate = fileNameDate
                }
            );
        }
        else if (fullString.find("<NextMarker>") != std::string::npos) {
            *nextMarker = match.str(3);
        }
    }
}