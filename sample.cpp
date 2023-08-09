
#include <fmt/format.h>
#include <chrono>
#include "ProcessHooker/SocketHook.h"
#include "Utils/Utils.h"

bool funcRecv(HANDLE ProcessHandle, DataBuffer *DataBufferPtr) {
    try {

    } catch (std::exception &exception) {
        fmt::println("occour a exception but catch: {}", exception.what());
        return false;
    }
    return true;
}

int main() {
    setbuf(stdout, nullptr);
    try {
        

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

    return 0;
}