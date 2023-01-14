#include "include/Cross_Process_Hook.h"

void remote_cb(void *argAddr){

    printf("call it!");
}
int main(){
    c_ProcessHook p("", e_SendDataMethod::Socket, remote_cb);

    /*
    c_ProcessHook processHook("notepad.exe",e_SendDataMethod::Socket,remote_cb);


    processHook.CtorHook((LPVOID)0x75000000,5);
    processHook.CommitMem();
    WaitForSingleObject(GetCurrentProcess(),INFINITE);
    processHook.DeleteHook((LPVOID)0x75000000);
*/

    return 0;
}