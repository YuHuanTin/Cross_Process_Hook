cmake_minimum_required(VERSION 3.23)
project(Cross_Process_Hook)

set(CMAKE_CXX_STANDARD 17)

add_executable(Cross_Process_Hook main.cpp include/PreProcess.h include/ShellCodeMaker.h include/Cross_Process_Hook.h include/Data_Struct.h)

