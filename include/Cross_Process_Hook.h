#ifndef CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#define CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#include "PreProcess.h"
#include "ShellCodeMaker.h"

using std::string;

class c_ProcessHook{
private:
    HANDLE GetProcessHandle(const string &processName);
public:
    st_ProcessInfo_Dst processInfoDst;
    //need low-case processName
    explicit c_ProcessHook(const string &processName){
        if (processName.length() > 0) {
            processInfoDst.ProcessHandle = GetProcessHandle(processName);
            if (processInfoDst.ProcessHandle == nullptr) {
                printf("error:GetProcessHandle\n");
            }
        }
    }
    LPVOID AllocMem();
    bool FreeMem(LPVOID hookedAddress);
    bool CtorHook(LPVOID hookedAddress,st_Params &params);
    bool CommitMem();
    bool DeleteHook();
};


#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
