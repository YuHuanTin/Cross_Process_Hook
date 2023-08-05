
#include "Cross_Process_Hook.h"
#include "Data_Struct.h"
#include "DllFunctionRVAReader.h"
#include "ShellCodeMaker.h"
#include "../Utils/Utils.h"



ProcessHook::ProcessHook(const std::string &ProcessName, HookMethod HookMethod)
        : ProcessHook(Utils::RemoteProcess::getProcessID(ProcessName).value_or(-1), HookMethod) {}

ProcessHook::ProcessHook(DWORD ProcessID, HookMethod HookMethod)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_hookMethod(HookMethod) {
    if (m_processID == -1) throw std::runtime_error("bad process id");

    auto pDataBufferBlock = new DataBufferBlock{0};

    // 收集所需要的函数地址
    DllFunctionRVAReader rvaReader(m_processID, DllFunctionRVAReader::SEARCH_IN_FILE);

    bool bRet = rvaReader.initSearch("kernelbase.dll");
    if (!bRet) throw std::runtime_error("initSearch kernelbase");
    DWORD loadLibraryAddr = rvaReader.searchRVA("LoadLibraryA", true);
    DWORD freeLibraryAddr = rvaReader.searchRVA("FreeLibrary", true);

    if (HookMethod == HookMethod::Socket) {
        if (!rvaReader.isDllLoaded("ws2_32.dll") && !rvaReader.loadDll("ws2_32.dll", loadLibraryAddr)) {
            throw std::runtime_error("loadDll ws2_32");
        }
        if (!rvaReader.initSearch("ws2_32.dll")) {
            throw std::runtime_error("initSearch ws2_32");
        }

        pDataBufferBlock->HookMethod                         = Socket;
        pDataBufferBlock->PSocketFunctionAddress.WSAStartup  = rvaReader.searchRVA("WSAStartup", true);
        pDataBufferBlock->PSocketFunctionAddress.WSACleanup  = rvaReader.searchRVA("WSACleanup", true);
        pDataBufferBlock->PSocketFunctionAddress.socket      = rvaReader.searchRVA("socket", true);
        pDataBufferBlock->PSocketFunctionAddress.connect     = rvaReader.searchRVA("connect", true);
        pDataBufferBlock->PSocketFunctionAddress.send        = rvaReader.searchRVA("send", true);
        pDataBufferBlock->PSocketFunctionAddress.recv        = rvaReader.searchRVA("recv", true);
        pDataBufferBlock->PSocketFunctionAddress.closesocket = rvaReader.searchRVA("closesocket", true);
        pDataBufferBlock->PSocketFunctionAddress.htons       = rvaReader.searchRVA("htons", true);
        pDataBufferBlock->PSocketFunctionAddress.inet_addr   = rvaReader.searchRVA("inet_addr", true);

        if (!rvaReader.isDllLoaded("ucrtbase.dll") && !rvaReader.loadDll("ucrtbase.dll", loadLibraryAddr)) {
            throw std::runtime_error("loadDll ucrtbase");
        }
        if (!rvaReader.initSearch("ucrtbase.dll")) {
            throw std::runtime_error("initSearch ucrtbase");
        }

        pDataBufferBlock->PSocketFunctionAddress.malloc = rvaReader.searchRVA("malloc",true);
        pDataBufferBlock->PSocketFunctionAddress.free   = rvaReader.searchRVA("free",true);
    }

    auto dataBufferBlockAddress = Utils::RemoteProcess::allocMemory(m_processHandle);
    if (!dataBufferBlockAddress) throw std::runtime_error("allocMemory");
    m_dataBufferAddress = dataBufferBlockAddress.value();

    printf("%d", (int) m_dataBufferAddress);
    Utils::RemoteProcess::writeMemory(m_processHandle, m_dataBufferAddress, pDataBufferBlock, sizeof(DataBufferBlock));
}

bool ProcessHook::AddHook(DWORD HookAddress, unsigned int HookLen) {
    if (m_hookMethod == HookMethod::Socket) {

        ShellCodeMaker maker;
        auto data = maker.makeSocketShellCode((DWORD) m_dataBufferAddress, sizeof(DataBufferBlock) - sizeof(DataBufferBlock::PSocketFunctionAddress));
        auto allocAddr = Utils::RemoteProcess::allocMemory(m_processHandle);
        if (!allocAddr)
            return false;

        Utils::RemoteProcess::writeMemory(m_processHandle, allocAddr.value(), data.data(), data.size());

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

