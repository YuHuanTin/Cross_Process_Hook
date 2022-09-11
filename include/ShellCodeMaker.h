#ifndef CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#define CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#include "Data_Struct.h"

class c_ShellCodeMaker{
    void MakeShellCode_CreateRemoteThread(LPVOID &allocMemAddr);
    void MakeShellCode_Socket(LPVOID &allocMemAddr);
public:
    //just calc offset of params_CreateRemoteThread
    st_wCode wCode;
    explicit c_ShellCodeMaker(LPVOID allocMemAddr){
        MakeShellCode_CreateRemoteThread(allocMemAddr);
    };
};


#endif //CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
