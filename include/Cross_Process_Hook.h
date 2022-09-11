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
    bool FreeMem(LPVOID allocMem);
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
    //ctor wCode
    bool CtorHook(LPVOID hookedAddress,unsigned hookedLen,st_wParams &params);
    //commit wCode
    bool CommitMem();
    //delete wCode
    bool DeleteHook(LPVOID hookedAddress);
};


#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
