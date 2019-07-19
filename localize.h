/*
 * localize.h
 * High-level localization functions
 */

#pragma once

#include <string>
using std::string;

// localize a format string and a list of args
string localize(const string& fmt_str, ...);

// same as localize except it capitalizes first letter
string localize_sentence(const string& fmt_str, ...);
