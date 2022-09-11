#include "ShellCodeMaker.h"
//private

void c_ShellCodeMaker::MakeShellCode(LPVOID &allocMemAddr) {
    unsigned char m_rawShellCode[] = {
            /* pushad                           */  0x60,
            /* mov dword ptr ds:[0x4DE0480],eax */  0xA3, 0xCC, 0xCC, 0xCC, 0xCC,
            /* mov dword ptr ds:[0x4DE0484],ebx */  0x89, 0x1D, 0xCC, 0xCC, 0xCC, 0xCC,
            /* mov dword ptr ds:[0x4DE0488],ecx */  0x89, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,
            /* mov dword ptr ds:[0x4DE048C],edx */  0x89, 0x15, 0xCC, 0xCC, 0xCC, 0xCC,
            /* mov dword ptr ds:[0x4DE0490],ebp */  0x89, 0x2D, 0xCC, 0xCC, 0xCC, 0xCC,
            /* mov dword ptr ds:[0x4DE0494],esp */  0x89, 0x25, 0xCC, 0xCC, 0xCC, 0xCC,
            /* mov dword ptr ds:[0x4DE0498],esi */  0x89, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* mov dword ptr ds:[0x4DE049C],edi */  0x89, 0x3D, 0xCC, 0xCC, 0xCC, 0xCC,
            /* xor ebx,ebx                      */  0x31, 0xDB,
            /* xor eax,eax                      */  0x31, 0xC0,
            /* push dword ptr ds:[0x4DE0408]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* push dword ptr ds:[0x4DE0404]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* push dword ptr ds:[0x4DE0400]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* call 0x4DE00A9                   */  0xE8, 0x5E, 0x00, 0x00, 0x00,
            /* add eax,dword ptr ds:[0x4DE0440] */  0x03, 0x05, 0xCC, 0xCC, 0xCC, 0xCC,
            /* call eax                         */  0xFF, 0xD0,
            /* mov ebx,eax                      */  0x89, 0xC3,
            /* add esp,0xC                      */  0x83, 0xC4, 0x0C,
            /* xor eax,eax                      */  0x31, 0xC0,
            /* push dword ptr ds:[0x4DE0420]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* push dword ptr ds:[0x4DE041C]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* push dword ptr ds:[0x4DE0418]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* push dword ptr ds:[0x4DE0414]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* push dword ptr ds:[0x4DE0410]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* push dword ptr ds:[0x4DE040C]    */  0xFF, 0x35, 0xCC, 0xCC, 0xCC, 0xCC,
            /* push ebx                         */  0x53,
            /* call 0x4DE00A9                   */  0xE8, 0x25, 0x00, 0x00, 0x00,
            /* add eax,dword ptr ds:[0x4DE0444] */  0x03, 0x05, 0xCC, 0xCC, 0xCC, 0xCC,
            /* call eax                         */  0xFF, 0xD0,
            /* add esp,0x1C                     */  0x83, 0xC4, 0x1C,
            /* mov ebx,eax                      */  0x89, 0xC3,
            /* push 0xFFFFFFFF                  */  0x6A, 0xFF,
            /* push ebx                         */  0x53,
            /* call 0x4DE00A9                   */  0xE8, 0x10, 0x00, 0x00, 0x00,
            /* add eax,dword ptr ds:[0x4DE0448] */  0x03, 0x05, 0xCC, 0xCC, 0xCC, 0xCC,
            /* call eax                         */  0xFF, 0xD0,
            /* add esp,0x8                      */  0x83, 0xC4, 0x08,
            /* jmp 0x4DE0E00                    */  0xE9, 0xCC, 0xCC, 0xCC, 0xCC,
            /* xor eax,eax                      */  0x31, 0xC0,
            /* xor ecx,ecx                      */  0x31, 0xC9,
            /* xor esi,esi                      */  0x31, 0xF6,
            /* xor edi,edi                      */  0x31, 0xFF,
            /* mov eax,dword ptr fs:[0x30]      */  0x64, 0xA1, 0x30, 0x00, 0x00, 0x00,
            /* mov eax,dword ptr ds:[eax+0xC]   */  0x8B, 0x40, 0x0C,
            /* add eax,0x1C                     */  0x83, 0xC0, 0x1C,
            /* push dword ptr ds:[eax+0x4]      */  0xFF, 0x70, 0x04,
            /* mov eax,dword ptr ds:[eax]       */  0x8B, 0x00,
            /* push eax                         */  0x50,
            /* cmp eax,dword ptr ss:[esp+0x4]   */  0x3B, 0x44, 0x24, 0x04,
            /* je 0x4DE00E2                     */  0x74, 0x19,
            /* mov ecx,dword ptr ds:[eax+0x1C]  */  0x8B, 0x48, 0x1C,
            /* and ecx,0xFFFF                   */  0x81, 0xE1, 0xFF, 0xFF, 0x00, 0x00,
            /* mov esi,dword ptr ds:[eax+0x20]  */  0x8B, 0x70, 0x20,
            /* mov edi,0x4DE0424                */  0xBF, 0xCC, 0xCC, 0xCC, 0xCC,
            /* cld                              */  0xFC,
            /* repe cmpsb                       */  0xF3, 0xA6,
            /* jecxz 0x4DE00E2                  */  0xE3, 0x03,
            /* pop eax                          */  0x58,
            /* jmp 0x4DE00C0                    */  0xEB, 0xDE,
            /* add esp,0x8                      */  0x83, 0xC4, 0x08,
            /* mov eax,dword ptr ds:[eax+0x8]   */  0x8B, 0x40, 0x08,
            /* ret                              */  0xC3
    };
    /*
     * 2    1024+128+0x0
     * 8    1024+128+0x4
     * 14   1024+128+0x8
     * 20   1024+128+0xC
     * 26   1024+128+0x10
     * 32   1024+128+0x14
     * 38   1024+128+0x18
     * 44   1024+128+0x1C
     *
     * 54   arg3    1024+0x08    OpenProcess_dwProcessId
     * 60   arg2    1024+0x04    OpenProcess_bInheritHandle
     * 66   arg1    1024+0x00    OpenProcess_dwDesiredAccess
     * 77   arg11   1024+0x3E    RVA_OpenProcess
     *
     * 92   arg9    1024+0x20    CreateRemoteThread_lpThreadId
     * 98   arg8    1024+0x1C    CreateRemoteThread_dwCreationFlags
     * 104  arg7    1024+0x18    CreateRemoteThread_lpParameter
     * 110  arg6    1024+0x14    CreateRemoteThread_lpStartAddress
     * 116  arg5    1024+0x10    CreateRemoteThread_dwStackSize
     * 122  arg4    1024+0x0C    CreateRemoteThread_lpThreadAttributes
     * 134  arg12   1024+0x42    RVA_CreateRemoteThread
     *
     * 155  arg13   1024+0x46    RVA_WaitForSingleObject
     *
     * 165  Jmp to back up Code
     *
     * 214  arg10   1024+0x24    ptr_DllName
     */
    DWORD m_offsetAddr;
    //init 1024+128+0~28
    for (int i = 0; i < 8; ++i) {
        m_offsetAddr = (DWORD)allocMemAddr + 1024 + 128 + (i * 4);
        m_rawShellCode[2 + (i * 6)] = m_offsetAddr & 0xFF;
        m_rawShellCode[2 + (i * 6) + 1] = m_offsetAddr >> 8 & 0xFF;
        m_rawShellCode[2 + (i * 6) + 2] = m_offsetAddr >> 16 & 0xFF;
        m_rawShellCode[2 + (i * 6) + 3] = m_offsetAddr >> 24 & 0xFF;
    }
    //init arg1~arg3
    for (int i = 0; i < 3; ++i) {
        m_offsetAddr = (DWORD)allocMemAddr + 1024 + (i * 4);
        m_rawShellCode[66 - (i * 6)] = m_offsetAddr & 0xFF;
        m_rawShellCode[66 - (i * 6) + 1] = m_offsetAddr >> 8 & 0xFF;
        m_rawShellCode[66 - (i * 6) + 2] = m_offsetAddr >> 16 & 0xFF;
        m_rawShellCode[66 - (i * 6) + 3] = m_offsetAddr >> 24 & 0xFF;
    }
    //init arg4~arg9
    for (int i = 0; i < 6; ++i) {
        m_offsetAddr = (DWORD)allocMemAddr + 1024 + 0xC + (i * 4);
        m_rawShellCode[122 - (i * 6)] = m_offsetAddr & 0xFF;
        m_rawShellCode[122 - (i * 6) + 1] = m_offsetAddr >> 8 & 0xFF;
        m_rawShellCode[122 - (i * 6) + 2] = m_offsetAddr >> 16 & 0xFF;
        m_rawShellCode[122 - (i * 6) + 3] = m_offsetAddr >> 24 & 0xFF;
    }
    //init arg10~arg13
    //arg10
    m_offsetAddr = (DWORD)allocMemAddr + 1024 + 0x24;
    m_rawShellCode[214] = m_offsetAddr & 0xFF;
    m_rawShellCode[214 + 1] = m_offsetAddr >> 8 & 0xFF;
    m_rawShellCode[214 + 2] = m_offsetAddr >> 16 & 0xFF;
    m_rawShellCode[214 + 3] = m_offsetAddr >> 24 & 0xFF;
    //arg11
    m_offsetAddr = (DWORD)allocMemAddr + 1024 + 0x40;
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
    //init jmp offset
    m_offsetAddr = 3415;
    m_rawShellCode[165] = m_offsetAddr & 0xFF;
    m_rawShellCode[165 + 1] = m_offsetAddr >> 8 & 0xFF;
    m_rawShellCode[165 + 2] = m_offsetAddr >> 16 & 0xFF;
    m_rawShellCode[165 + 3] = m_offsetAddr >> 24 & 0xFF;

    memcpy(wCode.shellCode, m_rawShellCode, sizeof(m_rawShellCode));
}
