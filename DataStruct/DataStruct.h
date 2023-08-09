#ifndef CROSS_PROCESS_HOOK_DATASTRUCT_H
#define CROSS_PROCESS_HOOK_DATASTRUCT_H

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

    struct SocketFunctionAddress {
        /// ucrtbase.dll
        DWORD malloc      = 0;
        DWORD free        = 0;
        /// ws2_32.dll
        DWORD WSAStartup  = 0;
        DWORD WSACleanup  = 0;
        DWORD socket      = 0;
        DWORD connect     = 0;
        DWORD send        = 0;
        DWORD recv        = 0;
        DWORD closesocket = 0;
        DWORD htons       = 0;
        DWORD inet_addr   = 0;
    }     PSocketFunctionAddress;
#endif

};

// 按照 pushad 的顺序接收
struct DataBuffer {
    DWORD edi;
    DWORD esi;
    DWORD ebp;
    DWORD esp;
    DWORD ebx;
    DWORD edx;
    DWORD ecx;
    DWORD eax;
    DWORD whereFrom;
};


#endif //CROSS_PROCESS_HOOK_DATASTRUCT_H
