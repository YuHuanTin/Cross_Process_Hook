

#include <WinSock2.h>
#include "SocketHook.h"
#include "../ShellCodeMaker/SocketShellCodeX86.h"
#include "../Utils/Utils.h"
#include "../ControlBlockManager/SocketControlBlock.h"

#pragma comment(lib, "ws2_32.lib")

SocketHook::SocketHook(const std::string &ProcessName) : SocketHook(Utils::RemoteProcess::getProcessID(ProcessName).value()) {}

SocketHook::SocketHook(DWORD ProcessID) : ProcessHookBase(ProcessID) {
    m_controlBlock = std::make_unique<SocketControlBlock>(m_processID);
    m_controlBlock->fillControlBlockArgs();
    m_controlBlock->injectControlBlock();
}

void SocketHook::addHook(DWORD HookAddr, std::size_t HookLen) {
    SocketShellCodeX86 shellCodeX86;
    LPVOID             shellCodeAddr = Utils::RemoteProcess::allocMemory(m_processHandle);

    auto shellCode = shellCodeX86.makeShellCode((DWORD) m_controlBlock->getControlBlockRemoteAddr(), (DWORD) shellCodeAddr + 0xF00);
    Utils::RemoteProcess::writeMemory(m_processHandle, shellCodeAddr, shellCode.data(), shellCode.size());

    m_hooks.emplace_back(HookAddr, HookLen, shellCodeAddr, shellCode.size());
}

void SocketHook::commitHook(std::function<bool(HANDLE, DataBuffer *)> FuncRecvData) {
    // jmp 跳转填充
    patchCodeJmp();

    m_socketRecvThread = std::thread([FuncRecvData = std::move(FuncRecvData), this]() {
        // WSAStartup
        WSADATA wsaData;

        int iResult;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            return;
        }

        // Create a SOCKET for the server to listen for client connections.
        SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Setup the TCP listening socket
        sockaddr_in serAddr{
                AF_INET, //与socket创建时一样
                htons(9999),
                INADDR_ANY //绑定本机的任意端口
        };
        iResult = bind(ListenSocket, (sockaddr *) &serAddr, sizeof(serAddr));
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        // Accept a client socket
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }
        // No longer need server socket
        closesocket(ListenSocket);

        // Receive until the peer shuts down the connection
        DWORD iSendResult = 0;
        std::unique_ptr<DataBuffer> buf(new DataBuffer);
        do {
            iResult = recv(ClientSocket, (char *) buf.get(), sizeof(DataBuffer), 0);
            if (iResult > 0) {

                /// custom function to resolve data
                FuncRecvData(m_processHandle, buf.get());

                iSendResult = send(ClientSocket, "ResponseOK", 11, 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    return;
                }
            } else if (iResult == 0) {
                printf("Connection closing...\n");
            } else {
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }
        } while (iResult > 0);

        // shutdown the connection since we're done
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }

        // cleanup
        closesocket(ClientSocket);
        WSACleanup();
    });
}

bool SocketHook::deleteHook(DWORD HookAddress) {
    return false;
}

SocketHook::~SocketHook() {
    m_socketRecvThread.join();
}

