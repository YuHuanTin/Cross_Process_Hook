#ifndef CROSS_PROCESS_HOOK_PREPROCESS_H
#define CROSS_PROCESS_HOOK_PREPROCESS_H

#include <map>
#include <string>
#include "Data_Struct.h"

class c_PreProcess{
private:
    st_RVA rva;
    st_RVA GetFunctionsRVA(std::map<std::string,DWORD *> &key_value){
        auto m_BaseAddr = (DWORD)LoadLibraryA("kernel32.dll");
        //get dos size
        long m_DosSize = *(long *)(m_BaseAddr + 0x3C);
        //get optional PE address
        DWORD m_OptionalPEAddr = m_BaseAddr + m_DosSize + 0x4 + 0x14;
        //get export table info
        DWORD m_ExportRVA = *(DWORD *)(m_OptionalPEAddr + 0x60);
        DWORD m_ExportSize = *(DWORD *)(m_OptionalPEAddr + 0x64);
        if(m_ExportSize == 0){
            return rva;
        }
        DWORD ExportAddr = m_BaseAddr + m_ExportRVA;
        //get num of named functions
        DWORD ExportNumberOfNames = *(DWORD *)(ExportAddr + 0x18);
        //get function,ordinal,name table address
        DWORD ExportFunctionsAddr = m_BaseAddr + *(DWORD *)(ExportAddr + 0x1C);
        DWORD ExportNamesAddr = m_BaseAddr + *(DWORD *)(ExportAddr + 0x20);
        DWORD ExportNameOrdinalsAddr = m_BaseAddr + *(DWORD *)(ExportAddr + 0x24);
        int cnt = 0;
        for (int i = 0 ;i < ExportNumberOfNames && cnt < key_value.size(); ++i){
            char *strAddr = (char *)(m_BaseAddr + *(DWORD *)(ExportNamesAddr + i * 4));
            if (key_value.find(strAddr) != key_value.end()){
                WORD Ordinal = *(WORD *)(ExportNameOrdinalsAddr + i * 2);
                *key_value.at(strAddr) = *(DWORD *)(ExportFunctionsAddr + Ordinal * 4);
                ++cnt;
            }
        }
        return rva;
    };
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
