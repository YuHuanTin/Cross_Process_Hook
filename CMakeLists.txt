cmake_minimum_required(VERSION 3.23)
project(Cross_Process_Hook)

set(CMAKE_CXX_STANDARD 17)


find_package(fmt CONFIG REQUIRED)

add_executable(Cross_Process_Hook
        sample.cpp
        include/ShellCodeMaker.h
        include/ShellCodeMaker.cpp
        include/Cross_Process_Hook.h
        include/Cross_Process_Hook.cpp
        include/Data_Struct.h
        include/Data_Struct.cpp
        DllFuncRVAReader/RVAReaderBase.cpp
        DllFuncRVAReader/RVAReaderBase.h
        Utils/Utils.cpp
        Utils/Utils.h
        include/ControlBlockManager.cpp
        include/ControlBlockManager.h
        Utils/MyException.cpp
        Utils/MyException.h
        DllFuncRVAReader/ReadFromFile.cpp
        DllFuncRVAReader/ReadFromFile.h
        DllFuncRVAReader/ReadFromMemory.cpp
        DllFuncRVAReader/ReadFromMemory.h
)

target_link_libraries(Cross_Process_Hook fmt::fmt)
