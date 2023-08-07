
#include "Cross_Process_Hook.h"
#include "ShellCodeMaker.h"
#include "../Utils/Utils.h"



ProcessHook::ProcessHook(const std::string &ProcessName, HookMethod HookMethod)
        : ProcessHook(Utils::RemoteProcess::getProcessID(ProcessName).value(), HookMethod) {}

ProcessHook::ProcessHook(DWORD ProcessID, HookMethod HookMethod)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_hookMethod(HookMethod) {

    m_controlBlockManager = std::make_unique<ControlBlockManager>(m_processID);
    if (HookMethod == HookMethod::Socket) {
        m_controlBlockManager->fillSocketArgs();
    }
    m_controlBlockManager->injectControlBlock(m_processHandle);
}

bool ProcessHook::AddHook(DWORD HookAddress, unsigned int HookLen) {
    if (m_hookMethod == HookMethod::Socket) {
        ShellCodeMaker maker;

        LPVOID hookAddress = Utils::RemoteProcess::allocMemory(m_processHandle);
        auto data = maker.makeSocketShellCode((DWORD) m_controlBlockManager->getRemoteControlBlockAddr(), (DWORD)hookAddress + 0xF00);
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
            do {
                iResult = recv(ClientSocket, (char *) buf.get(), 4 * 8, 0);
                if (iResult > 0) {
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


    }

    return true;
}

bool ProcessHook::CommitHook() {
    return false;
}

bool ProcessHook::DeleteHook(DWORD HookAddress) {
    return false;
}

ProcessHook::~ProcessHook() {
    CloseHandle(m_processHandle);
}

