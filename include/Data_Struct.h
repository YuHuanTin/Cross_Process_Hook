#ifndef CROSS_PROCESS_HOOK_DATA_STRUCT_H
#define CROSS_PROCESS_HOOK_DATA_STRUCT_H
#include <map>
#include <string>
#include "Windows.h"
#include "PreProcess.h"
//enum class
enum class e_SendDataMethod{
    NONE,CreateRemoteThread,Socket
};
//struct
struct st_wRVA_Socket{
    //Socket
    DWORD WSAStartup;
    DWORD socket;
    DWORD connect;
    DWORD send;
    DWORD recv;
    DWORD closesocket;
    DWORD WSACleanup;
    st_wRVA_Socket() : WSAStartup(), socket(), connect(), send(), recv(), closesocket(), WSACleanup(){
        std::map<const std::string,DWORD *> keyValue{
                {"WSAStartup",&WSAStartup},
                {"socket",&socket},
                {"connect",&connect},
                {"send",&send},
                {"recv",&recv},
                {"closesocket",&closesocket},
                {"WSACleanup",&WSACleanup}
        };
        fn_GetRVAs(keyValue,"ws2_32.dll");
    }
};
struct st_wRVA_CreateRemoteThread{
    DWORD OpenProcess;
    DWORD CreateRemoteThread;
    DWORD WaitForSingleObject;
    st_wRVA_CreateRemoteThread() : OpenProcess(), CreateRemoteThread(), WaitForSingleObject(){
        std::map<const std::string,DWORD *> keyValue{
                {"OpenProcess",&OpenProcess},
                {"CreateRemoteThread",&CreateRemoteThread},
                {"WaitForSingleObject",&WaitForSingleObject}
        };
        fn_GetRVAs(keyValue,"kernel32.dll");
    }
};
struct st_wParams_Socket{
    wchar_t DllName[11];
    st_wRVA_Socket rvaSocket;
    st_wParams_Socket() :
            DllName{L"WS2_32.dll"},
            rvaSocket(){}
};
struct st_wParams_CreateRemoteThread{
    DWORD OpenProcess_dwDesiredAccess;
    DWORD OpenProcess_bInheritHandle;
    DWORD OpenProcess_dwProcessId;
    DWORD CreateRemoteThread_lpThreadAttributes;
    DWORD CreateRemoteThread_dwStackSize;
    DWORD CreateRemoteThread_lpStartAddress;//need to specify
    DWORD CreateRemoteThread_lpParameter;//need to specify
    DWORD CreateRemoteThread_dwCreationFlags;
    DWORD CreateRemoteThread_lpThreadId;
    wchar_t DllName[13];
    st_wRVA_CreateRemoteThread rvaCreateRemoteThread;
    st_wParams_CreateRemoteThread() :
            OpenProcess_dwDesiredAccess(PROCESS_ALL_ACCESS),
            OpenProcess_bInheritHandle(0),
            OpenProcess_dwProcessId(GetCurrentProcessId()),
            CreateRemoteThread_lpThreadAttributes(0),
            CreateRemoteThread_dwStackSize(0),
            CreateRemoteThread_lpStartAddress(0),
            CreateRemoteThread_lpParameter(0),
            CreateRemoteThread_dwCreationFlags(0),
            CreateRemoteThread_lpThreadId(0),
            DllName{ L"KERNEL32.DLL" },
            rvaCreateRemoteThread(){}
};
struct st_rData{
    DWORD eax;
    DWORD ebx;
    DWORD ecx;
    DWORD edx;
    DWORD ebp;
    DWORD esp;
    DWORD esi;
    DWORD edi;
    DWORD hookedFrom;//address of the trigger shellcode
};
struct st_wCode{
    //  [wCode][params_CreateRemoteThread][params_Socket][st_rData][reserve][backupCode]
    //  ^      ^                          ^              ^         ^        ^
    //  1024   76/512                     00/512         36        auto     512        = 4096
    unsigned char shellCode[1024];
    st_wParams_CreateRemoteThread params_CreateRemoteThread;
    unsigned char reserve1[512 - sizeof(st_wParams_CreateRemoteThread)];
    st_wParams_Socket params_Socket;
    unsigned char reserve2[512 - sizeof(st_wParams_Socket)];
    st_rData registerSaved;
    unsigned char reserve3[1536 - sizeof(st_rData)];
    unsigned char backupCode[512];
    st_wCode() :
            shellCode{0},
            params_CreateRemoteThread(),
            reserve1{},
            params_Socket(),
            reserve2{},
            registerSaved(),
            reserve3{},
            backupCode{0}{}
};
struct st_keyValue_AllocMem{
    LPVOID hookedAddress;//addr of code need to hook
    unsigned HookedCodeLen;//len of code need to hook
    st_wCode code;

    bool init;
    bool used;
};
struct st_ProcInfo_Dst{
    HANDLE ProcessHandle;//dst process handle
    std::map<LPVOID,st_keyValue_AllocMem> AllocMem;//AllocMemAddr-st_keyValue_AllocMem
    st_ProcInfo_Dst() : ProcessHandle(nullptr), AllocMem(){}
};

//declare the function form of callback
typedef void(*fn_cb) (void *argAddr);

#endif //CROSS_PROCESS_HOOK_DATA_STRUCT_H
