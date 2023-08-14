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
        std::size_t malloc      = 0;
        std::size_t free        = 0;
        /// ws2_32.dll
        std::size_t WSAStartup  = 0;
        std::size_t WSACleanup  = 0;
        std::size_t socket      = 0;
        std::size_t connect     = 0;
        std::size_t send        = 0;
        std::size_t recv        = 0;
        std::size_t closesocket = 0;
        std::size_t htons       = 0;
        std::size_t inet_addr   = 0;
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
