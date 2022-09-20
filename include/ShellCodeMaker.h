#ifndef CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#define CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#include "Data_Struct.h"

class c_ShellCodeMaker{
    void MakeShellCode_CreateRemoteThread(LPVOID &allocMemAddr);
    void MakeShellCode_Socket(LPVOID &allocMemAddr);
public:
    st_wData wCode;

    explicit c_ShellCodeMaker(LPVOID allocMemAddr,e_SendDataMethod sendDataMethod){
        switch (sendDataMethod) {
            case e_SendDataMethod::NONE:
                break;
            case e_SendDataMethod::CreateRemoteThread:
                MakeShellCode_CreateRemoteThread(allocMemAddr);
                break;
            case e_SendDataMethod::Socket:
                MakeShellCode_Socket(allocMemAddr);
                break;
        }
    };
};

#endif //CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
