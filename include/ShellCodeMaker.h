#ifndef CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#define CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#include "Data_Struct.h"

class c_ShellCodeMaker{
    void MakeShellCode(LPVOID &allocMemAddr);
public:
    //just calc offset of params
    st_wCode wCode;
    explicit c_ShellCodeMaker(LPVOID allocMemAddr){
        MakeShellCode(allocMemAddr);
    };
};


#endif //CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
