

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

bool SocketHook::commitHook(std::function<bool(DataBuffer *)> FuncRecvData) {
    // jmp 跳转填充
    patchCodeJmp();

//    m_socketRecvThread = std::thread([]() {
//
//    }).detach();

    auto wsadata = Utils::AutoPtr::moveTypeOwner<WSAData>(new WSAData);
    WSAStartup(MAKEWORD(2, 2), wsadata.get());
    SOCKET      hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serAddr{
            AF_INET,//与socket创建时一样
            htons(9999),
            INADDR_ANY//绑定本机的任意端口
    };
    if (bind(hSocket, (sockaddr *) &serAddr, sizeof(serAddr)) == SOCKET_ERROR) {

    }
    if (listen(hSocket, SOMAXCONN) == SOCKET_ERROR) {

    }
    SOCKET ClientSocket = accept(hSocket, NULL, NULL);

    closesocket(hSocket);
    int         iResult;
    int         iSendResult;
    auto        buf = std::make_unique<DataBuffer>();
    std::size_t cnt = 0;
    do {
        ++cnt;
        iResult = recv(ClientSocket, (char *) buf.get(), sizeof(DataBuffer), 0);
        if (iResult > 0) {
            printf("cnt: %d\neax:0x%08lX\nebx:0x%08lX\necx:0x%08lX\nedx:0x%08lX\nesi:0x%08lX\nedi:0x%08lX\nesp:0x%08lX\nebp:0X%08lX\nfrom:0x%08lX\n",
                   cnt,
                   buf->eax,
                   buf->ebx,
                   buf->ecx,
                   buf->edx,
                   buf->esi,
                   buf->edi,
                   buf->esp,
                   buf->ebp,
                   buf->whereFrom);

            iSendResult = send(ClientSocket, "ResponseOK", 11, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);


    return false;
}

bool SocketHook::deleteHook(DWORD HookAddress) {
    return false;
}

