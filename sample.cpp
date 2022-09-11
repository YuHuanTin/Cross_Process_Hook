#include "include/Cross_Process_Hook.h"

void remote_cb(void *argAddr){

    printf("call it!");
}
int main(){

    c_ProcessHook processHook("notepad.exe");
    st_wParams params;
    params.CreateRemoteThread_lpStartAddress = (DWORD)&remote_cb;

    processHook.CtorHook((LPVOID)0x75000000,5,params);
    processHook.CommitMem();

    WaitForSingleObject(GetCurrentProcess(),INFINITE);

    processHook.DeleteHook((LPVOID)0x75000000);


    return 0;
}