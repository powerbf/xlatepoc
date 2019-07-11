/**
 * @file
 * @brief A subset of stringutils from Crawl
 **/

#pragma once

#include <cctype>
#include <cstring>
#include <string>
using namespace std;

string lowercase_string(const string &s);
string uppercase_string(string s);

string lowercase_first(string);
string uppercase_first(string);

/**
 * Returns 1 + the index of the first suffix that matches the given string,
 * 0 if no suffixes match.
 */
int ends_with(const string &s, const char * const suffixes[]);

static inline bool starts_with(const string &s, const string &prefix)
{
    return s.rfind(prefix, 0) != string::npos;
}

static inline bool ends_with(const string &s, const string &suffix)
{
    if (s.length() < suffix.length())
        return false;
    return s.find(suffix, s.length() - suffix.length()) != string::npos;
}
