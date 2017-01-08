#ifndef USEREXCEPTION_HPP
#define USEREXCEPTION_HPP

#include <exception>

class UserException : public std::runtime_error
{
public:
    explicit UserException(std::string msg)
        : runtime_error(msg)
    {
    }
};

#endif // USEREXCEPTION_HPP
