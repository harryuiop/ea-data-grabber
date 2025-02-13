#include <iostream>
#include <sstream>
#include "Constants.hpp"


void PrintDownloadInstructions() {
    std::ostringstream oss;
    oss << CONSTANTS::InformationInsert << "(x -> Enter) To Exit " << CONSTANTS::NewLine << CONSTANTS::NewLine;
    oss << CONSTANTS::InformationInsert << "Enter a filepath to select with an optional start date (-sd) and end date (-ed)" << CONSTANTS::DefaultEscape << CONSTANTS::NewLine << CONSTANTS::NewLine;
    oss << "      " << "Note: " << "Optionally add only a -sd or an -ed to get all specified files before or after a given date, and all" << CONSTANTS::DefaultEscape << CONSTANTS::NewLine;
    oss << "      " << "      " << "incorrectly formatted dates will be ignored" << CONSTANTS::DefaultEscape << CONSTANTS::NewLine << CONSTANTS::NewLine;
    oss << "      " << "Note: " << "If an expected filepath is not working, download a file via \"https://www.emi.ea.govt.nz/Wholesale/Datasets\"" << CONSTANTS::DefaultEscape << CONSTANTS::NewLine;
    oss << "      " << "      " << "using the network tab within dev-tools and take the path (including) \"Datasets/\" onwards," << CONSTANTS::DefaultEscape << CONSTANTS::NewLine;
    oss << "      " << "      " << "alternatively you can use the webpage URL but note \"/Datasets\" must " << CONSTANTS::DefaultEscape << CONSTANTS::NewLine;
    oss << "      " << "      " << "come before \"/Wholesale\" within the input below (e.g. \"Datasets/Wholesale/...\")" << CONSTANTS::DefaultEscape << CONSTANTS::NewLine << CONSTANTS::NewLine;
    oss << "      Example Input: Datasets/Wholesale/BidsAndOffers/Bids/2018/20180623_Bids.csv" << CONSTANTS::DefaultEscape << CONSTANTS::NewLine;
    oss << "      Example Input: Datasets/Wholesale/BidsAndOffers/Bids/ -SD YYYY/MM/DD -ED YYYY/MM/DD" << CONSTANTS::DefaultEscape << CONSTANTS::NewLine << CONSTANTS::NewLine;
    oss << CONSTANTS::InformationInsert << "If a partial path is inputted all files within that tree will be selected." << CONSTANTS::DefaultEscape << CONSTANTS::NewLine << CONSTANTS::NewLine;
    oss << "      Example Input: Datasets/Wholesale/BidsAndOffers (This will return all files with this starting predicate)" << CONSTANTS::DefaultEscape << CONSTANTS::NewLine << CONSTANTS::NewLine;

    std::cout << oss.str();
}

void PrintPrompt(const std::string& message) {
    std::cout << CONSTANTS::UserInputInsert << message;
}

void PrintError(const std::string& message) {
    std::cout << CONSTANTS::RedEscape << message << CONSTANTS::DefaultEscape << std::endl;
}

void PrintSuccess(const std::string& message) {
    std::cout << CONSTANTS::SuccessEscape << message << CONSTANTS::DefaultEscape << std::endl;
}

void PrintInfo(const std::string& message) {
    std::cout << CONSTANTS::InformationInsert << message << CONSTANTS::DefaultEscape << std::endl;
}

void PrintRedEscape(const std::string& message) {
    std::cout << CONSTANTS::RedEscape << message << CONSTANTS::DefaultEscape << std::endl;
}

void PrintPurpleEscape(const std::string& message) {
    std::cout << CONSTANTS::PurpleEscape << message << CONSTANTS::DefaultEscape << std::endl;
}

void PrintGreyEscape(const std::string& message) {
    std::cout << CONSTANTS::LightGreyEscape << message << CONSTANTS::DefaultEscape << std::endl;
}

void PrintMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void PrintNewLine() {
    std::cout << CONSTANTS::NewLine;
}