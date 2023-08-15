

#include "ShellCodeBase.h"
#include "../../Utils/MyException.h"

/**
 * 保存寄存器到 $shellcode + 0xEF9 偏移处，即 RegisterStorageAddr 处，但是这里是写死的
 *
 * @param CodeData
 * @return
 */
std::vector<std::uint8_t> &ShellCodeBase::insertRegisterSaveCode(std::vector<std::uint8_t> &CodeData) {
    /**
        046D0001 | 60                       | pushad
        046D0002 | E8 5D000000              | call 0x46D0064
        046D0007 | 50                       | push eax
        046D0008 | 53                       | push ebx
        046D0009 | 05 F90E0000              | add eax,0xEF9
        046D000E | 8B5C24 28                | mov ebx,dword ptr ss:[esp+0x28]
        046D0012 | 8918                     | mov dword ptr ds:[eax],ebx
        046D0014 | 83C0 04                  | add eax,0x4
        046D0017 | 8B5C24 2C                | mov ebx,dword ptr ss:[esp+0x2C]
        046D001B | 8918                     | mov dword ptr ds:[eax],ebx
        046D001D | 83C0 04                  | add eax,0x4
        046D0020 | 8B5C24 30                | mov ebx,dword ptr ss:[esp+0x30]
        046D0024 | 8918                     | mov dword ptr ds:[eax],ebx
        046D0026 | 83C0 04                  | add eax,0x4
        046D0029 | 8B5C24 34                | mov ebx,dword ptr ss:[esp+0x34]
        046D002D | 8918                     | mov dword ptr ds:[eax],ebx
        046D002F | 83C0 04                  | add eax,0x4
        046D0032 | 8B5C24 38                | mov ebx,dword ptr ss:[esp+0x38]
        046D0036 | 8918                     | mov dword ptr ds:[eax],ebx
        046D0038 | 83C0 04                  | add eax,0x4
        046D003B | 8B5C24 3C                | mov ebx,dword ptr ss:[esp+0x3C]
        046D003F | 8918                     | mov dword ptr ds:[eax],ebx
        046D0041 | 83C0 04                  | add eax,0x4
        046D0044 | 8B5C24 40                | mov ebx,dword ptr ss:[esp+0x40]
        046D0048 | 8918                     | mov dword ptr ds:[eax],ebx
        046D004A | 83C0 04                  | add eax,0x4
        046D004D | 8B5C24 44                | mov ebx,dword ptr ss:[esp+0x44]
        046D0051 | 8918                     | mov dword ptr ds:[eax],ebx
        046D0053 | 83C0 04                  | add eax,0x4
        046D0056 | 8B5C24 04                | mov ebx,dword ptr ss:[esp+0x4]
        046D005A | 83EB 07                  | sub ebx,0x7
        046D005D | 8918                     | mov dword ptr ds:[eax],ebx
        046D005F | 5B                       | pop ebx
        046D0060 | 58                       | pop eax
        046D0061 | 61                       | popad
        046D0062 | EB 04                    | jmp 0x46D0068
        046D0064 | 8B0424                   | mov eax,dword ptr ss:[esp]
        046D0067 | C3                       | ret
     */
    CodeData.insert(CodeData.begin(),
                    {0x60, 0xe8, 0x5d, 0x00, 0x00, 0x00, 0x50, 0x53, 0x05, 0xf9, 0x0e, 0x00, 0x00, 0x8b, 0x5c, 0x24, 0x28, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b,
                     0x5c, 0x24, 0x2c, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x30, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x34, 0x89, 0x18,
                     0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x38, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x3c, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c,
                     0x24, 0x40, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x44, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x04, 0x83, 0xeb, 0x07,
                     0x89, 0x18, 0x5b, 0x58, 0x61, 0xeb, 0x04, 0x8b, 0x04, 0x24, 0xc3});
    return CodeData;
}

/**
 * 在数据头尾添加 pushad（0x60） 和 popad（0x61）
 *
 * @param CodeData
 * @return
 */
std::vector<std::uint8_t> &ShellCodeBase::insertPushadAndPopad(std::vector<std::uint8_t> &CodeData) {
    // 添加 pushad
    CodeData.insert(CodeData.begin(), 0x60);

    // 添加 popad
    CodeData.insert(CodeData.end(), 0x61);
    return CodeData;
}

/**
 * 一定有值，否则异常，不需要判断是否有值
 * 以 FunPtr 函数指针开始（会自动处理增量链接），获取函数的起始地址和大小
 *
 * @param FunPtr
 * @param EndSign
 * @param SignSize
 * @return
 */
std::pair<std::size_t, std::size_t> ShellCodeBase::getFuncAddrAndSize(std::size_t FunPtr, const std::uint8_t *EndSign, std::size_t SignSize) {
    std::size_t functionSize = 0;

    // 增量链接(debug模式) 需要 jmp 到函数实际地址再计算
    if (*(uint8_t *) (FunPtr) == 0xE9) {
        std::size_t offset = *(std::size_t *) (FunPtr + 1) + 5;
        FunPtr = (std::size_t) (FunPtr + offset);
    }
    while (true) {
        std::size_t sameSignSize = 0;

        for (std::size_t i = 0; i < SignSize; ++i) {
            if (*(uint8_t *) (FunPtr + i + functionSize) == EndSign[i]) {
                ++sameSignSize;
            }
        }
        if (sameSignSize == SignSize) {
            // 找到 ret 但是不要加入 ret
            for (std::size_t i = 0; i < 0x10; ++i) {
                if (*(uint8_t *) (FunPtr + i + functionSize) == 0xC3) {
                    return {FunPtr, functionSize + i};
                }
            }
            // 找不到 ret
            break;
        }
        // 找不到函数结束标识，并且 0x1000 是目标进程申请的内存最大空间
        if (functionSize > 0x1000) {
            break;
        }
        ++functionSize;
    }
    throw MyException("calc shellcode size", __FUNCTION__);
}

/**
 * 一定有值，否则异常，不需要判断是否有值
 *
 * @param DataPtr
 * @param Begin
 * @param End
 * @param Sign
 * @param PatchValue
 * @return
 */
std::size_t ShellCodeBase::patchAddress(std::uint8_t *DataPtr, std::size_t Begin, std::size_t End, std::size_t Sign, std::size_t PatchValue) {
    for (std::size_t i = Begin; i < End; ++i) {
        std::size_t *value = (std::size_t *) (DataPtr + i);
        if (*value == Sign) {
            *value = PatchValue;
            return i;
        }
    }
    throw MyException("can not find sign", __FUNCTION__);
}



