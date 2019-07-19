/*
 * localize.h
 * High-level localization functions
 */

#include <sstream>
#include <vector>
#include <cstdarg>
using namespace std;

#include "localize.h"
#include "xlate.h"
#include "stringutil.h"

// check if string conatins the char
static inline bool _contains(const std::string& s, char c)
{
    return (s.find(c) != string::npos);
}

// is this char a printf typespec (i.e. the end of %<something><char>)?
static inline bool _is_type_spec(char c)
{
    static const string type_specs = "diufFeEgGxXoscpaAn";
    return (type_specs.find(c) != string::npos);
}

// split format string into constants and format specifiers
static vector<string> _split_format(const string& fmt_str)
{
    vector<string> results;
    int fmt_len = fmt_str.length();

    int token_start = 0;
    int token_len = 0;
    const char* ch = fmt_str.c_str();

    while (*ch != '\0')
    {
        token_start += token_len;
        token_len = 1;
        bool finished = false;

        if (*ch == '%' && *(ch+1) != '%')
        {
            // read format specifier
            ++ch;
            while (!finished)
            {
                finished = (*ch == '\0' || _is_type_spec(*ch));
                if (*ch != '\0')
                {
                    ++token_len;
                    ++ch;
                }
            }
        }
        else if (*ch == '{')
        {
            // read context specifier
            ++ch;
            while (!finished)
            {
                finished = (*ch == '\0' || *ch == '}');
                if (*ch != '\0')
                {
                    ++token_len;
                    ++ch;
                }
            }
        }
        else
        {
            // read literal string
            while (!finished)
            {
                bool escaped = (*ch == '\\' || (*ch == '%' && *(ch+1) == '%'));
                ++ch;
                if (*ch == '\0')
                {
                    finished = true;
                }
                else if (escaped)
                {
                    // ignore character
                    ++token_len;
                    finished = false;
                }
                else
                {
                    finished = (*ch == '%' || *ch == '{');
                    if (!finished)
                    {
                        ++token_len;
                    }
                }
            }
        }

        // extract token
        results.push_back(fmt_str.substr(token_start, token_len));
    }
    return results;
}

// string replace
// std::regex_replace would do this, but not sure if available on all platforms
void _replace(std::string& str, const std::string& patt, const std::string& replace)
{
    std::string::size_type pos = 0u;
    while((pos = str.find(patt, pos)) != std::string::npos){
        str.replace(pos, patt.length(), replace);
        pos += replace.length();
    }
}
static void _resolve_escapes(string& str)
{
    _replace(str, "%%", "%");
    _replace(str, "\\{", "{");
    _replace(str, "\\}", "}");
}

string localize(const string& fmt_string, va_list& args)
{
    static const string double_type_specs = "aAeEfFgG";

    string format2 = dxlate("messages", fmt_string);
    vector<string> strings = _split_format(format2);
    ostringstream ss;

    string context;
    for (vector<string>::iterator it = strings.begin() ; it != strings.end(); ++it)
    {
        if (it->at(0) == '{' && it->length() > 1)
        {
            context = it->substr(1, it->length() - 2); // strip curly brackets
        }
        else if (it->at(0) == '%' && it->length() > 1 && it->at(1) != '%')
        {
            if (it->back() == 's')
            {
                // arg is string and needs to be localized
                char* arg = va_arg(args, char*);
                string argx = dcxlate("entities", context, (char*)arg);
                string s = make_stringf(it->c_str(), argx.c_str());
                ss << s;
            }
            else
            {
                string s = vmake_stringf(it->c_str(), args);
                va_arg(args, void*); // pop one arg
                ss << s;
            }
        }
        else
        {
            // plain string (but could have escapes)
            string str = *it;
            _resolve_escapes(str);
            ss << str;
        }
    }

    return ss.str();
}

string localize(const string& fmt_str, ...)
{
    va_list args;
    va_start(args, fmt_str);

    string result = localize(fmt_str, args);

    va_end(args);

    return result;
}

// same as localize except it capitalizes first letter
string localize_sentence(const string& fmt_str, ...)
{
    va_list args;
    va_start(args, fmt_str);

    string result = localize(fmt_str, args);

    va_end(args);

    return uppercase_first(result);
}

