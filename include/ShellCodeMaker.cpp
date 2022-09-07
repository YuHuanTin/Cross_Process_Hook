#include "ShellCodeMaker.h"

void c_ShellCodeMaker::MakeCode(LPVOID allocMemAddr, st_Params &params) {
    unsigned char m_rawShellCode[] = {
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
    DWORD m_offsetAddr;
    //init shellcode[2]~shellcode[44] register
    for (int i = 0; i < 8; ++i) {
        m_offsetAddr = (DWORD)allocMemAddr + 1024 + 128 + (i * 4);
        m_rawShellCode[2 + (i * 6)] = m_offsetAddr & 0xFF;
        m_rawShellCode[2 + (i * 6) + 1] = m_offsetAddr >> 8 & 0xFF;
        m_rawShellCode[2 + (i * 6) + 2] = m_offsetAddr >> 16 & 0xFF;
        m_rawShellCode[2 + (i * 6) + 3] = m_offsetAddr >> 24 & 0xFF;
    }

    //init shellcode[54]~shellcode[66] arg1~arg3
    for (int i = 0; i < 3; ++i) {
        m_offsetAddr = (DWORD)allocMemAddr + 1024 + (i * 4);
        m_rawShellCode[66 - (i * 6)] = m_offsetAddr & 0xFF;
        m_rawShellCode[66 - (i * 6) + 1] = m_offsetAddr >> 8 & 0xFF;
        m_rawShellCode[66 - (i * 6) + 2] = m_offsetAddr >> 16 & 0xFF;
        m_rawShellCode[66 - (i * 6) + 3] = m_offsetAddr >> 24 & 0xFF;
    }

    //init shellcode[92]~shellcode[122] arg4~arg9
    for (int i = 0; i < 6; ++i) {
        m_offsetAddr = (DWORD)allocMemAddr + 1024 + 3 * 4 + (i * 4);
        m_rawShellCode[122 - (i * 6)] = m_offsetAddr & 0xFF;
        m_rawShellCode[122 - (i * 6) + 1] = m_offsetAddr >> 8 & 0xFF;
        m_rawShellCode[122 - (i * 6) + 2] = m_offsetAddr >> 16 & 0xFF;
        m_rawShellCode[122 - (i * 6) + 3] = m_offsetAddr >> 24 & 0xFF;
    }

    //arg10
    m_offsetAddr = (DWORD)allocMemAddr + 1024 + 9 * 4;
    m_rawShellCode[210] = m_offsetAddr & 0xFF;
    m_rawShellCode[210 + 1] = m_offsetAddr >> 8 & 0xFF;
    m_rawShellCode[210 + 2] = m_offsetAddr >> 16 & 0xFF;
    m_rawShellCode[210 + 3] = m_offsetAddr >> 24 & 0xFF;

    //arg11
    m_offsetAddr = (DWORD)allocMemAddr + 1024 + 62;
    m_rawShellCode[77] = m_offsetAddr & 0xFF;
    m_rawShellCode[77 + 1] = m_offsetAddr >> 8 & 0xFF;
    m_rawShellCode[77 + 2] = m_offsetAddr >> 16 & 0xFF;
    m_rawShellCode[77 + 3] = m_offsetAddr >> 24 & 0xFF;

    //arg12
    m_offsetAddr+=4;
    m_rawShellCode[134] = m_offsetAddr & 0xFF;
    m_rawShellCode[134 + 1] = m_offsetAddr >> 8 & 0xFF;
    m_rawShellCode[134 + 2] = m_offsetAddr >> 16 & 0xFF;
    m_rawShellCode[134 + 3] = m_offsetAddr >> 24 & 0xFF;

    //arg13
    m_offsetAddr+=4;
    m_rawShellCode[155] = m_offsetAddr & 0xFF;
    m_rawShellCode[155 + 1] = m_offsetAddr >> 8 & 0xFF;
    m_rawShellCode[155 + 2] = m_offsetAddr >> 16 & 0xFF;
    m_rawShellCode[155 + 3] = m_offsetAddr >> 24 & 0xFF;

    memcpy(shellCode.shellcode,m_rawShellCode, sizeof(m_rawShellCode));
}
