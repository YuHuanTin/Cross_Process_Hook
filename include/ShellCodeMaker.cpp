#include "ShellCodeMaker.h"



ShellCodeMaker::ShellCodeMaker() {

}

void ShellCodeMaker::makeShellCode() {
//    using namespace asmjit;
//    using namespace x86;
//
//    Environment env;
//    env.setArch(Arch::kX86);
//    CodeHolder code;
//    code.init(env);
//
//    x86::Assembler a(&code);
//    a.
//
//    auto *wsaData = (WSAData *) lpMalloc(sizeof(WSAData));
//    lpWSAStartup(MAKEWORD(2, 2), wsaData);
//    SOCKET hSocket = lpSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if (hSocket == INVALID_SOCKET) {
//        perror("Socket");
//        return;
//    }
//
//    const char strIP[] = {'1', '2', '7', '.', '0', '.', '0', '.', '1', '\0'};
//    auto *addr = (sockaddr_in *) lpMalloc(sizeof(sockaddr_in));
//    addr->sin_family = AF_INET;
//    addr->sin_port = lpHtons(9999);
//    addr->sin_addr.S_un.S_addr = lpInet_addr(strIP);
//
//    if (lpConnect(hSocket, (sockaddr *)addr, sizeof(sockaddr)) == SOCKET_ERROR) {
//        perror("Connect");
//        return;
//    }
//
//    char strBuf[] = {'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd', '\0'};
//    if (lpSend(hSocket, strBuf, sizeof(strBuf), 0) == SOCKET_ERROR) {
//        perror("Send");
//        return;
//    }
//    char strRecvBuf[1024]{0};
//    if (lpRecv(hSocket, strRecvBuf, sizeof(strRecvBuf), 0) == SOCKET_ERROR) {
//        perror("Recv");
//        return;
//    }
//    printf("server return data: %s\n", strRecvBuf);
//
//    lpClosesocket(hSocket);
//    lpWSACleanup();
//
//    lpFree(addr);
//    lpFree(wsaData);
//
//
//    CodeBuffer& buffer = code.textSection()->buffer();
//

}
