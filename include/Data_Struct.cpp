#include "Data_Struct.h"

//void getModuleRVA(std::map<const std::string, DWORD *> &keyValue, const std::string &ModuleName) {
//    auto m_BaseAddr = (DWORD) LoadLibraryA(ModuleName.c_str());
//    //get dos size
//    long m_DosSize = *(long *) (m_BaseAddr + 0x3C);
//    //get optional PE address
//    DWORD m_OptionalPEAddr = m_BaseAddr + m_DosSize + 0x4 + 0x14;
//    //get export table info
//    DWORD m_ExportRVA = *(DWORD *) (m_OptionalPEAddr + 0x60);
//    DWORD m_ExportSize = *(DWORD *) (m_OptionalPEAddr + 0x64);
//    if (m_ExportSize <= 0) {
//        return;
//    }
//    DWORD ExportAddr = m_BaseAddr + m_ExportRVA;
//    //get num of named functions
//    DWORD ExportNumberOfNames = *(DWORD *) (ExportAddr + 0x18);
//    //get function,ordinal,name table address
//    DWORD ExportFunctionsAddr = m_BaseAddr + *(DWORD *) (ExportAddr + 0x1C);
//    DWORD ExportNamesAddr = m_BaseAddr + *(DWORD *) (ExportAddr + 0x20);
//    DWORD ExportNameOrdinalsAddr = m_BaseAddr + *(DWORD *) (ExportAddr + 0x24);
//    int cnt = 0;
//    for (int i = 0; i < ExportNumberOfNames && cnt < keyValue.size(); ++i) {
//        char *strAddr = (char *) (m_BaseAddr + *(DWORD *) (ExportNamesAddr + i * 4));
//        if (keyValue.find(strAddr) != keyValue.end()) {
//            WORD Ordinal = *(WORD *) (ExportNameOrdinalsAddr + i * 2);
//            *keyValue.at(strAddr) = *(DWORD *) (ExportFunctionsAddr + Ordinal * 4);
//            ++cnt;
//        }
//    }
//}

