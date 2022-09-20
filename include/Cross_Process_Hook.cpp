#include <algorithm>
#include "Cross_Process_Hook.h"
#include "ShellCodeMaker.h"
#include "TlHelp32.h"

using std::string;

//private
HANDLE c_ProcessHook::GetProcessHandle(const string &processName) {
    HANDLE m_hTh32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (m_hTh32 != nullptr){
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(m_hTh32, &pe)){
            do {
                string m_ProcessName_transform = pe.szExeFile;
                std::transform(m_ProcessName_transform.begin(), m_ProcessName_transform.end(), m_ProcessName_transform.data(), tolower);
                if (processName == m_ProcessName_transform){
                    CloseHandle(m_hTh32);
                    HANDLE m_hProcess = OpenProcess(PROCESS_ALL_ACCESS,false,pe.th32ProcessID);
                    if (m_hProcess != nullptr){
                        return m_hProcess;
                    }
                    break;
                }
            } while (Process32Next(m_hTh32, &pe));
        }
        CloseHandle(m_hTh32);
        return nullptr;
    }
    return nullptr;
}
LPVOID c_ProcessHook::AllocMem() {
    LPVOID m_AllocMemAddr = VirtualAllocEx(this->ProcInfo_Dst.ProcessHandle,nullptr,4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if (m_AllocMemAddr == nullptr){
        return nullptr;
    }
    return m_AllocMemAddr;
}
bool c_ProcessHook::FreeMem(LPVOID allocMem) {
    if (VirtualFreeEx(this->ProcInfo_Dst.ProcessHandle,allocMem,0,MEM_RELEASE)){
        return true;
    }
    return false;
}
//public
bool c_ProcessHook::CtorHook(LPVOID hookedAddress,e_SendDataMethod sendDataMethod,unsigned hookedLen){
    switch (sendDataMethod) {
        case e_SendDataMethod::NONE:
            printf("error:e_SendDataMethod have not init\n");
            return false;
        case e_SendDataMethod::CreateRemoteThread:
            break;
        case e_SendDataMethod::Socket:
#pragma comment(lib,"ws2_32.lib")
            WSAData wsaData{};
            if (WSAStartup(MAKEWORD(2,2),&wsaData) != 0){
                printf("error:WSAStartup fail\n");
                return false;
            }
            break;
    }
    //Check if the hookedAddress is used
    for (auto ch:ProcInfo_Dst.AllocMem) {
        if (ch.second.hookedAddress == hookedAddress){
            printf("error:hookedAddress already in use\n");
            return false;
        }
    }
    //Alloc mem in dst proc
    LPVOID m_AllocMemAddr = this->AllocMem();
    if (m_AllocMemAddr == nullptr){
        printf("error:AllocMem fail\n");
        return false;
    }
    //Switch method to make shellCode
    c_ShellCodeMaker shellCodeMaker(m_AllocMemAddr,sendDataMethod);



    this->ProcInfo_Dst.AllocMem.insert({m_AllocMemAddr,{hookedAddress,hookedLen,shellCodeMaker.wCode,true,false,sendDataMethod}});
    return false;
}
bool c_ProcessHook::CommitMem() {
    for (auto i = this->ProcInfo_Dst.AllocMem.begin(); i != this->ProcInfo_Dst.AllocMem.end(); ++i) {
        if ((*i).second.init && !(*i).second.used){
            //read backup code

            //write code
            SIZE_T m_NumOfWrite = 0;
            printf("%#x\n",(unsigned )(*i).first);
            WriteProcessMemory(this->ProcInfo_Dst.ProcessHandle,(*i).first,(LPCVOID)&(*i).second.code,4096,&m_NumOfWrite);
            if (m_NumOfWrite!= 4096){
                printf("error:%#X ,%#X\n",(unsigned)(*i).first,(unsigned)(*i).second.hookedAddress);
            }

            (*i).second.used = true;
        }

    }
    return false;
}
bool c_ProcessHook::DeleteHook(LPVOID hookedAddress) {




    for (auto i = this->ProcInfo_Dst.AllocMem.begin(); i != this->ProcInfo_Dst.AllocMem.end(); ++i) {
        if ((*i).second.hookedAddress == hookedAddress){
            FreeMem((*i).first);
            this->ProcInfo_Dst.AllocMem.erase(i);
            break;
        }
    }
    return false;
}
