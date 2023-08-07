

#include "ShellCodeMaker.h"
#include "../Utils/MyException.h"

/// shell code base begin

#pragma runtime_checks( "", off )
#pragma optimize( "", off )

// 请在调用函数前就在函数头部填充 pushad
__declspec(safebuffers)
void socketShellCodeBase86() {
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
    auto dataBufferAddr      = (std::uint8_t *) 0x99999999;
    // vvv 填入储存寄存器的地址
    auto registerStorageAddr = (std::uint8_t *) 0x99999999;

    /// vvv 获取控制块参数
    DWORD offsetSocketFunction = *(DWORD *) (dataBufferAddr + 16);
    auto  socketFunctionAddr   = (DWORD *) (dataBufferAddr + offsetSocketFunction);

    bool   &isSocketEstablish = *(bool *) (dataBufferAddr + 4);
    SOCKET &hSocket           = *(SOCKET *) (dataBufferAddr + 12);
    /// ^^^ 获取控制块参数
    /// vvv 获取所需函数地址
    auto   lpMalloc           = (malloc *) socketFunctionAddr[0];
    auto   lpFree             = (free *) socketFunctionAddr[1];
    auto   lpWSAStartup       = (WSAStartup *) socketFunctionAddr[2];
    auto   lpWSACleanup       = (WSACleanup *) socketFunctionAddr[3];
    auto   lpSocket           = (socket *) socketFunctionAddr[4];
    auto   lpConnect          = (connect *) socketFunctionAddr[5];
    auto   lpSend             = (send *) socketFunctionAddr[6];
    auto   lpRecv             = (recv *) socketFunctionAddr[7];
    auto   lpClosesocket      = (closesocket *) socketFunctionAddr[8];
    auto   lpHtons            = (htons *) socketFunctionAddr[9];
    auto   lpInet_addr        = (inet_addr *) socketFunctionAddr[10];
    /// ^^^ 获取所需函数地址

    if (!isSocketEstablish) {
        // 建立socket
        auto *allocWsaData = (WSAData *) lpMalloc(sizeof(WSAData));
        lpWSAStartup(MAKEWORD(2, 2), allocWsaData);
        SOCKET socketHandle = lpSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socketHandle == INVALID_SOCKET) {
            lpWSACleanup();
            lpFree(allocWsaData);
            return;
        }

        const char strIP[]          = {'1', '2', '7', '.', '0', '.', '0', '.', '1', '\0'};
        auto       *allocSockAddrIn = (sockaddr_in *) lpMalloc(sizeof(sockaddr_in));
        allocSockAddrIn->sin_family           = AF_INET;
        allocSockAddrIn->sin_port             = lpHtons(9999);
        allocSockAddrIn->sin_addr.S_un.S_addr = lpInet_addr(strIP); // 或者也可以写成 0x0100007F

        if (lpConnect(socketHandle, (sockaddr *) allocSockAddrIn, sizeof(sockaddr)) == SOCKET_ERROR) {
            lpClosesocket(socketHandle);
            lpWSACleanup();
            lpFree(allocSockAddrIn);
            lpFree(allocWsaData);
            return;
        }

        // 保存寄存器（4 * 8）
        if (lpSend(socketHandle, (char *) registerStorageAddr, 32, 0) == SOCKET_ERROR) {
            lpClosesocket(socketHandle);
            lpWSACleanup();
            lpFree(allocSockAddrIn);
            lpFree(allocWsaData);
            return;
        }

        // 不能使用 {0}，因为不能使用memset
        char      recvBuf[16];
        for (char &i: recvBuf) {
            i = '\0';
        }
        if (lpRecv(socketHandle, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            lpClosesocket(socketHandle);
            lpWSACleanup();
            lpFree(allocSockAddrIn);
            lpFree(allocWsaData);
            return;
        }

        const char strServerResponse[] = {'R', 'e', 's', 'p', 'o', 'n', 's', 'e', 'O', 'K', '\0'};

        std::size_t sameLen = 0;

        for (std::size_t i = 0; i < sizeof(strServerResponse) / sizeof(strServerResponse[0]); ++i) {
            if (recvBuf[i] == strServerResponse[i])
                ++sameLen;
            else
                break;
        }
        if (sameLen == sizeof(strServerResponse) / sizeof(strServerResponse[0])) {
            // 通信成功
            isSocketEstablish = true;
            hSocket = socketHandle;
        }
    } else {
        // 保存寄存器（4 * 8）
        if (lpSend(hSocket, (char *) registerStorageAddr, 32, 0) == SOCKET_ERROR) {
            lpClosesocket(hSocket);
            lpWSACleanup();
            isSocketEstablish = false;
            return;
        }

        // 不能使用 {0}，因为不能使用memset
        char      recvBuf[16];
        for (char &i: recvBuf) {
            i = '\0';
        }
        if (lpRecv(hSocket, recvBuf, sizeof(recvBuf), 0) == SOCKET_ERROR) {
            lpClosesocket(hSocket);
            lpWSACleanup();
            isSocketEstablish = false;
            return;
        }

        const char strServerResponse[] = {'R', 'e', 's', 'p', 'o', 'n', 's', 'e', 'O', 'K', '\0'};

        std::size_t sameLen = 0;

        for (std::size_t i = 0; i < sizeof(strServerResponse) / sizeof(strServerResponse[0]); ++i) {
            if (recvBuf[i] == strServerResponse[i])
                ++sameLen;
            else
                break;
        }
    }

    __asm {
            nop
            nop
    }
}

