#ifndef CROSS_PROCESS_HOOK_DATA_STRUCT_H
#define CROSS_PROCESS_HOOK_DATA_STRUCT_H

#include <cassert>
#include <vector>
#include <string>
#include "Windows.h"

//extern void getModuleRVA(std::map<const std::string, DWORD *> &keyValue, const std::string &ModuleName);

//enum class HookMethodE {
//    NONE, CreateRemoteThread, Socket
//};
//
//struct rRegister32T {
//    DWORD eax;
//    DWORD ebx;
//    DWORD ecx;
//    DWORD edx;
//    DWORD ebp;
//    DWORD esp;
//    DWORD esi;
//    DWORD edi;
//};
//struct wCodeT {
//    // [code][codeSavedData][reserve]
//    //  3072  512            512     = 4096
//
//
//    BYTE code[3072];
//    BYTE codeSavedData[512];
//    BYTE reserve[512];
//
//};
//struct HookT {
//    DWORD    hookAddress;
//    unsigned hookLen;
//
//    LPVOID allocAddress;
//};
//struct ProcessInformationT {
//    HANDLE      processHandle = nullptr;
//    HookMethodE hookMethod    = HookMethodE::NONE;
//    fn_cb       callback      = nullptr;
//};


#endif //CROSS_PROCESS_HOOK_DATA_STRUCT_H
