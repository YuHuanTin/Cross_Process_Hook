#ifndef CROSS_PROCESS_HOOK_DATA_STRUCT_H
#define CROSS_PROCESS_HOOK_DATA_STRUCT_H

#include <cassert>
#include <vector>
#include <string>
#include "Windows.h"


struct DataBufferBlock {
    DWORD HookMethod;
    uint8_t isSocketEstablish;
    uint8_t isPipeEstablish;
    uint8_t isSharedMemoryEstablish;
    uint8_t reserve[5];

#if _WIN32
    DWORD registerSize = sizeof(Register32);
    struct Register32 {
        DWORD eax;
        DWORD ebx;
        DWORD ecx;
        DWORD edx;
        DWORD ebp;
        DWORD esp;
        DWORD esi;
        DWORD edi;
    } PRegister32;
#elif _WIN64
    // todo : x64 QWORD register
    DWORD registerSize = sizeof(Register64);
    struct Register64 {

    };
#endif


    struct SocketFunctionAddress {
        /// ucrtbase.dll
        // function[0] = 'malloc'
        // function[1] = 'free'

        /// ws2_32.dll
        // function[2] = 'WSAStartup'
        // function[3] = 'WSACleanup'
        // function[4] = 'socket'
        // function[5] = 'connect'
        // function[6] = 'send'
        // function[7] = 'recv'
        // function[8] = 'closesocket'
        // function[9] = 'htons'
        // function[10] = 'inet_addr'

        DWORD malloc;
        DWORD free;
        DWORD WSAStartup;
        DWORD WSACleanup;
        DWORD socket;
        DWORD connect;
        DWORD send;
        DWORD recv;
        DWORD closesocket;
        DWORD htons;
        DWORD inet_addr;
    } PSocketFunctionAddress;
};

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
