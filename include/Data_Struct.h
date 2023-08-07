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
    SOCKET  hSocket = 0;                     // 保存目标进程socket句柄到控制块，一个远端程序只需要一个socket

#if _WIN64
    // todo : x64 QWORD register
    DWORD registerSize = sizeof(Register64);
    struct Register64 {

    };
#elif _WIN32
    // 结构体对于开头的偏移（十进制）
    DWORD offsetSocketFunction       = 28;
    DWORD offsetPipeFunction         = 0;
    DWORD offsetSharedMemoryFunction = 0;

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

struct DataBuffer {
    DWORD edi;
    DWORD esi;
    DWORD ebp;
    DWORD esp;
    DWORD ebx;
    DWORD edx;
    DWORD ecx;
    DWORD eax;
};


#endif //CROSS_PROCESS_HOOK_DATA_STRUCT_H
