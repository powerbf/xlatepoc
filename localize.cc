/*
 * localize.h
 * High-level localization functions
 */

#include <sstream>
#include <vector>
#include <cstdarg>
#include <cstdlib>
#include <typeinfo>
using namespace std;

#include "localize.h"
#include "xlate.h"
#include "stringutil.h"

union arg_t
{
    int i;
    long l;
    long long ll;
    intmax_t im;
    double d;
    long double ld;
    size_t sz;
    ptrdiff_t pd;
    char* s;
    int* pi;
    void* pv;
};


// check if string contains the char
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

// Extract arg id from format specifier
// (e.g. for the specifier "%2$d", return 2)
// Returns 0 if no positional specifier
static int _get_arg_id(const string& fmt)
{
    size_t dollar_pos = fmt.find('$');
    if (dollar_pos == string::npos)
    {
        return 0;
    }
    size_t pos_len = dollar_pos-2;
    if (pos_len < 1)
    {
        return 0;
    }
    string pos_str = fmt.substr(1, pos_len);
    int result = atoi(pos_str.c_str());
    if (errno != 0)
    {
        return 0;
    }
    return result;
}

// Remove arg id from arg format specifier
// (e.g. for "%1$d", return "%d")
static string _remove_arg_id(const string& fmt)
{
    size_t dollar_pos = fmt.find('$');
    if (dollar_pos == string::npos)
    {
        return fmt;
    }
    string result("%");
    if (dollar_pos < fmt.length()-1)
    {
        result += fmt.substr(dollar_pos+1, string::npos);
    }
    return result;
}

