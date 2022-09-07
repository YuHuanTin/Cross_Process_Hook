#ifndef CROSS_PROCESS_HOOK_DATA_STRUCT_H
#define CROSS_PROCESS_HOOK_DATA_STRUCT_H

#include "Windows.h"

struct st_RVA{
    DWORD OpenProcess;
    DWORD CreateRemoteThread;
    DWORD WaitForSingleObject;
    st_RVA() : OpenProcess(0),CreateRemoteThread(0),WaitForSingleObject(0){};
};
struct st_Params{
    DWORD OpenProcess_dwDesiredAccess;
    DWORD OpenProcess_bInheritHandle;
    DWORD OpenProcess_dwProcessId;
    DWORD CreateRemoteThread_lpThreadAttributes;
    DWORD CreateRemoteThread_dwStackSize;
    DWORD CreateRemoteThread_lpStartAddress;
    DWORD CreateRemoteThread_lpParameter;
    DWORD CreateRemoteThread_dwCreationFlags;
    DWORD CreateRemoteThread_lpThreadId;
    wchar_t DllName[26];
    st_RVA rva;
    explicit st_Params(st_RVA rva_input) :
    OpenProcess_dwDesiredAccess(0),
    OpenProcess_bInheritHandle(0),
    OpenProcess_dwProcessId(0),
    CreateRemoteThread_lpThreadAttributes(0),
    CreateRemoteThread_dwStackSize(0),
    CreateRemoteThread_lpStartAddress(0),
    CreateRemoteThread_lpParameter(0),
    CreateRemoteThread_dwCreationFlags(0),
    CreateRemoteThread_lpThreadId(0),
    DllName{ L"KERNEL32.DLL" }{
        rva.OpenProcess = rva_input.OpenProcess;
        rva.CreateRemoteThread = rva_input.CreateRemoteThread;
        rva.WaitForSingleObject = rva_input.WaitForSingleObject;
    }
};

#endif //CROSS_PROCESS_HOOK_DATA_STRUCT_H
