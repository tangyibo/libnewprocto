#include "exception_error.h"
#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

namespace olptnet{
using namespace std;

Exception::Exception(const char* msg)
: message_(msg)
{
    fillStackTrace();
}

Exception::Exception(const string& msg)
: message_(msg)
{
    fillStackTrace();
}

Exception::~Exception() throw ()
{
    message_.clear();
    stack_.clear();
}

const char* Exception::what() const throw ()
{
    return message_.c_str();
}

const char* Exception::stackTrace() const throw ()
{
    return stack_.c_str();
}

void Exception::fillStackTrace()
{
    const int len = 256;
    void* buffer[len];
    int nptrs = ::backtrace(buffer, len); //列出当前函数调用关系
    //将从backtrace函数获取的信息转化为一个字符串数组
    char** strings = ::backtrace_symbols(buffer, nptrs);

    if (strings)
    {
        for (int i = 0; i < nptrs; ++i)
        {
            std::stringstream ss;
            ss << "\n >>>[" << i << "] ";
            stack_.append(ss.str());
            stack_.append(demangle(strings[i]));
        }

        //要对backtrace_symbols的返回值strings调用free
        free(strings);
    }
}

string Exception::demangle(const char* symbol)
{
    size_t size;
    int status;
    char temp[128];
    char* demangled;
    //first, try to demangle a c++ name
    if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp))
    {
        if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, &size, &status)))
        {
            string result(demangled);
            free(demangled);
            return result;
        }
    }
    //if that didn't work, try to get a regular c symbol
    if (1 == sscanf(symbol, "%127s", temp))
    {
        return temp;
    }

    //if all else fails, just return the symbol
    return symbol;
}

}//end namespace