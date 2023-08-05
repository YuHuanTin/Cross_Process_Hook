
#include <fmt/format.h>
#include <chrono>
#include "include/Cross_Process_Hook.h"
#include "Utils/Utils.h"

int main(){
    setbuf(stdout, nullptr);


    ProcessHook processHook(Utils::RemoteProcess::getProcessID("notepad.exe").value());

    processHook.AddHook(0, 0);


    /**
     *  Hook 构造的时候创建一个函数用于加载远程 dll
     *  Hook 析构的时候释放dll
     *
     */

    /**
     *  写一小段 asmjit 代码建立共享内存（只需要call once）
     */

    /**
     *  建立共享内存后通信（分别实现异步和同步）
     */

//    Environment env;
//    env.setArch(Arch::kX86);
//    CodeHolder code;                  // Create a CodeHolder.
//    code.init(env);                   // Initialize CodeHolder with custom environment.
//    // Generate a 32-bit function that sums 4 floats and looks like:
//    //   void func(float* dst, const float* a, const float* b)
//    code.relocateToBase(0x69857500);
//    x86::Assembler a(&code);          // Create and attach x86::Assembler to `code`.
//    DWORD base = 0x60000000;
//
//    a.
//    a.mov(ptr(base + 4), eax);
//    a.mov(ptr(base + 8), ebx);
//    a.jmp(x86::Mem(0x6985752A));
//    a.jmp(Imm(0x6985752A,0));
//    a.jmp(0x6985752A);
//    a.ret();                          // Return from function.
//    // We have no Runtime this time, it's on us what we do with the code.
//    // CodeHolder stores code in Section, which provides some basic properties
//    // and CodeBuffer structure. We are interested in section's CodeBuffer.
//    //
//    // NOTE: The first section is always '.text', it can be retrieved by
//    // code.sectionById(0) or simply by code.textSection().
//    CodeBuffer& buffer = code.textSection()->buffer();
//    // Print the machine-code generated or do something else with it...
//    //   8B4424048B4C24048B5424040F28010F58010F2900C3
//    for (size_t i = 0; i < buffer.size(); i++)
//        printf("%02X", buffer._data[i]);


//    ProcessHook p("notepad.exe", HookMethodE::Socket, remote_cb);
//
//    p.AddHook(0x75000000,5);
//    p.CommitHook();
//    WaitForSingleObject(GetCurrentProcess(),INFINITE);
//    p.DeleteHook(0x75000000);


    return 0;
}