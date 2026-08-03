#pragma once

/**
 * I had to code a parser for yaml files (or more like a localization parser)
 * by hand instead of relying on a full-fletched yaml library for several reasons:
 * 
 * 1. Comments and the weird syntax 'key:0 "value"' break
 *    most, if not all, yaml libraries.
 * 
 * 2. Localizations files only acts as maps of strings
 *    therefore, most features such as lists, numbers, etc
 *    are not used at all, while still being relatively slow
 *    to parse.
 */

namespace Yaml {
    std::map<std::string, std::string> ParseFile(const std::string& filePath);
    std::map<std::string, std::string> ParseFile(std::ifstream& file);
    std::map<std::string, std::string> Parse(const std::string& content);
}