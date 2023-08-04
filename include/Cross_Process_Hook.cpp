
#include "Cross_Process_Hook.h"
#include "../Utils/StringUtils.h"
#include "../Utils/ProcessUtils.h"
#include "../Utils/MemoryManager.h"
#include "../Utils/AutoMemoryManager.h"

//bool ProcessHook::AddHook(DWORD HookAddress, unsigned HookLen) {
//    assert(("hook length cannot be less than 5", HookLen >= 5));
//    try {
//        LPVOID allocMemAddr = allocMem(processInformation.processHandle);
//
//        ShellCodeMaker shellCodeMaker;
//
//        // todo 构造shellcode
//
//        vHooks.push_back({HookAddress, HookLen, allocMemAddr});
//        return true;
//    } catch (runtime_error &error) {
//        printException(error);
//    }
//    return false;
//}
//
//bool ProcessHook::CommitHook() {
//    if (processInformation.hookMethod == HookMethodE::Socket) {
//
//    }
//    // todo 写入shellcode并且创建远程线程执行
//
//    return false;
//}
//
//bool ProcessHook::DeleteHook(DWORD HookAddress) {
//    try {
//        for (auto &hook: vHooks) {
//            if (hook.hookAddress == HookAddress) {
//                freeMem(processInformation.processHandle, hook.allocAddress);
//
//                // todo 恢复hook处的代码
//
//
//                return true;
//            }
//        }
//    } catch (runtime_error &error) {
//        printException(error);
//    }
//    return false;
//}


ProcessHook::ProcessHook(const std::string &ProcessName, HookMethod HookMethod)
        : ProcessHook(Utils::getProcessID(ProcessName).value_or(-1), HookMethod) {}

ProcessHook::ProcessHook(DWORD ProcessID, HookMethod HookMethod)
        : m_processID(ProcessID), m_hookMethod(HookMethod) {
    if (m_processID == -1) throw std::runtime_error("bad process id");


}


