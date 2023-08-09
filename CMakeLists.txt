cmake_minimum_required(VERSION 3.23)
project(Cross_Process_Hook)

set(CMAKE_CXX_STANDARD 17)


find_package(fmt CONFIG REQUIRED)

add_executable(Cross_Process_Hook
        sample.cpp
        ShellCodeMaker/ShellCodeBase.h
        ShellCodeMaker/ShellCodeBase.cpp
        DataStruct/DataStruct.h
        DllFuncRVAReader/RVAReaderBase.cpp
        DllFuncRVAReader/RVAReaderBase.h
        Utils/Utils.cpp
        Utils/Utils.h
        ControlBlockManager/ControlBlockBase.cpp
        ControlBlockManager/ControlBlockBase.h
        Utils/MyException.cpp
        Utils/MyException.h
        DllFuncRVAReader/ReadFromFile.cpp
        DllFuncRVAReader/ReadFromFile.h
        DllFuncRVAReader/ReadFromMemory.cpp
        DllFuncRVAReader/ReadFromMemory.h
        ControlBlockManager/SocketControlBlock.cpp
        ControlBlockManager/SocketControlBlock.h
        ProcessHooker/ProcessHookBase.cpp
        ProcessHooker/ProcessHookBase.h
        ProcessHooker/SocketHook.cpp
        ProcessHooker/SocketHook.h
        ShellCodeMaker/SocketShellCodeX86.cpp
        ShellCodeMaker/SocketShellCodeX86.h
        ProcessHooker/HookPoint.cpp
        ProcessHooker/HookPoint.h
)

target_link_libraries(Cross_Process_Hook fmt::fmt)
