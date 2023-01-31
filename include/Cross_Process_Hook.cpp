#include "Cross_Process_Hook.h"
#include <TlHelp32.h>

using std::string;
using std::runtime_error;

DWORD getProcessID(const std::string &ProcessName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == nullptr) throw runtime_error("failed CreateToolhelp32Snapshot");

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        throw runtime_error("failed Process32First");
    }
    do {
        string tolowerExeFile     = pe.szExeFile;
        string tolowerProcessName = ProcessName;

        [&tolowerExeFile] {
            for (auto &one: tolowerExeFile) {
                if (isupper(one)) tolower(one);
            }
        };
        [&tolowerProcessName] {
            for (auto &one: tolowerProcessName) {
                if (isupper(one)) tolower(one);
            }
        };

        if (tolowerProcessName == tolowerExeFile) {
            CloseHandle(hSnapshot);
            return pe.th32ProcessID;
        }
    } while (Process32Next(hSnapshot, &pe));
    CloseHandle(hSnapshot);
    throw runtime_error("failed find process");
}

HANDLE getProcessHandle(DWORD ProcessID) {
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);
    if (handle == nullptr) throw runtime_error("failed OpenProcess");
    return handle;
}

LPVOID allocMem(HANDLE ProcessHandle) {
    LPVOID addr = VirtualAllocEx(ProcessHandle, nullptr, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (addr == nullptr) throw runtime_error("failed VirtualAllocEx");
    return addr;
}

void freeMem(HANDLE ProcessHandle, LPVOID AllocMem) {
    if (!VirtualFreeEx(ProcessHandle, AllocMem, 0, MEM_RELEASE))
        throw runtime_error("failed VirtualFreeEx");
}

void printException(std::exception &Error) {
    std::cout << "error: " << Error.what() << '\n';
}

ProcessHook::ProcessHook(const string &ProcessName, HookMethodE HookMethod, fn_cb CallBack) {
    assert(("illegal process name", ProcessName.length() > 0));
    assert(("unset hook method", HookMethod != HookMethodE::NONE));
    assert(("unset callback function", CallBack != nullptr));

    try {
        DWORD pid = getProcessID(ProcessName);
        processInformation.processHandle = getProcessHandle(pid);
        processInformation.hookMethod = HookMethod;
        processInformation.callback = CallBack;
    } catch (runtime_error &error) {
        printException(error);
    }
}
ProcessHook::ProcessHook(DWORD ProcessID, HookMethodE HookMethod, fn_cb CallBack) {
    assert(("unset hook method", HookMethod != HookMethodE::NONE));
    assert(("unset callback function", CallBack != nullptr));

    try {
        processInformation.processHandle = getProcessHandle(ProcessID);
        processInformation.hookMethod = HookMethod;
        processInformation.callback = CallBack;
    } catch (runtime_error &error) {
        printException(error);
    }
}
bool ProcessHook::CtorHook(DWORD HookAddress, unsigned HookLen) {
    assert(("hook length cannot be less than 5", HookLen >= 5));
    try {
        LPVOID allocMemAddr = allocMem(processInformation.processHandle);

        ShellCodeMaker shellCodeMaker;

        // todo 构造shellcode

        vHooks.push_back({HookAddress, HookLen, allocMemAddr});
        return true;
    } catch (runtime_error &error) {
        printException(error);
    }
    return false;
}

bool ProcessHook::CommitMem() {
    if (processInformation.hookMethod == HookMethodE::Socket) {

    }
    // todo 写入shellcode并且创建远程线程执行

    return false;
}

bool ProcessHook::DeleteHook(DWORD HookAddress) {
    try {
        for (auto &hook: vHooks) {
            if (hook.hookAddress == HookAddress) {
                freeMem(processInformation.processHandle, hook.allocAddress);

                // todo 恢复hook处的代码


                return true;
            }
        }
    } catch (runtime_error &error) {
        printException(error);
    }
    return false;
}


