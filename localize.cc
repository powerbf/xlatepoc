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

string localize(const string& fmt_string, va_list& args)
{
    string format2 = dxlate("messages", fmt_string);
    vector<string> strings = _split_format(format2);
    ostringstream ss;
    char buf[8000];

    string context;
    for (vector<string>::iterator it = strings.begin() ; it != strings.end(); ++it)
    {
        if (it->at(0) == '{' && it->length() > 1)
        {
            context = it->substr(1, it->length() - 2); // strip curly brackets
        }
        else if (it->at(0) == '%' && it->length() > 1 && it->at(1) != '%')
        {
            void* arg = va_arg(args, void*);
            if (it->back() == 's')
            {
                // arg is string and needs to be localized
                string argx = dcxlate("entities", context, (char*)arg);
                sprintf(buf, it->c_str(), argx.c_str());
            }
            else
            {
                sprintf(buf, it->c_str(), arg);
            }
            ss << buf;
        }
        else
        {
            // plain string
            ss << *it;
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

