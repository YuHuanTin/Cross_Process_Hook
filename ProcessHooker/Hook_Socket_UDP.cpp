

#include <WinSock2.h>
#include "Hook_Socket_UDP.h"
#include "ShellCodeMaker/ShellCode_Socket_UDP_X86.h"
#include "ControlBlockManager/ControlBlock_Socket_UDP.h"

#pragma comment(lib, "ws2_32.lib")

Hook_Socket_UDP::Hook_Socket_UDP(const std::string &ProcessName) : Hook_Socket_UDP(Utils::RemoteProcess::getProcessID(ProcessName).value()) {}

Hook_Socket_UDP::Hook_Socket_UDP(DWORD ProcessID) : HookBase(ProcessID) {
    m_controlBlock = std::make_unique<ControlBlock_Socket_UDP>(m_processID);
    m_controlBlock->fillControlBlockArgs();
    m_controlBlock->injectControlBlock();
}

void Hook_Socket_UDP::addHook(std::size_t HookAddr, std::size_t HookLen) {
    auto hookPoint = std::make_unique<HookPoint>(
            m_processID,
            m_controlBlock->getControlBlockRemoteAddr(),
            HookAddr,
            HookLen,
            std::make_unique<ShellCode_Socket_UDP_X86>());

    m_hooks.emplace_back(std::move(hookPoint));
}

void Hook_Socket_UDP::commitHook(std::function<bool(HANDLE, DataBuffer *)> FuncRecvData) {
    // 填充 jmp 到shellcode地址
    for (auto &hook: m_hooks) {
        hook->coverCodeJmp();
    }

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
        m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_hSocket == INVALID_SOCKET) {
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
        iResult = bind(m_hSocket, (sockaddr *) &serAddr, sizeof(serAddr));
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            closesocket(m_hSocket);
            WSACleanup();
            return;
        }

        // Receive until the peer shuts down the connection
        int                         serAddrSize = sizeof(serAddr);
        DWORD                       iSendResult = 0;
        std::unique_ptr<DataBuffer> buf(new DataBuffer);
        do {
            iResult = recvfrom(m_hSocket, (char *) buf.get(), sizeof(DataBuffer), 0, (sockaddr *) &serAddr, &serAddrSize);
            // udp never equal zero, unless the packet size is 0
            if (iResult >= 0) {

                /// custom function to resolve data
                if (!FuncRecvData(m_processHandle, buf.get())) {
                    break;
                }

                iSendResult = sendto(m_hSocket, "ReOK", 5, 0, (sockaddr *) &serAddr, serAddrSize);
                if (iSendResult == SOCKET_ERROR) {
                    printf("sendto failed with error: %d\n", WSAGetLastError());
                    closesocket(m_hSocket);
                    WSACleanup();
                    return;
                }
            } else {
                printf("recvfrom failed with error: %d\n", WSAGetLastError());
                closesocket(m_hSocket);
                WSACleanup();
                return;
            }
        } while (iResult > 0);

        // cleanup
        closesocket(m_hSocket);
        WSACleanup();
    });
}

void Hook_Socket_UDP::deleteHook(std::size_t HookAddress) {
    for (std::size_t i = 0; i < m_hooks.size(); ++i) {
        if (HookAddress == m_hooks.at(i)->getHookAddr()) {
            m_hooks.at(i)->recoverCodeJmp();
            m_hooks.erase(m_hooks.begin() + i);
            break;
        }
    }
}

Hook_Socket_UDP::~Hook_Socket_UDP() {
    if (m_socketRecvThread.joinable()) {
        /// 显式删除所有hook点防止再次进入目标进程的socket循环
        for (auto   &one: m_hooks) {
            one->recoverCodeJmp();
        }
        /// 关闭目标进程的socket
        std::size_t closeSocket = m_controlBlock->getControlBlockRef()->PSocketFunctionAddress.closesocket;
        std::size_t wsaCleanup  = m_controlBlock->getControlBlockRef()->PSocketFunctionAddress.WSACleanup;
        auto        hSocket     = Utils::RemoteProcess::readMemory<SOCKET>(
                m_processHandle,
                m_controlBlock->getControlBlockRemoteAddr() + offsetof(ControlBlock, hSocket),
                1
        );

        auto hThread = Utils::AutoPtr::moveHandleOwner(CreateRemoteThread(m_processHandle,
                                                                          nullptr,
                                                                          0,
                                                                          (LPTHREAD_START_ROUTINE) closeSocket,
                                                                          (LPVOID) *hSocket.get(),
                                                                          0,
                                                                          NULL)
        );
        WaitForSingleObject(hThread.get(), INFINITE);

        hThread = Utils::AutoPtr::moveHandleOwner(CreateRemoteThread(m_processHandle,
                                                                     nullptr,
                                                                     0,
                                                                     (LPTHREAD_START_ROUTINE) wsaCleanup,
                                                                     nullptr,
                                                                     0,
                                                                     NULL)
        );
        WaitForSingleObject(hThread.get(), INFINITE);
        /// 关闭自身的 socket
        closesocket(m_hSocket);

        m_socketRecvThread.join();
    }
}

