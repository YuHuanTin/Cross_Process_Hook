#include <algorithm>
#include "Cross_Process_Hook.h"
#include "TlHelp32.h"


using std::string;
//private
HANDLE c_ProcessHook::GetProcessHandle(const string &processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == nullptr) {
        return nullptr;
    }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return nullptr;
    }
    do {
        string exeFileTransform = pe.szExeFile;
        string processNameTransform = processName;

        std::transform(exeFileTransform.begin(), exeFileTransform.end(), exeFileTransform.data(), tolower);
        std::transform(processNameTransform.begin(), processNameTransform.end(), processNameTransform.data(), tolower);

        if (processNameTransform == exeFileTransform) {
            CloseHandle(hSnapshot);
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pe.th32ProcessID);
            if (hProcess != nullptr) {
                return hProcess;
            }
        }
    } while (Process32Next(hSnapshot, &pe));
    CloseHandle(hSnapshot);
    return nullptr;
}
LPVOID c_ProcessHook::AllocMem() {
    LPVOID m_AllocMemAddr = VirtualAllocEx(this->ProcInfo_Dst.processHandle, nullptr, 4096, MEM_COMMIT,
                                           PAGE_EXECUTE_READWRITE);
    if (m_AllocMemAddr == nullptr){
        return nullptr;
    }
    return m_AllocMemAddr;
}
bool c_ProcessHook::FreeMem(LPVOID allocMem) {
    if (VirtualFreeEx(this->ProcInfo_Dst.processHandle, allocMem, 0, MEM_RELEASE)) {
        return true;
    }
    return false;
}
void c_ProcessHook::PrintException(const std::string &out) {
    std::cout << out << '\n';
}
//public
bool c_ProcessHook::CtorHook(LPVOID hookedAddress,unsigned hookedLen){
    //auto alloc mem in dst proc
    LPVOID m_AllocMemAddr = this->AllocMem();
    if (m_AllocMemAddr == nullptr){
        return false;
    }
    //ctor write Code
    c_ShellCodeMaker shellCodeMaker(m_AllocMemAddr);



    this->ProcInfo_Dst.AllocMem.insert({m_AllocMemAddr,{hookedAddress,hookedLen,shellCodeMaker.wCode,true,false}});
    return false;
}
bool c_ProcessHook::CommitMem() {
    for (auto i = this->ProcInfo_Dst.AllocMem.begin(); i != this->ProcInfo_Dst.AllocMem.end(); ++i) {
        if ((*i).second.init && !(*i).second.used){
            //read backup code

            //write code
            SIZE_T m_NumOfWrite = 0;
            printf("%#x\n",(unsigned )(*i).first);
            WriteProcessMemory(this->ProcInfo_Dst.processHandle, (*i).first, (LPCVOID)&(*i).second.code, 4096, &m_NumOfWrite);
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
