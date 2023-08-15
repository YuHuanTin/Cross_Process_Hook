cmake_minimum_required(VERSION 3.23)
project(Cross_Process_Hook)

set(CMAKE_CXX_STANDARD 17)


find_package(fmt CONFIG REQUIRED)

add_executable(Cross_Process_Hook
        sample.cpp
        ProcessHooker/ShellCodeMaker/ShellCodeBase.h
        ProcessHooker/ShellCodeMaker/ShellCodeBase.cpp
        DataStruct/DataStruct.h
        ProcessHooker/ControlBlockManager/ModuleFuncAddrReader/ReaderBase.cpp
        ProcessHooker/ControlBlockManager/ModuleFuncAddrReader/ReaderBase.h
        Utils/Utils.cpp
        Utils/Utils.h
        ProcessHooker/ControlBlockManager/ControlBlockBase.cpp
        ProcessHooker/ControlBlockManager/ControlBlockBase.h
        Utils/MyException.cpp
        Utils/MyException.h
        ProcessHooker/ControlBlockManager/ModuleFuncAddrReader/Reader_FromFile.cpp
        ProcessHooker/ControlBlockManager/ModuleFuncAddrReader/Reader_FromFile.h
        ProcessHooker/ControlBlockManager/ModuleFuncAddrReader/Reader_FromMemory.cpp
        ProcessHooker/ControlBlockManager/ModuleFuncAddrReader/Reader_FromMemory.h
        ProcessHooker/ControlBlockManager/ControlBlock_Socket_TCP.cpp
        ProcessHooker/ControlBlockManager/ControlBlock_Socket_TCP.h
        ProcessHooker/HookBase.cpp
        ProcessHooker/HookBase.h
        ProcessHooker/Hook_Socket_TCP.cpp
        ProcessHooker/Hook_Socket_TCP.h
        ProcessHooker/Hook_Socket_UDP.cpp
        ProcessHooker/Hook_Socket_UDP.h
        ProcessHooker/HookPoint.cpp
        ProcessHooker/HookPoint.h
        ProcessHooker/ShellCodeMaker/ShellCode_Socket_TCP_X86.cpp
        ProcessHooker/ShellCodeMaker/ShellCode_Socket_TCP_X86.h
        ProcessHooker/ShellCodeMaker/ShellCode_Socket_UDP_X86.cpp
        ProcessHooker/ShellCodeMaker/ShellCode_Socket_UDP_X86.h
        ProcessHooker/ControlBlockManager/ControlBlock_Socket_UDP.cpp
        ProcessHooker/ControlBlockManager/ControlBlock_Socket_UDP.h
)



target_link_libraries(Cross_Process_Hook fmt::fmt)