// translate a format spec like %d to a type like int
static const type_info* _format_spec_to_type(const string& fmt)
{
    if (fmt.length() < 2 || fmt.at(0) != '%')
    {
        return NULL;
    }

    char last_char = fmt.back();
    if (last_char == 'p')
    {
        return &typeid(void*);
    }
    else if (last_char == 's')
    {
        return &typeid(char*);
    }
    else if (last_char == 'n')
    {
        return &typeid(int*);
    }
    else if (_contains("aAeEfFgG", last_char))
    {
        if (fmt.at(fmt.length()-2) == 'L')
        {
            return &typeid(long double);
        }
        else
        {
            return &typeid(double);
        }
    }
    else if (_contains("uoxX", last_char))
    {
        // unsigned
        char penultimate = fmt.at(fmt.length()-2);
        if (penultimate == 'p')
        {
            return &typeid(ptrdiff_t);
        }
        else if (penultimate == 'z')
        {
            return &typeid(size_t);
        }
        else if (penultimate == 'j')
        {
            return &typeid(uintmax_t);
        }
        else if (penultimate == 'l')
        {
            if (fmt.length() > 2 && fmt.at(fmt.length()-3) == 'l')
            {
                return &typeid(unsigned long long);
            }
            else
            {
                return &typeid(unsigned long);
            }
        }
        else
        {
            return &typeid(unsigned);
        }
    }
    else if (_contains("cid", last_char))
    {
        // signed int
        char penultimate = fmt.at(fmt.length()-2);
        if (penultimate == 'p')
        {
            return &typeid(ptrdiff_t);
        }
        else if (penultimate == 'z')
        {
            return &typeid(size_t);
        }
        else if (penultimate == 'j')
        {
            return &typeid(intmax_t);
        }
        else if (penultimate == 'l')
        {
            if (fmt.length() > 2 && fmt.at(fmt.length()-3) == 'l')
            {
                return &typeid(long long);
            }
            else
            {
                return &typeid(long);
            }
        }
        else
        {
            return &typeid(int);
        }
    }

    return NULL;
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

// get arg types from format string
static map<int, const type_info*> _get_arg_types(const string& fmt)
{
    map<int, const type_info*> results;
    vector<string> strings = _split_format(fmt);
    int arg_count = 0;
    for (vector<string>::iterator it = strings.begin() ; it != strings.end(); ++it)
    {
        if (it->at(0) == '%' && it->length() > 1 && it->at(1) != '%')
        {
            ++arg_count;
            const type_info* ti = _format_spec_to_type(*it);
            int arg_id = _get_arg_id(*it);
            if (arg_id == 0)
            {
                arg_id = arg_count;
            }
            results[arg_id] = ti;
        }
    }
    return results;
}

// replace all instances of given substring
// std::regex_replace would do this, but not sure if available on all platforms
static void _replace_all(std::string& str, const std::string& patt, const std::string& replace)
{
    std::string::size_type pos = 0u;
    while((pos = str.find(patt, pos)) != std::string::npos){
        str.replace(pos, patt.length(), replace);
        pos += replace.length();
    }
}

static void _resolve_escapes(string& str)
{
    _replace_all(str, "%%", "%");
    _replace_all(str, "\\{", "{");
    _replace_all(str, "\\}", "}");
}

string localize(const string& fmt_string, va_list& args)
{
    // get arg types for origin English string
    map<int, const type_info*> arg_types = _get_arg_types(fmt_string);

    // translate format string
    string fmt_xlated = dxlate("messages", fmt_string);

    vector<string> strings = _split_format(fmt_xlated);

    // store args in map
    map<int, arg_t> arg_map;
    int arg_count = 0;
    for (vector<string>::iterator it = strings.begin() ; it != strings.end(); ++it)
    {
        if (it->length() < 2 || it->at(0) != '%' || it->at(1) == '%')
        {
            // not a format spec
            continue;
        }

        // determine arg id
        ++arg_count;
        int arg_id = _get_arg_id(*it);
        arg_id = (arg_id == 0 ? arg_count : arg_id);

        const type_info* ti = arg_types[arg_id];

        arg_t arg;
        arg.ll = 0;
        if (ti == NULL)
        {
            // do nothing
            va_arg(args, void*); // pop one arg
        }
        else if (*ti == typeid(int*))
        {
            arg.pi = va_arg(args, int*);
        }
        else if (*ti == typeid(char*))
        {
            arg.s = va_arg(args, char*);
        }
        else if (*ti == typeid(void*))
        {
            arg.pv = va_arg(args, void*);
        }
        else if (*ti == typeid(long double))
        {
            arg.ld = va_arg(args, long double);
        }
        else if (*ti == typeid(double))
        {
            arg.d = va_arg(args, double);
        }
        else if (*ti == typeid(ptrdiff_t))
        {
            arg.pd = va_arg(args, ptrdiff_t);
        }
        else if (*ti == typeid(size_t))
        {
            arg.sz = va_arg(args, size_t);
        }
        else if (*ti == typeid(intmax_t) || *ti == typeid(uintmax_t))
        {
            arg.im = va_arg(args, intmax_t);
        }
        else if (*ti == typeid(long long) || *ti == typeid(unsigned long long))
        {
            arg.ll = va_arg(args, long long);
        }
        else if (*ti == typeid(long) || *ti == typeid(unsigned long))
        {
            arg.l = va_arg(args, long);
        }
        else
        {
            arg.i = va_arg(args, int);
        }
        arg_map.insert(pair<int, arg_t>(arg_id, arg));
    }

    ostringstream ss;

    string context;
    arg_count = 0;
    for (vector<string>::iterator it = strings.begin() ; it != strings.end(); ++it)
    {
        if (it->at(0) == '{' && it->length() > 1)
        {
            context = it->substr(1, it->length() - 2); // strip curly brackets
        }
        else if (it->at(0) == '%' && it->length() > 1 && it->at(1) != '%')
        {
            ++arg_count;
            int arg_id = _get_arg_id(*it);
            arg_id = (arg_id == 0 ? arg_count : arg_id);

            map<int, const type_info*>::iterator type_entry = arg_types.find(arg_id);
            map<int, arg_t>::const_iterator arg_entry = arg_map.find(arg_id);
            if (type_entry != arg_types.end() && arg_entry != arg_map.end())
            {
                string s;
                string fmt_spec = _remove_arg_id(*it);
                const type_info* type = _format_spec_to_type(fmt_spec);
                const type_info* expected_type = type_entry->second;
                const arg_t arg = arg_entry->second;

                if (expected_type == NULL || type == NULL || *type != *expected_type)
                {
                    // something's wrong - skip this arg
                }
                else if (*type == typeid(int*))
                {
                    // expects us to write into the the int pointed to
                    // I don't think we want to do this
                }
                else if (*type == typeid(char*))
                {
                    // arg is string and needs to be localized
                    string argx = dcxlate("entities", context, arg.s);
                    s = make_stringf(fmt_spec.c_str(), argx.c_str());
                }
                else if (*type == typeid(void*))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.pv);
                }
                else if (*type == typeid(long double))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.ld);
                }
                else if (*type == typeid(double))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.d);
                }
                else if (*type == typeid(ptrdiff_t))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.pd);
                }
                else if (*type == typeid(size_t))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.sz);
                }
                else if (*type == typeid(intmax_t) || *type == typeid(uintmax_t))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.im);
                }
                else if (*type == typeid(long long) || *type == typeid(unsigned long long))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.ll);
                }
                else if (*type == typeid(long) || *type == typeid(unsigned long))
                {
                    s = make_stringf(fmt_spec.c_str(), arg.l);
                }
                else
                {
                    s = make_stringf(fmt_spec.c_str(), arg.i);
                }
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

