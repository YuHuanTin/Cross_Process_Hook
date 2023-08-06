#ifndef CROSS_PROCESS_HOOK_DATA_STRUCT_H
#define CROSS_PROCESS_HOOK_DATA_STRUCT_H

#include <cassert>
#include <vector>
#include <string>
#include <Windows.h>

enum HookMethod {
    Socket, Pipe, Shared_Memory
};

struct ControlBlock {
    DWORD   HookMethod              = HookMethod::Socket;
    uint8_t isSocketEstablish       = 0;
    uint8_t isPipeEstablish         = 0;
    uint8_t isSharedMemoryEstablish = 0;
#if _WIN64
    uint8_t isX86 = 0;
#elif _WIN32
    uint8_t isX86 = 1;
#endif

    uint8_t reserve[4]{0};

#if _WIN64
    // todo : x64 QWORD register
    DWORD registerSize = sizeof(Register64);
    struct Register64 {

    };
#elif _WIN32
    // 结构体对于开头的偏移（十进制）
    DWORD offsetRegister             = 28;
    DWORD offsetSocketFunction       = 60;
    DWORD offsetPipeFunction         = 0;
    DWORD offsetSharedMemoryFunction = 0;

    struct Register32 {
        DWORD eax = 0;
        DWORD ebx = 0;
        DWORD ecx = 0;
        DWORD edx = 0;
        DWORD ebp = 0;
        DWORD esp = 0;
        DWORD esi = 0;
        DWORD edi = 0;
    }     PRegister32;
    struct SocketFunctionAddress {
        /// ucrtbase.dll
        DWORD malloc      = 0;               // function[0] = 'malloc'
        DWORD free        = 0;               // function[1] = 'free'
        /// ws2_32.dll
        DWORD WSAStartup  = 0;               // function[2] = 'WSAStartup'
        DWORD WSACleanup  = 0;               // function[3] = 'WSACleanup'
        DWORD socket      = 0;               // function[4] = 'socket'
        DWORD connect     = 0;               // function[5] = 'connect'
        DWORD send        = 0;               // function[6] = 'send'
        DWORD recv        = 0;               // function[7] = 'recv'
        DWORD closesocket = 0;               // function[8] = 'closesocket'
        DWORD htons       = 0;               // function[9] = 'htons'
        DWORD inet_addr   = 0;               // function[10] = 'inet_addr'
    }     PSocketFunctionAddress;
#endif

};


#endif //CROSS_PROCESS_HOOK_DATA_STRUCT_H
