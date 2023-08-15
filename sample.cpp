
#include <fmt/format.h>
#include <chrono>
#include <iostream>
#include "ProcessHooker/Hook_Socket_TCP.h"

// for read process funcaddr to find hook addr
// Reader_FromMemory
#include "ProcessHooker/ControlBlockManager/ModuleFuncAddrReader/Reader_FromMemory.h"

bool funcRecv(HANDLE ProcessHandle, DataBuffer *DataBufferPtr) {
    try {
        fmt::println("from: 0x{:08X} "
                     "eax: 0x{:08X} "
                     "ebx: 0x{:08X} "
                     "ecx: 0x{:08X} "
                     "edx: 0x{:08X} "
                     "edi: 0x{:08X} "
                     "esi: 0x{:08X} "
                     "esp: 0x{:08X} "
                     "ebp: 0x{:08X}",
                     DataBufferPtr->whereFrom,
                     DataBufferPtr->eax,
                     DataBufferPtr->ebx,
                     DataBufferPtr->ecx,
                     DataBufferPtr->edx,
                     DataBufferPtr->edi,
                     DataBufferPtr->esi,
                     DataBufferPtr->esp,
                     DataBufferPtr->ebp);
        

        
    } catch (std::exception &exception) {
        fmt::println("occour a exception but catch: {}", exception.what());
        return true;
    }
    return true;
}

int main() {
    setbuf(stdout, nullptr);
    try {
        
        // —≠ª∑≤‚ ‘÷° ˝”∞œÏ
        while (1) {
            Hook_Socket_TCP hookSocketUdp("notepad.exe");
            fmt::println("start hook!");
            hookSocketUdp.addHook(0x6B5B5120, 5);
            hookSocketUdp.commitHook(funcRecv);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            fmt::println("delete hook!");
            hookSocketUdp.deleteHook(0x6B5B5120);
            
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

    } catch (std::exception &exception) {
        fmt::println("{}", exception.what());
    }


    return 0;
}