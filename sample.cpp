
#include <fmt/format.h>
#include <chrono>
#include "ProcessHooker/SocketHook.h"
#include <functional>
bool funcRecv(DataBuffer *DataBufferPtr) {

    return true;
}

int main(){
    setbuf(stdout, nullptr);
    try {
        SocketHook hookWithSocket("notepad.exe");
        hookWithSocket.addHook(0x762C1DA0, 5);

        hookWithSocket.commitHook(funcRecv);

    } catch (std::exception &exception) {
        fmt::println("{}", exception.what());
    }

    /**
     *  std::optional 只用于判断是否有值
     *  如果碰到错误请直接抛异常而不是返回 std::nullopt
     */
    /**
     *  建立共享内存后通信（分别实现异步和同步）
     */
//    ProcessHook p("notepad.exe", HookMethodE::Socket, remote_cb);
//
//    p.AddHook(0x75000000,5);
//    p.CommitHook();
//    WaitForSingleObject(GetCurrentProcess(),INFINITE);
//    p.DeleteHook(0x75000000);


    return 0;
}