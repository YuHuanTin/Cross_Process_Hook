

#include "ShellCodeMaker.h"


/// shell code base begin

#pragma runtime_checks( "", off )
#pragma optimize( "", off )

// 请在调用函数前就在函数头部填充 pushad
__declspec(safebuffers)
void socketShellCodeBase() {
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

    // vvv this value must point to DataBufferBlock
    auto functionAddress = (DWORD *) 0xCCCCCCCC;
    auto lpMalloc        = (malloc *) functionAddress[0];
    auto lpFree          = (free *) functionAddress[1];
    auto lpWSAStartup    = (WSAStartup *) functionAddress[2];
    auto lpWSACleanup    = (WSACleanup *) functionAddress[3];
    auto lpSocket        = (socket *) functionAddress[4];
    auto lpConnect       = (connect *) functionAddress[5];
    auto lpSend          = (send *) functionAddress[6];
    auto lpRecv          = (recv *) functionAddress[7];
    auto lpClosesocket   = (closesocket *) functionAddress[8];
    auto lpHtons         = (htons *) functionAddress[9];
    auto lpInet_addr     = (inet_addr *) functionAddress[10];

    auto *wsaData = (WSAData *) lpMalloc(sizeof(WSAData));
    lpWSAStartup(MAKEWORD(2, 2), wsaData);
    SOCKET hSocket = lpSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) {
        return;
    }

    const char strIP[] = {'1', '2', '7', '.', '0', '.', '0', '.', '1', '\0'};
    auto       *addr   = (sockaddr_in *) lpMalloc(sizeof(sockaddr_in));
    addr->sin_family           = AF_INET;
    addr->sin_port             = lpHtons(9999);
    addr->sin_addr.S_un.S_addr = lpInet_addr(strIP); // 或者也可以写成 0x0100007F

    if (lpConnect(hSocket, (sockaddr *) addr, sizeof(sockaddr)) == SOCKET_ERROR) {
        return;
    }

    char strBuf[] = {'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd', '\0'};
    if (lpSend(hSocket, strBuf, sizeof(strBuf), 0) == SOCKET_ERROR) {
        return;
    }
    char strRecvBuf[1024];
    if (lpRecv(hSocket, strRecvBuf, sizeof(strRecvBuf), 0) == SOCKET_ERROR) {
        return;
    }


    lpClosesocket(hSocket);
    lpWSACleanup();

    lpFree(addr);
    lpFree(wsaData);

    __asm {
            popad
            nop
    }
}

#pragma runtime_checks( "", restore )
#pragma optimize( "", on )


/// shell code base end

/**
 * 以 FunPtr 函数指针开始（会自动处理增量链接），获取函数的起始地址和大小
 *
 * @param FunPtr
 * @param EndSign
 * @param SignSize
 * @return
 */
std::optional<std::pair<DWORD, DWORD>> getFunBeginAddressAndSize(DWORD FunPtr, std::uint8_t *EndSign, std::size_t SignSize) {
    std::size_t functionSize = 0;

    // 增量链接(debug模式) 需要 jmp 到函数实际地址再计算
    if (*(uint8_t *) (FunPtr) == 0xE9) {
        DWORD offset = *(DWORD *) (FunPtr + 1) + 5;
        FunPtr = (DWORD) (FunPtr + offset);
    }
    while (true) {
        std::size_t sameSignSize = 0;

        for (std::size_t i = 0; i < SignSize; ++i) {
            if (*(uint8_t *) (FunPtr + i + functionSize) == EndSign[i]) {
                ++sameSignSize;
            }
        }
        if (sameSignSize == SignSize)
            return {{FunPtr, functionSize + SignSize}};

        // 找不到函数结束标识，并且 0x1000 是目标进程申请的内存最大空间
        if (functionSize > 0x1000) {
            break;
        }
        ++functionSize;
    }
    return std::nullopt;
}

ShellCodeMaker::ShellCodeMaker() {

}

std::vector<std::uint8_t> ShellCodeMaker::makeSocketShellCode(DWORD DataBufferBlockAddr, DWORD FunctionAddrStructOffset) {
    std::uint8_t endSign[] = {0x61, 0x90};

    auto funBeginAddAndSize = getFunBeginAddressAndSize((DWORD) socketShellCodeBase, endSign, 2);
    if (!funBeginAddAndSize) throw std::runtime_error("getFunBeginAddressAndSize");

    std::vector<std::uint8_t> codeBytes(funBeginAddAndSize->second);
    memcpy(codeBytes.data(), (void *) funBeginAddAndSize->first, funBeginAddAndSize->second);


    // 找到第一个值为 0xCCCCCCCC 的地址替换为目标进程的 DataBufferBlock 地址 + [Socket/Pipe/Shared_Memory]FunctionAddrOffset
    for (std::size_t i = 0; i < codeBytes.size() - sizeof(DWORD) + 1; ++i) {
        DWORD &value = *(DWORD *) (codeBytes.data() + i);
        if (value == 0xCCCCCCCC) {
            value = (DWORD) ((uint8_t *) DataBufferBlockAddr + FunctionAddrStructOffset);
            break;
        }
    }
    return codeBytes;
}
