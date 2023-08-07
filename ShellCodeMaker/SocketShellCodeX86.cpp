

#include "SocketShellCodeX86.h"

/// shell code base begin

#pragma runtime_checks( "", off )
#pragma optimize( "", off )

// ���ڵ��ú���ǰ���ں���ͷ����� pushad
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

    // vvv ������ƿ�ĵ�ַ
    auto dataBufferAddr      = (std::uint8_t *) 0x99999999;
    // vvv ���봢��Ĵ����ĵ�ַ
    auto registerStorageAddr = (std::uint8_t *) 0x99999999;

    /// vvv ��ȡ���ƿ����
    DWORD offsetSocketFunction = *(DWORD *) (dataBufferAddr + 16);
    auto  socketFunctionAddr   = (DWORD *) (dataBufferAddr + offsetSocketFunction);

    bool   &isSocketEstablish = *(bool *) (dataBufferAddr + 4);
    SOCKET &hSocket           = *(SOCKET *) (dataBufferAddr + 12);
    /// ^^^ ��ȡ���ƿ����
    /// vvv ��ȡ���躯����ַ
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
    /// ^^^ ��ȡ���躯����ַ

    if (!isSocketEstablish) {
        // ����socket
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
        allocSockAddrIn->sin_addr.S_un.S_addr = lpInet_addr(strIP); // ����Ҳ����д�� 0x0100007F

        if (lpConnect(socketHandle, (sockaddr *) allocSockAddrIn, sizeof(sockaddr)) == SOCKET_ERROR) {
            lpClosesocket(socketHandle);
            lpWSACleanup();
            lpFree(allocSockAddrIn);
            lpFree(allocWsaData);
            return;
        }

        // ����Ĵ�����4 * 8��
        if (lpSend(socketHandle, (char *) registerStorageAddr, 32, 0) == SOCKET_ERROR) {
            lpClosesocket(socketHandle);
            lpWSACleanup();
            lpFree(allocSockAddrIn);
            lpFree(allocWsaData);
            return;
        }

        // ����ʹ�� {0}����Ϊ����ʹ��memset
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
            // ͨ�ųɹ�
            isSocketEstablish = true;
            hSocket = socketHandle;
        }
    } else {
        // ����Ĵ�����4 * 8��
        if (lpSend(hSocket, (char *) registerStorageAddr, 32, 0) == SOCKET_ERROR) {
            lpClosesocket(hSocket);
            lpWSACleanup();
            isSocketEstablish = false;
            return;
        }

        // ����ʹ�� {0}����Ϊ����ʹ��memset
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

std::vector<std::uint8_t> SocketShellCodeX86::makeShellCode(DWORD ControlBlockAddr, DWORD RegisterStorageAddr) {
    std::uint8_t endSign[] = {0x90, 0x90};
    auto funBeginAddAndSize = getFuncAddressAndSize((DWORD) socketShellCodeX86, endSign, 2);

    std::vector<std::uint8_t> codeBytes(funBeginAddAndSize.second);
    memcpy(codeBytes.data(), (void *) funBeginAddAndSize.first, funBeginAddAndSize.second);

    // �ҵ���һ�� 0xCCCCCCCC �滻Ϊ���ƿ�ĵ�ַ
    // �ҵ��ڶ��� 0xCCCCCCCC �滻Ϊ����Ĵ����ĵ�ַ
    std::size_t patchPos = 0;
    patchPos = patchAddress(codeBytes.data(), patchPos, codeBytes.size() - sizeof(DWORD) + 1, 0x99999999, ControlBlockAddr);
    patchAddress(codeBytes.data(), patchPos + sizeof(DWORD), codeBytes.size() - sizeof(DWORD) + 1, 0x99999999, RegisterStorageAddr);

    return insertPushadAndPopad(insertRegisterSaveCode(codeBytes));
}
