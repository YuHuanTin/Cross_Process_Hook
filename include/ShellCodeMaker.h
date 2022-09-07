#ifndef CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#define CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#include "Data_Struct.h"

unsigned char p_rawShellCode[] = {
        0x60, 0xA3, 0xCC, 0xCC, 0xCC, 0xCC, 0x89, 0x1D,
        0xCC, 0xCC, 0xCC, 0xCC, 0x89, 0x0D, 0xCC, 0xCC,
        0xCC, 0xCC, 0x89, 0x15, 0xCC, 0xCC, 0xCC, 0xCC,
        0x89, 0x2D, 0xCC, 0xCC, 0xCC, 0xCC, 0x89, 0x25,
        0xCC, 0xCC, 0xCC, 0xCC, 0x89, 0x35, 0xCC, 0xCC,
        0xCC, 0xCC, 0x89, 0x3D, 0xCC, 0xCC, 0xCC, 0xCC,
        0x31, 0xDB, 0x31, 0xC0, 0xFF, 0x35, 0xCC, 0xCC,
        0xCC, 0xCC, 0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
        0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC, 0xE8, 0x5A,
        0x00, 0x00, 0x00, 0x03, 0x05, 0xCC, 0xCC, 0xCC,
        0xCC, 0xFF, 0xD0, 0x89, 0xC3, 0x83, 0xC4, 0x0C,
        0x31, 0xC0, 0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
        0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC, 0xFF, 0x35,
        0xCC, 0xCC, 0xCC, 0xCC, 0xFF, 0x35, 0xCC, 0xCC,
        0xCC, 0xCC, 0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
        0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC, 0x53, 0xE8,
        0x21, 0x00, 0x00, 0x00, 0x03, 0x05, 0xCC, 0xCC,
        0xCC, 0xCC, 0xFF, 0xD0, 0x83, 0xC4, 0x1C, 0x31,
        0xC0, 0x6A, 0xFF, 0x53, 0xE8, 0x0C, 0x00, 0x00,
        0x00, 0x03, 0x05, 0xCC, 0xCC, 0xCC, 0xCC, 0xFF,
        0xD0, 0x83, 0xC4, 0x08, 0x61, 0x31, 0xC0, 0x31,
        0xC9, 0x31, 0xF6, 0x31, 0xFF, 0x64, 0xA1, 0x30,
        0x00, 0x00, 0x00, 0x8B, 0x40, 0x0C, 0x83, 0xC0,
        0x1C, 0xFF, 0x70, 0x04, 0x8B, 0x00, 0x50, 0x3B,
        0x44, 0x24, 0x04, 0x74, 0x19, 0x8B, 0x48, 0x1C,
        0x81, 0xE1, 0xFF, 0xFF, 0x00, 0x00, 0x8B, 0x70,
        0x20, 0xBF, 0xCC, 0xCC, 0xCC, 0xCC, 0xFC, 0xF3,
        0xA6, 0xE3, 0xEA, 0x58, 0xEB, 0xDE, 0x83, 0xC4,
        0x08, 0x8B, 0x40, 0x08, 0xC3
};
/*
 * 2    1024+128+0
 * 8    1024+128+4
 * 14   1024+128+8
 * 20   1024+128+C
 * 26   1024+128+10
 * 32   1024+128+14
 * 38   1024+128+18
 * 44   1024+128+1C
 *
 * 54   arg3    OpenProcess_dwProcessId
 * 60   arg2    OpenProcess_bInheritHandle
 * 66   arg1    OpenProcess_dwDesiredAccess
 * 77   arg11   RVA_OpenProcess
 *
 * 92   arg9    CreateRemoteThread_lpThreadId
 * 98   arg8    CreateRemoteThread_dwCreationFlags
 * 104  arg7    CreateRemoteThread_lpParameter
 * 110  arg6    CreateRemoteThread_lpStartAddress
 * 116  arg5    CreateRemoteThread_dwStackSize
 * 122  arg4    CreateRemoteThread_lpThreadAttributes
 * 134  arg12   RVA_CreateRemoteThread
 *
 * 155  arg13   RVA_WaitForSingleObject
 *
 * 210  arg10   ptr_DllName
 */

class c_ShellCodeMaker{
private:

public:
    explicit c_ShellCodeMaker(st_Params params){

    }
};


#endif //CROSS_PROCESS_HOOK_SHELLCODEMAKER_H