#ifndef CROSS_PROCESS_HOOK_DATA_STRUCT_H
#define CROSS_PROCESS_HOOK_DATA_STRUCT_H
#include <map>
#include <string>
#include "Windows.h"

struct st_wRVA{
    DWORD OpenProcess;
    DWORD CreateRemoteThread;
    DWORD WaitForSingleObject;
    st_wRVA() : OpenProcess(),CreateRemoteThread(),WaitForSingleObject(){
        std::map<std::string,DWORD *> key_value{
                {"OpenProcess",&OpenProcess},
                {"CreateRemoteThread",&CreateRemoteThread},
                {"WaitForSingleObject",&WaitForSingleObject}
        };
        auto m_BaseAddr = (DWORD)LoadLibraryA("kernel32.dll");
        //get dos size
        long m_DosSize = *(long *)(m_BaseAddr + 0x3C);
        //get optional PE address
        DWORD m_OptionalPEAddr = m_BaseAddr + m_DosSize + 0x4 + 0x14;
        //get export table info
        DWORD m_ExportRVA = *(DWORD *)(m_OptionalPEAddr + 0x60);
        DWORD m_ExportSize = *(DWORD *)(m_OptionalPEAddr + 0x64);
        if (m_ExportSize <= 0){
            return;
        }
        DWORD ExportAddr = m_BaseAddr + m_ExportRVA;
        //get num of named functions
        DWORD ExportNumberOfNames = *(DWORD *)(ExportAddr + 0x18);
        //get function,ordinal,name table address
        DWORD ExportFunctionsAddr = m_BaseAddr + *(DWORD *)(ExportAddr + 0x1C);
        DWORD ExportNamesAddr = m_BaseAddr + *(DWORD *)(ExportAddr + 0x20);
        DWORD ExportNameOrdinalsAddr = m_BaseAddr + *(DWORD *)(ExportAddr + 0x24);
        int cnt = 0;
        for (int i = 0 ;i < ExportNumberOfNames && cnt < key_value.size(); ++i){
            char *strAddr = (char *)(m_BaseAddr + *(DWORD *)(ExportNamesAddr + i * 4));
            if (key_value.find(strAddr) != key_value.end()){
                WORD Ordinal = *(WORD *)(ExportNameOrdinalsAddr + i * 2);
                *key_value.at(strAddr) = *(DWORD *)(ExportFunctionsAddr + Ordinal * 4);
                ++cnt;
            }
        }
    }
};
struct st_wParams{
    DWORD OpenProcess_dwDesiredAccess;
    DWORD OpenProcess_bInheritHandle;
    DWORD OpenProcess_dwProcessId;
    DWORD CreateRemoteThread_lpThreadAttributes;
    DWORD CreateRemoteThread_dwStackSize;
    DWORD CreateRemoteThread_lpStartAddress;//need to specify
    DWORD CreateRemoteThread_lpParameter;//need to specify
    DWORD CreateRemoteThread_dwCreationFlags;
    DWORD CreateRemoteThread_lpThreadId;
    wchar_t DllName[13];//auto fill to 4Bytes
    st_wRVA rva;
    st_wParams() :
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
    rva(){}
};
struct st_rRegister{
    DWORD eax;
    DWORD ebx;
    DWORD ecx;
    DWORD edx;
    DWORD ebp;
    DWORD esp;
    DWORD esi;
    DWORD edi;
};
struct st_rData : st_rRegister{
    DWORD hookFrom;//jmp to the wCode,same as hookedAddress
};
struct st_wCode{
    //  [wCode][params][st_rData][reserve][backupCode]
    //  ^           ^       ^         ^         ^
    //  1024        76/128  32        auto      512        = 4096
    unsigned char shellCode[1024];
    st_wParams params;
    st_rRegister registerSaved;
    unsigned char reserve[4096 - sizeof(shellCode) - sizeof(params) - sizeof(registerSaved) - 512];
    unsigned char backupCode[512];
    st_wCode() : shellCode{0}, params(), registerSaved(), reserve{0}, backupCode{0}{}
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

//RemoteThread will call it
typedef void(*fn_cb) (st_rData *argAddr);

#endif //CROSS_PROCESS_HOOK_DATA_STRUCT_H
