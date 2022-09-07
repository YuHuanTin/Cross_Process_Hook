#ifndef CROSS_PROCESS_HOOK_PREPROCESS_H
#define CROSS_PROCESS_HOOK_PREPROCESS_H
#include <map>
#include <string>
#include "Data_Struct.h"

class c_PreProcess{
private:
    st_RVA rva;
    st_RVA GetFunctionsRVA(std::map<std::string,DWORD *> &key_value);
public:
    st_RVA GetRVAs(){
        std::map<std::string,DWORD *> p_KeyValue{
                {"OpenProcess",&rva.OpenProcess},
                {"CreateRemoteThread",&rva.CreateRemoteThread},
                {"WaitForSingleObject",&rva.WaitForSingleObject}
        };
        return GetFunctionsRVA(p_KeyValue);
    }
};

#endif //CROSS_PROCESS_HOOK_PREPROCESS_H
