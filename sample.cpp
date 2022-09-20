#include "include/Cross_Process_Hook.h"


void remote_cb(st_rData *rData){

    printf("call it!");
}
int main(){
    c_ProcessHook processHook("notepad.exe",remote_cb);


    processHook.CtorHook((LPVOID)0x75000000,e_SendDataMethod::Socket,5);
    processHook.CommitMem();
    WaitForSingleObject(GetCurrentProcess(),INFINITE);
    processHook.DeleteHook((LPVOID)0x75000000);
    return 0;
}