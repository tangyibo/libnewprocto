#ifndef _EXCEPTION_HEADER_H_
#define _EXCEPTION_HEADER_H_
#include <string>
#include <exception>

namespace olptnet{
    
class Exception : public std::exception
{
public:
    explicit Exception ( const char* what );
    explicit Exception ( const std::string& what );
    virtual ~Exception ( ) throw ( );
    
    virtual const char* what ( ) const throw ( );
    const char* stackTrace ( ) const throw ( );

private:
    void fillStackTrace ( ); //填充栈痕迹
    std::string demangle ( const char* symbol ); //反名字改编

    std::string message_; //异常信息
    std::string stack_; //栈trace
};

}//end namespace
#endif  // EXCEPTION_H_