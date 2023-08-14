

#include "SocketShellCodeX86.h"
#include "../DataStruct/DataStruct.h"

/// shell code base begin

#pragma runtime_checks( "", off )
#pragma optimize( "", off )

// 请在调用函数前就在函数头部填充 pushad
__declspec(safebuffers)
void socketShellCodeX86() {
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
    typedef int           PASCAL FAR connect(
            _In_ SOCKET s,
            _In_reads_bytes_(namelen) const struct sockaddr FAR *name,
            _In_ int namelen);
    typedef int           PASCAL FAR send(
            _In_ SOCKET s,
            _In_reads_bytes_(len) const char FAR *buf,
            _In_ int len,
            _In_ int flags);
    typedef int           PASCAL FAR recv(
            _In_ SOCKET s,
            _Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR *buf,
            _In_ int len,
            _In_ int flags);
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
    auto   lpConnect          = (connect *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, connect));
    auto   lpSend             = (send *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, send));
    auto   lpRecv             = (recv *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, recv));
    auto   lpClosesocket      = (closesocket *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, closesocket));
    auto   lpHtons            = (htons *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, htons));
//    auto   lpInet_addr        = (inet_addr *) *(std::size_t *) (socketFunctionAddr + offsetof(ControlBlock::SocketFunctionAddress, inet_addr));
    /// ^^^ 获取所需函数地址

    /// 建立socket
    if (!*isSocketEstablish) {
        auto *allocWsaData = (WSAData *) lpMalloc(sizeof(WSAData));
        lpWSAStartup(MAKEWORD(2, 2), allocWsaData);
        SOCKET socketHandle = lpSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socketHandle == INVALID_SOCKET) {
            lpWSACleanup();
            lpFree(allocWsaData);
            return;
        }
        lpFree(allocWsaData);

        auto *allocSockAddrIn = (sockaddr_in *) lpMalloc(sizeof(sockaddr_in));
        allocSockAddrIn->sin_family           = AF_INET;
        allocSockAddrIn->sin_port             = lpHtons(9999);
        allocSockAddrIn->sin_addr.S_un.S_addr = 0x0100007F; // 127.0.0.1也可以写成 0x0100007F

        if (lpConnect(socketHandle, (sockaddr *) allocSockAddrIn, sizeof(sockaddr)) == SOCKET_ERROR) {
            lpClosesocket(socketHandle);
            lpWSACleanup();
            lpFree(allocSockAddrIn);
            return;
        }
        lpFree(allocSockAddrIn);

        *hSocket           = socketHandle;
        *isSocketEstablish = true;
    }
    /// 收发消息
    // 保存DataBuffer结构体数据
    if (lpSend(*hSocket, (char *) registerStorageAddr, sizeof(DataBuffer), 0) == SOCKET_ERROR) {
        lpClosesocket(*hSocket);
        lpWSACleanup();
        *isSocketEstablish = false;
        return;
    }

    // 不能使用 {0}，因为不能使用memset
    char      recvBuf[8];
    for (char &buf: recvBuf)
        buf = '\0';

    if (lpRecv(*hSocket, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
        lpClosesocket(*hSocket);
        lpWSACleanup();
        *isSocketEstablish = false;
        return;
    }
//    const char  strReOK[] = {'R', 'e', 'O', 'K', '\0'};
//    std::size_t sameLen   = 0;
//
//    for (std::size_t i = 0; i < sizeof(strReOK) / sizeof(strReOK[0]); ++i) {
//        if (recvBuf[i] == strReOK[i])
//            ++sameLen;
//        else
//            break;
//    }
//    if (sameLen == sizeof(strReOK) / sizeof(strReOK[0])) {
//
//    }


    __asm {
            nop
            nop
    }
}

#pragma runtime_checks( "", restore )
#pragma optimize( "", on )


/// shell code base end

std::vector<std::uint8_t> SocketShellCodeX86::makeShellCode(std::size_t ControlBlockAddr, std::size_t RegisterStorageAddr) {
    std::uint8_t endSign[]          = {0x90, 0x90};
    auto         funBeginAddAndSize = getFuncAddrAndSize((std::size_t) socketShellCodeX86, endSign, 2);

    std::vector<std::uint8_t> codeBytes(funBeginAddAndSize.second);
    memcpy(codeBytes.data(), (void *) funBeginAddAndSize.first, funBeginAddAndSize.second);

    // 找到第一个 0xCCCCCCCC 替换为控制块的地址
    // 找到第二个 0xCCCCCCCC 替换为保存寄存器的地址
    std::size_t patchPos = 0;
    patchPos = patchAddress(codeBytes.data(), patchPos, codeBytes.size() - sizeof(std::size_t) + 1, 0x99999999, ControlBlockAddr);
    patchAddress(codeBytes.data(), patchPos + sizeof(std::size_t), codeBytes.size() - sizeof(std::size_t) + 1, 0x99999999, RegisterStorageAddr);

    return insertPushadAndPopad(insertRegisterSaveCode(codeBytes));
}
