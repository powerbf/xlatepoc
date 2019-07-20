/*
 * localize.h
 * High-level localization functions
 */

#pragma once

#include <string>
using std::string;

// Localize a format string and a list of args.
//
// It is expected that fmt_str and any other input strings are in English.
// If you pre-translate some strings it will probably still work in most cases.
// However, there's a remote chance that the pre-translated string may match an English
// string that we've provided a translation for and be erroneously translated again.
// One theoretical case would be: "poison"(en) -> "Gift"(de) -> "Gift"(en) -> "Geschenk"(de).
string localize(const string& fmt_str, ...);

// same as localize except it capitalizes first letter
string localize_sentence(const string& fmt_str, ...);
