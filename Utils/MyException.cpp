

#include "MyException.h"
#include <string>

MyException::MyException(std::string ErrorMsg, const char *Function)
        : m_ErrorMsg(std::move(ErrorMsg)) {
    m_ErrorMsg = "[MyException]" + m_ErrorMsg + " at: " + Function;
}

MyException::MyException(std::string ErrorMsg, DWORD ErrorCode, const char *Function)
        : m_ErrorMsg(std::move(ErrorMsg)) {
    m_ErrorMsg = "[MyException]" + m_ErrorMsg + " ec: " + std::to_string(ErrorCode) + " at: " + Function;
}

const char *MyException::what() const {
    return m_ErrorMsg.c_str();
}

