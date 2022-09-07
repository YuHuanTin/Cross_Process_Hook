#ifndef CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#define CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#include "Data_Struct.h"

class c_ShellCodeMaker{
private:
    void MakeCode(LPVOID allocMemAddr, st_Params &params);
public:
    st_ShellCode shellCode;
    explicit c_ShellCodeMaker(LPVOID allocMemAddr,st_Params &params){
        MakeCode(allocMemAddr,params);
    }
};


#endif //CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
