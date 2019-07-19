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


// split format string into constants and format specifiers
vector<string> split_format(const string& fmt_str)
{
    vector<string> results;
    int fmt_len = fmt_str.length();

    int token_start = 0;
    while (token_start < fmt_len)
    {
        int token_len = 0;
        if (fmt_str.at(token_start) == '%' && token_start < fmt_len - 1 && fmt_str.at(token_start+1) != '%')
        {
            // extract format spec
            token_len++;
            char curr;
            do {
                token_len++;
                curr = fmt_str.at(token_start+token_len-1);
            } while (token_start + token_len < fmt_len && !isalpha(curr));
        }
        else if (fmt_str.at(token_start) == '{' && token_start < fmt_len - 1)
        {
            // context specifier
            token_len = 1;
            while (token_start + token_len < fmt_len && fmt_str.at(token_start+token_len-1) != '}')
            {
                token_len++;
            }
        }
        else
        {   bool finished = false;
            do {
                token_len++;
                if (token_start + token_len == fmt_len)
                {
                    finished = true;
                }
                else
                {
                    char next = fmt_str.at(token_start + token_len);
                    if (next == '{')
                    {
                        finished = true;
                    }
                    else if (next == '%')
                    {
                        int next_next_idx = token_start + token_len + 1;
                        if (next_next_idx == fmt_len)
                        {
                            finished = true;
                        }
                        else if (fmt_str.at(next_next_idx) == '%')
                        {
                            token_len++; // skip next
                        }
                        else
                        {
                            finished = true;
                        }
                    }
                }
            } while (!finished);
        }

        results.push_back(fmt_str.substr(token_start, token_len));
        token_start += token_len;
    }
    return results;
}

string localize(const string& format, ...)
{
    string format2 = dxlate("messages", format);
    vector<string> strings = split_format(format2);
    ostringstream ss;
    char buf[8000];

    va_list args;
    va_start(args, format);

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
            context = "";
        }
        else
        {
            // plain string
            ss << *it;
            context = "";
        }
    }

    va_end(args);

    return ss.str();
}


