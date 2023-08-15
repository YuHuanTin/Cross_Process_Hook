

#include "ShellCode_Socket_UDP_X86.h"
#include "../../DataStruct/DataStruct.h"

/// shell code base begin

#pragma runtime_checks( "", off )
#pragma optimize( "", off )

// 请在调用函数前就在函数头部填充 pushad
__declspec(safebuffers)
void udpX86() {
    /// ucrtbase.dll
    typedef void *__cdecl malloc(
            _In_ _CRT_GUARDOVERFLOW size_t Size);
    typedef void __cdecl free(
            _Pre_maybenull_ _Post_invalid_ void *Block);

    /// ws2_32.dll
    typedef int           PASCAL FAR WSAStartup(
            _In_ WORD wVersionRequired,
            _Out_ LPWSADATA lpWSAData);
    typedef SOCKET        PASCAL FAR socket(
            _In_ int af,
            _In_ int type,
            _In_ int protocol);
    typedef int           PASCAL FAR recvfrom(
            _In_ SOCKET s,
            _Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR *buf,
            _In_ int len,
            _In_ int flags,
            _Out_writes_bytes_to_opt_(*fromlen, *fromlen) struct sockaddr FAR *from,
            _Inout_opt_ int FAR *fromlen);
    typedef int           PASCAL FAR sendto(
            _In_ SOCKET s,
            _In_reads_bytes_(len) const char FAR *buf,
            _In_ int len,
            _In_ int flags,
            _In_reads_bytes_opt_(tolen) const struct sockaddr FAR *to,
            _In_ int tolen);

    typedef int           PASCAL FAR closesocket(IN SOCKET s);
    typedef int           PASCAL FAR WSACleanup(void);
    typedef u_short       PASCAL FAR htons(_In_ u_short hostshort);
    typedef unsigned long PASCAL FAR inet_addr(_In_z_ const char FAR *cp);

    // vvv 填入控制块的地址
    auto controlBlockAddr    = (std::uint8_t *) 0x99999999;
    // vvv 填入储存寄存器的地址
    auto registerStorageAddr = (std::uint8_t *) 0x99999999;

    /// vvv 获取控制块参数
    bool   *isSocketEstablish = (bool *) (controlBlockAddr + offsetof(ControlBlock, isSocketEstablish));
    SOCKET *hSocket           = (SOCKET *) (controlBlockAddr + offsetof(ControlBlock, hSocket));
    /// ^^^ 获取控制块参数
    /// vvv 获取所需函数地址
    auto   socketFunctionAddr = (std::size_t) controlBlockAddr + offsetof(ControlBlock, PSocketFunctionAddress);
    auto   lpMalloc           = (malloc *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, malloc));
    auto   lpFree             = (free *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, free));
    auto   lpWSAStartup       = (WSAStartup *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, WSAStartup));
    auto   lpWSACleanup       = (WSACleanup *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, WSACleanup));
    auto   lpSocket           = (socket *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, socket));
    auto   lpSendto           = (sendto *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, sendto));
    auto   lpRecvfrom         = (recvfrom *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, recvfrom));
    auto   lpClosesocket      = (closesocket *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, closesocket));
    auto   lpHtons            = (htons *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, htons));
    /// ^^^ 获取所需函数地址

    /// 建立socket
    if (!*isSocketEstablish) {
        auto *allocWsaData = (WSAData *) lpMalloc(sizeof(WSAData));
        lpWSAStartup(MAKEWORD(2, 2), allocWsaData);
        SOCKET socketHandle = lpSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (socketHandle == INVALID_SOCKET) {
            lpWSACleanup();
            lpFree(allocWsaData);
            return;
        }
        lpFree(allocWsaData);


        *hSocket           = socketHandle;
        *isSocketEstablish = true;
    }
    /// 收发消息
    auto *allocSockAddrIn = (sockaddr_in *) lpMalloc(sizeof(sockaddr_in));
    allocSockAddrIn->sin_family           = AF_INET;
    allocSockAddrIn->sin_port             = lpHtons(9999);
    allocSockAddrIn->sin_addr.S_un.S_addr = 0x0100007F; // 127.0.0.1也可以写成 0x0100007F
    if (lpSendto(*hSocket, (char *) registerStorageAddr, sizeof(DataBuffer), 0, (sockaddr *) allocSockAddrIn, sizeof(sockaddr_in)) <= 0) {
        lpClosesocket(*hSocket);
        lpWSACleanup();
        *isSocketEstablish = false;
        lpFree(allocSockAddrIn);
        return;
    }


    // 不能使用 {0}，因为不能使用memset
    char      recvBuf[8];
    for (char &buf: recvBuf)
        buf = '\0';

    int len = sizeof(sockaddr_in);
    if (lpRecvfrom(*hSocket, recvBuf, sizeof(recvBuf), 0, (sockaddr *) allocSockAddrIn, &len) < 0) {
        lpClosesocket(*hSocket);
        lpWSACleanup();
        *isSocketEstablish = false;
        lpFree(allocSockAddrIn);
        return;
    }

    // 销毁端点
    lpFree(allocSockAddrIn);

    __asm {
            nop
            nop
    }
}

#pragma runtime_checks( "", restore )
#pragma optimize( "", on )

/// shell code base end

std::vector<std::uint8_t> ShellCode_Socket_UDP_X86::makeShellCode(std::size_t ControlBlockAddr, std::size_t RegisterStorageAddr) {
    std::uint8_t endSign[]          = {0x90, 0x90};
    auto         funBeginAddAndSize = getFuncAddrAndSize((std::size_t) udpX86, endSign, 2);

    std::vector<std::uint8_t> codeBytes(funBeginAddAndSize.second);
    memcpy(codeBytes.data(), (void *) funBeginAddAndSize.first, funBeginAddAndSize.second);

    // 找到第一个 0xCCCCCCCC 替换为控制块的地址
    // 找到第二个 0xCCCCCCCC 替换为保存寄存器的地址
    std::size_t patchPos = 0;
    patchPos = patchAddress(codeBytes.data(), patchPos, codeBytes.size() - sizeof(std::size_t) + 1, 0x99999999, ControlBlockAddr);
    patchAddress(codeBytes.data(), patchPos + sizeof(std::size_t), codeBytes.size() - sizeof(std::size_t) + 1, 0x99999999, RegisterStorageAddr);

    return insertPushadAndPopad(insertRegisterSaveCode(codeBytes));
}
