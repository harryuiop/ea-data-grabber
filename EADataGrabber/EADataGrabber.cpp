/*
 * This is a small tool that helps users download files from the New Zealand Electricity Authority's azure 
 *  container database in a quick and effective manner.
 *
 * Developed by Harry Ellis (Orbit Systems)
 * Last Updated 03/02/2025
 *
 * License: Apache License 2.0 
 */

#define CURL_STATICLIB
#define ClearTerminal system("cls");

#include <memory>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>   
#include <optional>
#include <sstream>   
#include "Constants.hpp"
#include "CurlUtilsWrap.hpp"
#include "WinUtilsWrap.hpp"
#include "DateUtilsWrap.hpp"
#include "XMLUtils.hpp"
#include "ConsoleUtils.hpp"
#include "curl/curl.h"

using namespace CONSTANTS;

/*
* Extracts the value following from given arguments in the form of "-Argument_Tag Argument_Value"
*   Whitespace between the argument tag and value is required, along with the leading "-"
* 
* @param argument - The argument tag we want the value of, including the "-"
* @param input - The entire strin, often including the query path
* @return the argument value if found, nullopt if not found
*/
std::optional<std::string> ExtractInputArguments(const std::string& argument, const std::string input) {
   size_t argPos = input.find(argument);
    if (argPos != std::string::npos) {
        size_t startPos = argPos + argument.length() + 1; // + 1 to start on the ' ' not the end last char of arg
        size_t endPos = input.find(' ', startPos);

        if (endPos == UINT64_MAX && startPos != std::string::npos) { // if .find() dosent find a ' ', it defaults to UINT64_MAX
            endPos = input.size();
        }

        if (!(endPos - startPos == ExpectedDateArgumentLengthShort || 
            endPos - startPos == ExpectedDateArgumentLengthLong)) {
            return std::optional<std::string>();
        }

        if (endPos == std::string::npos) {
            endPos = input.length();
        }
        return std::optional<std::string>(input.substr(startPos, endPos - startPos)); 
    }
    return std::optional<std::string>();
}

/*
*  Wrapper function to parse a console user input into date objects
*
* @param input - Entire CLI stirng (not just a chunck using cin <<)
* @return std::chrono ymd objects within a wrapper
*/
SearchDateParameters ExtractSearchDatesCLI(const std::string& input) {
    std::chrono::year_month_day startDate = std::chrono::year_month_day{}; // Empty date objects
    std::chrono::year_month_day endDate = std::chrono::year_month_day{};

    std::optional<std::string> startDateStr = ExtractInputArguments(StartDateStrArg, toLowercase(input)); // Hand off to see if a valid date/argument string exists
    std::optional<std::string> endDateStr = ExtractInputArguments(EndDateStrArg, toLowercase(input));

    if (startDateStr.has_value()) {
        std::optional<std::chrono::year_month_day> optionalStartDate = StringToDateCLIArg(startDateStr.value());
        if (optionalStartDate.has_value()) {
            startDate = optionalStartDate.value();
        }
    }
    if (endDateStr.has_value()) {
        std::optional<std::chrono::year_month_day> optionalEndDate = StringToDateCLIArg(endDateStr.value());
        if (optionalEndDate.has_value()) {
            endDate = optionalEndDate.value();
        }
    }
    return SearchDateParameters{ startDate, endDate };
}

/*
* Outputs contents of a BolbContainer into the console
*
* @param outputArray - Vector to be displayed
*/
void PrintBlobContainer(std::vector<BlobContainer> outputArray) {
    ClearTerminal;
    int counter = 0;
    for (const auto& match : outputArray) {
        std::string str = match.URL;
        size_t pos = str.find(BaseURL);
        counter++;

        if (pos == 0) {  
            str.erase(pos, BaseURL.length()); // Remove Base URL
        }
        if (counter % 2 == 0) {
            PrintMessage(str);
        }
        else {
            PrintGreyEscape(str);
        }
    }
    PrintNewLine();
    PrintInfo(std::to_string(outputArray.size()) + " Containers Found");
}

