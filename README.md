# EA Data Grabber
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

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

## Build/Develop Setup
##### This is not the only way to get this setup but, IMO, the most simple. This is assumes you are developing on a Windows system. 
1. Clone the repository:
   ```sh
   git clone https://github.com/harryuiop/EADataGrabber
   ```
2. Install Visual Studio 2022 (Not Visual Studio Code) with the following packages,
   - MSVC v143 - VS 2022 C++ [Your Architecture] build tools (This is our c++ complier)
3. Open the project
   - Locate the .sln file in the cloned repo, right click it and open with, Visual Studio 2022
4. Prepare dependencies (e.g., `libcurl`) **Only Required if unable to link/inlude libcurl**:
   - We now need to get the linker to recognise our libcurl library (Which is within the project as a pre-built binary)
      - In the toolbar, Project -> Properties -> Linker -> Input -> Additional Dependencies, add the path to ```\Dependencies\libCURL\x64-Release\lib\libcurl_a.lib```, delimited by ';'
      - Under Project -> Properties -> C/C++ -> General -> Addtional Include Directories, add the path to ```\Dependencies\libCURL\x64-Release\include```
     </br> Note: You have avaliable the variables '$(ProjectDir)' and '$(SolutionDir)' and to do this for each configuration/platform 
5. Compile the project:
   - Visual Studio now should offer you the abaility to compile/build/run the program via the tool bars.

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


