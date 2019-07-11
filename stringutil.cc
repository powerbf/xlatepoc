/**
 * @file
 * @brief String manipulation functions that don't fit elsewhere.
 **/

#include "stringutil.h"
#include "unicode.h"

#include <cwctype>
#include <sstream>

string lowercase_string(const string &s)
{
    string res;
    char32_t c;
    char buf[4];
    for (const char *tp = s.c_str(); int len = utf8towc(&c, tp); tp += len)
        res.append(buf, wctoutf8(buf, towlower(c)));
    return res;
}

string &lowercase(string &s)
{
    s = lowercase_string(s);
    return s;
}

string &uppercase(string &s)
{
    for (char &ch : s)
        ch = toupper(ch);

    return s;
}

string uppercase_string(string s)
{
    return uppercase(s);
}


// Warning: this (and uppercase_first()) relies on no libc (glibc, BSD libc,
// MSVC crt) supporting letters that expand or contract, like German ß (-> SS)
// upon capitalization / lowercasing. This is mostly a fault of the API --
// there's no way to return two characters in one code point.
// Also, all characters must have the same length in bytes before and after
// lowercasing, all platforms currently have this property.
//
// A non-hacky version would be slower for no gain other than sane code; at
// least unless you use some more powerful API.
string lowercase_first(string s)
{
    char32_t c;
    if (!s.empty())
    {
        utf8towc(&c, &s[0]);
        wctoutf8(&s[0], towlower(c));
    }
    return s;
}

string uppercase_first(string s)
{
    // Incorrect due to those pesky Dutch having "ij" as a single letter (wtf?).
    // Too bad, there's no standard function to handle that character, and I
    // don't care enough.
    char32_t c;
    if (!s.empty())
    {
        utf8towc(&c, &s[0]);
        wctoutf8(&s[0], towupper(c));
    }
    return s;
}

int ends_with(const string &s, const char * const suffixes[])
{
    if (!suffixes)
        return 0;

    for (int i = 0; suffixes[i]; ++i)
        if (ends_with(s, suffixes[i]))
            return 1 + i;

    return 0;
}
