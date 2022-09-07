#include <algorithm>
#include "Cross_Process_Hook.h"
#include "TlHelp32.h"

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
    LPVOID m_AllocMemAddr = VirtualAllocEx(processInfoDst.ProcessHandle, nullptr,4096,0,0);
    if (m_AllocMemAddr == nullptr){
        return nullptr;
    }
    return m_AllocMemAddr;
}
bool c_ProcessHook::FreeMem(LPVOID hookedAddress) {
    for (auto &ch:processInfoDst.mAllocMemAddr_HookAddr) {
        if (ch.second == hookedAddress){
            if (VirtualFreeEx(processInfoDst.ProcessHandle,ch.first,4096,0)){
                processInfoDst.mAllocMemAddr_HookAddr.erase(ch.first);
                return true;
            } else{
                return false;
            }
        }
    }
    return false;
}

bool c_ProcessHook::CtorHook(LPVOID hookedAddress,st_Params &params){
    LPVOID m_AllocMem = this->AllocMem();
    if (m_AllocMem == nullptr){
        return false;
    }
    processInfoDst.mAllocMemAddr_HookAddr.insert({m_AllocMem,hookedAddress});
    c_ShellCodeMaker shellCodeMaker(m_AllocMem,params);
    return false;
}
bool c_ProcessHook::CommitMem() {
    //writeProcess

    return false;
}
bool c_ProcessHook::DeleteHook() {

    return false;
}
