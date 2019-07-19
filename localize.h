/*
 * localize.h
 * High-level localization functions
 */

#pragma once

#include <string>
using std::string;

// localize a format string and a list of args
string localize(const string& fmt_str, ...);
