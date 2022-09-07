#include "include/Cross_Process_Hook.h"
int main(){
    c_PreProcess preProcess;
    st_RVA rva = preProcess.GetRVAs();
    st_Params params(rva);
    c_ShellCodeMaker shellCodeMaker(params);




    return 0;
}