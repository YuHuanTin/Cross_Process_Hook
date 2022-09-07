#ifndef CROSS_PROCESS_HOOK_DATA_STRUCT_H
#define CROSS_PROCESS_HOOK_DATA_STRUCT_H
#include <map>
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
    DWORD CreateRemoteThread_lpStartAddress;//need to specify
    DWORD CreateRemoteThread_lpParameter;//need to specify
    DWORD CreateRemoteThread_dwCreationFlags;
    DWORD CreateRemoteThread_lpThreadId;
    wchar_t DllName[26];
    st_RVA rva;
    explicit st_Params() :
    OpenProcess_dwDesiredAccess(PROCESS_ALL_ACCESS),
    OpenProcess_bInheritHandle(0),
    OpenProcess_dwProcessId(GetCurrentProcessId()),
    CreateRemoteThread_lpThreadAttributes(0),
    CreateRemoteThread_dwStackSize(0),
    CreateRemoteThread_lpStartAddress(0),
    CreateRemoteThread_lpParameter(0),
    CreateRemoteThread_dwCreationFlags(0),
    CreateRemoteThread_lpThreadId(0),
    DllName{ L"KERNEL32.DLL" }{}
};
struct st_RegisterSaved{
    DWORD eax;
    DWORD ebx;
    DWORD ecx;
    DWORD edx;
    DWORD ebp;
    DWORD esp;
    DWORD esi;
    DWORD edi;
};
struct st_ProcessInfo_Dst{
    HANDLE ProcessHandle;//dst process handle
    std::map<LPVOID,LPVOID> mAllocMemAddr_HookAddr;//key_value of AllocMemAddress-hookedAddress
    st_ProcessInfo_Dst() : ProcessHandle(nullptr),mAllocMemAddr_HookAddr(){}
};
struct st_ShellCode{
    unsigned char shellcode[1024];
    st_Params params;
    st_RegisterSaved registerSaved;
    st_ShellCode() : shellcode{0},params(),registerSaved(){}
};

//RemoteThread will call it
typedef void(*fn_cb) (void *argAddr);

#endif //CROSS_PROCESS_HOOK_DATA_STRUCT_H
