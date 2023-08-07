

#include "SocketHook.h"
#include "../ShellCodeMaker/SocketShellCodeX86.h"
#include "../Utils/Utils.h"
#include "../ControlBlockManager/SocketControlBlock.h"

#pragma comment(lib,"ws2_32.lib")

SocketHook::SocketHook(const std::string &ProcessName) : SocketHook(Utils::RemoteProcess::getProcessID(ProcessName).value()) {}

SocketHook::SocketHook(DWORD ProcessID) : ProcessHookBase(ProcessID) {
    m_controlBlock = std::make_unique<SocketControlBlock>(m_processID);
    m_controlBlock->fillControlBlockArgs();
    m_controlBlock->injectControlBlock();
}

bool SocketHook::addHook() {
    SocketShellCodeX86 shellCodeX86;

    LPVOID hookAddress = Utils::RemoteProcess::allocMemory(m_processHandle);
    auto data = shellCodeX86.makeShellCode((DWORD) m_controlBlock->getControlBlockRemoteAddr(), (DWORD) hookAddress + 0xF00);
    Utils::RemoteProcess::writeMemory(m_processHandle, hookAddress, data.data(), data.size());

    printf("%d\n", (int) hookAddress);

    while (1) {
        auto wsadata = Utils::AutoPtr::moveTypeOwner<WSAData>(new WSAData);
        WSAStartup(MAKEWORD(2, 2), wsadata.get());
        SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in serAddr{
                AF_INET,//与socket创建时一样
                htons(9999),
                INADDR_ANY//绑定本机的任意端口
        };
        if (bind(hSocket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {

        }
        if (listen(hSocket,SOMAXCONN) == SOCKET_ERROR){

        }
        SOCKET ClientSocket = accept(hSocket, NULL, NULL);

        closesocket(hSocket);
        int iResult;
        int iSendResult;
        auto buf = std::make_unique<DataBuffer>();
        std::size_t cnt = 0;
        do {
            ++cnt;
            iResult = recv(ClientSocket, (char *) buf.get(), 4 * 8, 0);
            if (iResult > 0) {
                printf("cnt: %d\n", cnt);
                printf("eax: 0x%08lX\n", buf->eax);
                printf("ebx: 0x%08lX\n", buf->ebx);
                printf("ecx: 0x%08lX\n", buf->ecx);
                printf("edx: 0x%08lX\n", buf->edx);
                printf("esi: 0x%08lX\n", buf->esi);
                printf("edi: 0x%08lX\n", buf->edi);
                printf("esp: 0x%08lX\n", buf->esp);
                printf("ebp: 0x%08lX\n", buf->ebp);



                iSendResult = send( ClientSocket, "ResponseOK", 11, 0 );
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    return 1;
                }
                printf("Bytes sent: %d\n", iSendResult);
            }
            else if (iResult == 0)
                printf("Connection closing...\n");
            else  {
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        } while (iResult > 0);
    }
    return false;
}

bool SocketHook::commitHook() {
    return false;
}

bool SocketHook::deleteHook(DWORD HookAddress) {
    return false;
}

