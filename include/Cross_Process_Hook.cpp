#include <algorithm>
#include "Cross_Process_Hook.h"
#include "TlHelp32.h"
//private
HANDLE c_ProcessHook::GetProcessHandle(const string &processName) {
    HANDLE m_hTh32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (m_hTh32 == nullptr){
        return nullptr;
    } else{
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (!Process32First(m_hTh32, &pe)){
            CloseHandle(m_hTh32);
            return nullptr;
        }
        do {
            string m_ProcessName_transform = pe.szExeFile;
            std::transform(m_ProcessName_transform.begin(), m_ProcessName_transform.end(), m_ProcessName_transform.data(), tolower);
            if (processName == m_ProcessName_transform){
                HANDLE m_hProcess = OpenProcess(PROCESS_ALL_ACCESS,false,pe.th32ProcessID);
                CloseHandle(m_hTh32);
                if (m_hProcess == nullptr){
                    return nullptr;
                } else{
                    return m_hProcess;
                }
            }
        } while (Process32Next(m_hTh32, &pe));
        CloseHandle(m_hTh32);
        return nullptr;
    }
}
LPVOID c_ProcessHook::AllocMem() {
    LPVOID m_AllocMemAddr = VirtualAllocEx(ProcInfo_Dst.ProcessHandle, nullptr, 4096, 0, 0);
    if (m_AllocMemAddr == nullptr){
        return nullptr;
    }
    return m_AllocMemAddr;
}
bool c_ProcessHook::FreeMem(LPVOID hookedAddress) {
    for (auto &ch:ProcInfo_Dst.AllocMem) {
        if (ch.second.hookedAddress == hookedAddress){
            if (VirtualFreeEx(ProcInfo_Dst.ProcessHandle, ch.first, 4096, 0)){
                ProcInfo_Dst.AllocMem.erase(ch.first);
                return true;
            } else{
                return false;
            }
        }
    }
    return false;
}
//public
bool c_ProcessHook::CtorHook(LPVOID hookedAddress,unsigned hookedLen,st_wParams &params){
    //auto alloc mem in dst proc
    LPVOID m_AllocMemAddr = this->AllocMem();
    if (m_AllocMemAddr == nullptr){
        return false;
    }
    ProcInfo_Dst.AllocMem.insert({m_AllocMemAddr,{hookedAddress,0,true,false}});
    //ctor shellCode
    c_ShellCodeMaker shellCodeMaker(hookedAddress);
    shellCodeMaker.shellCode.params = params;


    return false;
}
bool c_ProcessHook::CommitMem() {

    return false;
}
bool c_ProcessHook::DeleteHook() {

    return false;
}
