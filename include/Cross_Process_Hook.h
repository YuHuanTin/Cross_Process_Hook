#ifndef CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#define CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#include <string>
#include "ShellCodeMaker.h"

using std::string;

class c_ProcessHook{
private:
    st_ProcInfo_Dst ProcInfo_Dst;
    st_wParams_CreateRemoteThread

    HANDLE GetProcessHandle(const string &processName);
    LPVOID AllocMem();
    bool FreeMem(LPVOID allocMem);
public:
    //processName: need low-case process name
    c_ProcessHook(const string &processName,e_SendDataMethod sendDataMethod,fn_cb callback){
        //get dst process handle
        if (processName.length() > 0) {
            ProcInfo_Dst.ProcessHandle = this->GetProcessHandle(processName);
            if (ProcInfo_Dst.ProcessHandle == nullptr)
                printf("error:GetProcessHandle\n");
        }
        if (sendDataMethod == e_SendDataMethod::NONE)
            printf("error:e_SendDataMethod have not init\n");
        else if (sendDataMethod == e_SendDataMethod::Socket){
            WSAData wsaData{};
            if (WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
                printf("error:WSAStartup\n");
        }
        if (callback == nullptr)
            printf("error:callback have not init\n");
    }
    //ctor wCode
    bool CtorHook(LPVOID hookedAddress,unsigned hookedLen);
    //commit wCode
    bool CommitMem();
    //delete wCode
    bool DeleteHook(LPVOID hookedAddress);
};


#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H