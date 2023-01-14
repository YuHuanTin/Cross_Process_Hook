#ifndef CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#define CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#include <string>
#include <iostream>
#include "ShellCodeMaker.h"

#pragma comment(lib,"Ws2_32.lib")

class c_ProcessHook{
private:
    st_ProcInfo_Dst ProcInfo_Dst;
    //st_wParams_CreateRemoteThread;

    HANDLE GetProcessHandle(const std::string &processName);
    LPVOID AllocMem();
    bool FreeMem(LPVOID allocMem);
    void PrintException(const std::string &out);
public:
    c_ProcessHook(const std::string &processName, e_SendDataMethod sendDataMethod, fn_cb callback) {
        //init iostream
        std::ios::sync_with_stdio(false);
        std::cin.tie(nullptr);

        //get dst process handle
        if (processName.length() <= 0) {
            PrintException("Unknow Process Name!");
            return;
        }
        ProcInfo_Dst.processHandle = this->GetProcessHandle(processName);
        if (ProcInfo_Dst.processHandle == nullptr)
            PrintException("Can't find process or OpenProcess failed!");

        //choose method
        switch (sendDataMethod) {
            case e_SendDataMethod::NONE:
                PrintException("e_SendDataMethod have not init!");
                break;
            case e_SendDataMethod::CreateRemoteThread:
                break;
            case e_SendDataMethod::Socket:
                WSAData wsaData{};
                if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
                    PrintException("WSAStartup failed!");
                break;
        }

        //check callback function
        if (callback == nullptr)
            PrintException("callback have not init!");
    }
    //ctor wCode
    bool CtorHook(LPVOID hookedAddress,unsigned hookedLen);
    //commit wCode
    bool CommitMem();
    //delete wCode
    bool DeleteHook(LPVOID hookedAddress);
};

#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H