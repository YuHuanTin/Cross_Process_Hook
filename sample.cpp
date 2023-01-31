#include "include/Cross_Process_Hook.h"

void remote_cb(void *argAddr){

    printf("call it!");
}
int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    ProcessHook p("notepad.exe", HookMethodE::Socket, remote_cb);

    p.CtorHook(0x75000000,5);
    p.CommitMem();
    WaitForSingleObject(GetCurrentProcess(),INFINITE);
    p.DeleteHook(0x75000000);


    return 0;
}