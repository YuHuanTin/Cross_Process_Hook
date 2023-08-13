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
        LPVOID malloc      = nullptr;
        LPVOID free        = nullptr;
        /// ws2_32.dll
        LPVOID WSAStartup  = nullptr;
        LPVOID WSACleanup  = nullptr;
        LPVOID socket      = nullptr;
        LPVOID connect     = nullptr;
        LPVOID send        = nullptr;
        LPVOID recv        = nullptr;
        LPVOID closesocket = nullptr;
        LPVOID htons       = nullptr;
        LPVOID inet_addr   = nullptr;
    }       PSocketFunctionAddress;
#endif

};

#if _WIN64
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
#elif _WIN32
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
#endif


#endif //CROSS_PROCESS_HOOK_DATASTRUCT_H
