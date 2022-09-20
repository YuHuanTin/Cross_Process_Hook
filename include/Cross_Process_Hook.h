#ifndef CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#define CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#include "Data_Struct.h"

class c_ProcessHook{
private:
    st_ProcInfo_Dst ProcInfo_Dst;

    HANDLE GetProcessHandle(const std::string &processName);
    LPVOID AllocMem();
    bool FreeMem(LPVOID allocMem);
public:
    //processName: need low-case process name
    c_ProcessHook(const std::string &processName,fn_cb callback){
        if (processName.length() > 0) {
            ProcInfo_Dst.ProcessHandle = this->GetProcessHandle(processName);
            if (ProcInfo_Dst.ProcessHandle == nullptr)
                printf("error:GetProcessHandle\n");
        }
        if (callback == nullptr)
            printf("error:callback have not init\n");
    }
    //ctor wData
    bool CtorHook(LPVOID hookedAddress,e_SendDataMethod sendDataMethod,unsigned hookedLen);
    //commit wData
    bool CommitMem();
    //delete wData
    bool DeleteHook(LPVOID hookedAddress);
};


#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H