#pragma runtime_checks( "", restore )
#pragma optimize( "", on )


/// shell code base end

/**
 * 一定有值，否则异常，不需要判断是否有值
 * 以 FunPtr 函数指针开始（会自动处理增量链接），获取函数的起始地址和大小
 *
 * @param FunPtr
 * @param EndSign
 * @param SignSize
 * @return
 */
std::pair<DWORD, DWORD> getFuncAddressAndSize(DWORD FunPtr, const std::uint8_t *EndSign, std::size_t SignSize) {
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
        if (sameSignSize == SignSize) {
            // 找到 ret 但是不要加入 ret
            for (std::size_t i = 0; i < 0x10; ++i) {
                if (*(uint8_t *) (FunPtr + i + functionSize) == 0xC3) {
                    return {FunPtr, functionSize + i};
                }
            }
            // 找不到 ret
            break;
        }
        // 找不到函数结束标识，并且 0x1000 是目标进程申请的内存最大空间
        if (functionSize > 0x1000) {
            break;
        }
        ++functionSize;
    }
    throw MyException("calc shellcode size", __FUNCTION__);
}

/**
 * 一定有值，否则异常，不需要判断是否有值
 *
 * @param DataPtr
 * @param Begin
 * @param End
 * @param Sign
 * @param PatchValue
 * @return
 */
DWORD patchAddress(std::uint8_t *DataPtr, std::size_t Begin, std::size_t End, DWORD Sign, DWORD PatchValue) {
    for (std::size_t i = Begin; i < End; ++i) {
        DWORD &value = *(DWORD *) (DataPtr + i);
        if (value == Sign) {
            value = PatchValue;
            return i;
        }
    }
    throw MyException("can not find sign", __FUNCTION__);
}

ShellCodeMaker::ShellCodeMaker() {

}

std::vector<std::uint8_t> ShellCodeMaker::makeSocketShellCode(DWORD DataBufferBlockAddr, DWORD RegisterStorageAddr) {
    std::uint8_t endSign[] = {0x90, 0x90};

    auto funBeginAddAndSize = getFuncAddressAndSize((DWORD) socketShellCodeBase86, endSign, 2);

    std::vector<std::uint8_t> codeBytes(funBeginAddAndSize.second);
    memcpy(codeBytes.data(), (void *) funBeginAddAndSize.first, funBeginAddAndSize.second);

    // 找到第一个 0xCCCCCCCC 替换为控制块的地址
    // 找到第二个 0xCCCCCCCC 替换为保存寄存器的地址
    std::size_t patchPos = 0;
    patchPos = patchAddress(codeBytes.data(), patchPos, codeBytes.size() - sizeof(DWORD) + 1, 0x99999999, DataBufferBlockAddr);
    patchAddress(codeBytes.data(), patchPos + sizeof(DWORD), codeBytes.size() - sizeof(DWORD) + 1, 0x99999999, RegisterStorageAddr);

    // 保存寄存器到 $shellcode + 0xEF9 偏移处，即 RegisterStorageAddr 处，但是这里是写死的
    codeBytes.insert(codeBytes.begin(),
                     {0x60, 0xe8, 0x51, 0x00, 0x00, 0x00, 0x50, 0x53, 0x05, 0xf9, 0x0e, 0x00, 0x00, 0x8b, 0x5c, 0x24, 0x28, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b,
                      0x5c, 0x24, 0x2c, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x30, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x34, 0x89, 0x18,
                      0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x38, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x3c, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c,
                      0x24, 0x40, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x44, 0x89, 0x18, 0x5b, 0x58, 0x61, 0xeb, 0x04, 0x8b, 0x04, 0x24, 0xc3});
    // 添加 pushad
    codeBytes.insert(codeBytes.begin(), 0x60);

    // 添加 popad
    codeBytes.insert(codeBytes.end(), 0x61);

    return codeBytes;
}