/*
* Makes an inital HTTPS request to EA, parses the recieved XML data and repeats until the incoming response has an empty
* "nextMarker" tag
*
* @param outputArray - Vector to add the parsed "Blob" containers into
* @param curl - Pointer to the singleton curl object holding configuration information
*/
void RunGetContainers(std::vector<BlobContainer>& outputArray, std::shared_ptr<CURL> curl, const std::string& prefix = "") {    
    std::string responseData = "";
    std::string nextMarker = "";

    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseData);
    curl_easy_setopt(curl.get(), CURLOPT_URL, (AllContainersURL + prefix).c_str());

    int curlResultBool = PerformCurlRequest(curl);
    if (curlResultBool != 0) {
        return;
    }
    
    while (1) {
        ParseEAXML(outputArray, responseData, &nextMarker);

        if (nextMarker.empty()) {
            return;
        }

        responseData = ""; // curl will not overwrite data, so we must empty ourself

        std::string updatedUrl = AllContainersURL + prefix + "&marker=" + nextMarker; // Add new nextMarker so we get the next page
        curl_easy_setopt(curl.get(), CURLOPT_URL, updatedUrl.c_str());

        nextMarker = ""; 

        int curlResultBool = PerformCurlRequest(curl);
        if (curlResultBool != 0) {
            return;
        }
    }
}

/*
* Grabs a user inputted directory path and checks to see if it is valid, if so, updates it
*
* @param outputDirectory - The programs stored output directory location 
*/
void ChangeOutputDirectory(std::string& outputDirectory) {
    ClearTerminal;

    while (true) {
        PrintInfo("(x -> Enter) To Exit ");
        PrintNewLine();
        PrintPrompt("Enter new output Directory (Full System Path): ");
        std::string path = "";

        std::getline(std::cin, path);

        if (path.at(0) == ' ') {
            path = removeLeadingWhitespace(path);
        }

        if (toLowercase(path) == "x") {
            ClearTerminal;
            return;
        }

        if (directoryExists(path)) {
            outputDirectory = path;
            ClearTerminal;
            PrintSuccess("Output directory changed to " + outputDirectory);
            return;
        }
        else {
            ClearTerminal;
            PrintError("Directory does not exist!");
        }
    }
}

/*
* Wrapper for taking (or predefining) a user input to a container path optionally including a start/end date
*
* @param preDefinedInput - An optional input if you do not want the program to stop and ask the user
* @param curl - Pointer to the singleton curl object holding configuration information
* @return 
*/
std::vector<BlobContainer> FuzzyFindContainers(std::shared_ptr<CURL> curl, const std::string preDefinedInput = "") {
    using namespace std::chrono;

    std::vector<BlobContainer> allContainers;
    std::vector<BlobContainer> selectedContainers;
    std::string fuzzyUserInput;
    std::string queryPath;
    std::smatch queryPathMatch;
    std::regex pattern(R"(^\s*[^-\s]+)"); // Matches only the path and not the possible search dates
    SearchDateParameters searchDate;

    if (preDefinedInput.empty()) {
        ClearTerminal;
        PrintInfo("(x -> Enter) To Exit ");
        PrintPrompt("Enter a keyword to search for: ");
        std::getline(std::cin, fuzzyUserInput);

        if (toLowercase(fuzzyUserInput) == "x") {
            ClearTerminal;
            return allContainers;
        }
    }
    else {
        fuzzyUserInput = preDefinedInput;
    }

    if (std::regex_search(fuzzyUserInput, queryPathMatch, pattern)) { // Removes the search dates to be passed elsewhere
        queryPath = queryPathMatch.str();
    }
    else {
        queryPath = "";
    }

    RunGetContainers(allContainers, curl, queryPath);
    searchDate = ExtractSearchDatesCLI(fuzzyUserInput);

    if (searchDate.StartDate == DefaultDate && searchDate.EndDate == DefaultDate) {
        return allContainers;
    }

    for (BlobContainer Item : allContainers) { // Search parameters have been included, we must now filter 

        if (searchDate.StartDate != DefaultDate && searchDate.EndDate == DefaultDate) {
            if (Item.FileNameDate > searchDate.StartDate) {
                selectedContainers.push_back(Item);
            }
        }
        else if (searchDate.StartDate == DefaultDate && searchDate.EndDate != DefaultDate) {
            if (Item.FileNameDate < searchDate.EndDate) {
                selectedContainers.push_back(Item);
            }
        }
        else {
            if (Item.FileNameDate > searchDate.StartDate && Item.FileNameDate < searchDate.EndDate) {
                selectedContainers.push_back(Item);
            }
        }
    }
    PrintBlobContainer(selectedContainers);
    return selectedContainers;
}

