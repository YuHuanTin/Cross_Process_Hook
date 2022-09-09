#ifndef CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#define CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#include <string>
#include "ShellCodeMaker.h"

using std::string;

class c_ProcessHook{
private:
    st_ProcInfo_Dst ProcInfo_Dst;

    HANDLE GetProcessHandle(const string &processName);
    LPVOID AllocMem();
    bool FreeMem(LPVOID hookedAddress);
public:
    //need low-case processName
    explicit c_ProcessHook(const string &processName){
        if (processName.length() > 0) {
            ProcInfo_Dst.ProcessHandle = this->GetProcessHandle(processName);
            if (ProcInfo_Dst.ProcessHandle == nullptr) {
                printf("error:GetProcessHandle\n");
            }
        }
    }
    //ctor shellCode
    bool CtorHook(LPVOID hookedAddress,unsigned hookedLen,st_wParams &params);
    //commit shellCode
    bool CommitMem();
    //delete shellCode
    bool DeleteHook();
};


#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
