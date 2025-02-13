#ifndef XMLUTILS_HPP
#define XMLUTILS_HPP

#include <string>
#include <vector>
#include "CurlUtilsWrap.hpp"

void _ExtractValueFromXMLTag(const std::string& tagToMatch, const std::string& inputString, std::vector<std::string>& outputVector);
std::string _ExtractValueFromXMLTag(const std::string& tagToMatch, const std::string& inputObject);
void ParseEAXML(std::vector<BlobContainer>& outputVector, std::string& inputObject, std::string* nextMarker);

#endif