/*
* Entry into the file download process, takes a users input, hands it off to make an API request to find all related containers
*   then iterates through, downloading each.
*
* @param outputDir - Location to download the files into
* @param curl - Pointer to the singleton curl object holding configuration information
*/
void RunDownload(std::shared_ptr<CURL> curl, const std::string outputDir) {
    ClearTerminal;

    std::string userInput = "";
    std::vector<BlobContainer> selectedContainers;

    while (userInput.empty()) {
        ClearTerminal;
        PrintDownloadInstructions();
        PrintPrompt(": ");
        std::getline(std::cin, userInput);
        PrintNewLine();
    }

    if (toLowercase(userInput) == "x") {
        ClearTerminal;
        return;
    }

    if (userInput.at(0) == ' ') {
        userInput = removeLeadingWhitespace(userInput);
    }

    selectedContainers = FuzzyFindContainers(curl, userInput);
    PrintBlobContainer(selectedContainers);

    if (selectedContainers.empty()) {
        ClearTerminal;
        PrintInfo("No containers matching the input: " + userInput);
        return;
    }

    while (true) {
        PrintInfo("Would you like to download all of the following files ? (y / n)");
        PrintNewLine();
        PrintPrompt(": ");
        std::getline(std::cin, userInput);
        std::pair<int, int> downloadResults{ 0, 0 }; // .first is successful download count, .second is failed
        std::vector<std::string> failedDownloadUrls;

        if (userInput == "y") {
            for (BlobContainer File : selectedContainers) {
                std::string FileExt = getFileExtension(File.URL);
                std::string FileName = getFileName(File.URL);

                if (!FileExt.empty() && !FileName.empty()) {
                    DownloadParameters parameters{
                        .URL = File.URL,
                        .FileName = FileName,
                        .FileExtension = ConvertStrToFileExt(FileExt),
                        .OutputDir = outputDir
                    };
                    int result = PerformCurlRequest(curl, parameters);
                    if (result == 0) { 
                        downloadResults.first = downloadResults.first + 1; // Successful Download
                    }
                    else {
                        downloadResults.second = downloadResults.second + 1;
                        failedDownloadUrls.push_back(FileName + "." + FileExt);
                    }
                }
            }

            ClearTerminal;
            if (failedDownloadUrls.size() > 0) {
                for (std::string url : failedDownloadUrls) {
                    std::cout << "  " << RedEscape << url << NewLine;
                }
                PrintRedEscape("  Failed Downloads");
                PrintNewLine();
            }
            PrintInfo(std::to_string(downloadResults.first) + " File(s) Downloaded Successfully");
            PrintInfo(std::to_string(downloadResults.second) + " File(s) Downloads Failed");
            return;
        }
        else if (userInput == "n") {
            ClearTerminal;
            return;
        }
        else {
            ClearTerminal;
            PrintBlobContainer(selectedContainers);
            std::cout << NewLine << InformationInsert << RedEscape << "\"" + userInput + "\"" << " is not a valid input" << DefaultEscape << NewLine << NewLine;
        }
    }
}

/*
* Main menu screen and entry 
*/
int main(void) {
    std::shared_ptr<CURL> curl(InitCurl(), CurlDeleter);
    std::string outputDirectory = getDownloadsFolderPath();
    std::string userInput;

    PrintPurpleEscape("** Quickly download files from the New Zealand Electrity Authority  -  Orbit Systems **");

    while (true) {
        PrintNewLine();
        PrintMessage(CONSTANTS::MenuOption1);
        PrintMessage(CONSTANTS::MenuOption2);
        PrintMessage(CONSTANTS::MenuOption3);
        PrintNewLine();
        PrintMessage("  Output Directory : " + outputDirectory);
        PrintNewLine();
        PrintPrompt("Please enter a corrosponding key to continue: ");
        std::getline(std::cin, userInput);
        PrintNewLine();

        int userInputInt = userInput[0] - '0'; 

        switch (userInputInt) {
        case 1:
            RunDownload(curl, outputDirectory);
            break;
        case 2:
            ChangeOutputDirectory(outputDirectory);
            break;
        case 3:
            return 0;
        default:
            ClearTerminal;
            PrintRedEscape(CONSTANTS::InvalidInputMessage);
            break;
        }
    }
}

// make it so you are warned if the date is in the wrong order
