

#ifndef CROSS_PROCESS_HOOK_MYEXCEPTION_H
#define CROSS_PROCESS_HOOK_MYEXCEPTION_H

#include <stdexcept>
#include <Windows.h>

class MyException : public std::exception {
private:
    std::string m_ErrorMsg;
public:
    MyException(std::string ErrorMsg, const char *Function);

    MyException(std::string ErrorMsg, DWORD ErrorCode, const char *Function);

    [[nodiscard]] const char *what() const override;
};


#endif //CROSS_PROCESS_HOOK_MYEXCEPTION_H
