# EA Data Grabber

## Overview
EADataGrabber is a C++ command-line tool designed to help users download large batches of files from the New Zealand Electricity Authority (NZEA) based on user-defined conditions. It includes filtering, error handling, and search capabilities to give users a MUCH faster way of downloading files
from NZEA's system.

## Features
- **Batch File Downloading**: Automates the retrieval of multiple files in a single run.
- **File Search Autocomplete**: Allows users to find relevant files even if filenames are not an exact match.
- **Filename Date Parsing**: Extracts date information from filenames to filter downloads based on specified date ranges.
- **Date-Based Filtering**: Users can predefine a date range to limit downloads to specific time periods.
- **Error Reporting**: Logs errors for debugging and troubleshooting.

## Tech Stack
- **Language**: C++
- **Networking**: cURL (libcurl)
- **File Handling**: Standard C++ File I/O
- **Parsing & Filtering**: Regex and C++17 date/time utilities

## Installation
1. Clone the repository:
   ```sh
   git clone https://github.com/harryuiop/EADataGrabber
   ```
2. Install dependencies (e.g., `libcurl`). On Debian-based systems:
   ```sh
   sudo apt-get install libcurl4-openssl-dev
   ```
3. Compile the project:
   ```sh
   g++ -std=c++17 -o EADataGrabber main.cpp -lcurl
   ```

## Usage
Enter a filepath to select with an optional start date (-sd) and end date (-ed)

 Optionally add only a -sd or an -ed to get all specified files before or after a given date, and all
 incorrectly formatted dates will be ignored

 If an expected filepath is not working, download a file via "https://www.emi.ea.govt.nz/Wholesale/Datasets"
 using the network tab within dev-tools and take the path (including) "Datasets/" onwards,
 alternatively you can use the webpage URL but note "/Datasets" must
 come before "/Wholesale" within the input below (e.g. "Datasets/Wholesale/...")

    Example Input: Datasets/Wholesale/BidsAndOffers/Bids/2018/20180623_Bids.csv
    Example Input: Datasets/Wholesale/BidsAndOffers/Bids/ -SD YYYY/MM/DD -ED YYYY/MM/DD
    
## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


