

#include "ShellCodeBase.h"
#include "../Utils/MyException.h"

/**
 * 保存寄存器到 $shellcode + 0xEF9 偏移处，即 RegisterStorageAddr 处，但是这里是写死的
 *
 * @param CodeData
 * @return
 */
std::vector<std::uint8_t> & ShellCodeBase::insertRegisterSaveCode(std::vector<std::uint8_t> &CodeData) {
    CodeData.insert(CodeData.begin(),
                    {0x60, 0xe8, 0x51, 0x00, 0x00, 0x00, 0x50, 0x53, 0x05, 0xf9, 0x0e, 0x00, 0x00, 0x8b, 0x5c, 0x24, 0x28, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b,
                     0x5c, 0x24, 0x2c, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x30, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x34, 0x89, 0x18,
                     0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x38, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x3c, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c,
                     0x24, 0x40, 0x89, 0x18, 0x83, 0xc0, 0x04, 0x8b, 0x5c, 0x24, 0x44, 0x89, 0x18, 0x5b, 0x58, 0x61, 0xeb, 0x04, 0x8b, 0x04, 0x24, 0xc3});
    return CodeData;
}

/**
 * 在数据头尾添加 pushad（0x60） 和 popad（0x61）
 *
 * @param CodeData
 * @return
 */
std::vector<std::uint8_t> & ShellCodeBase::insertPushadAndPopad(std::vector<std::uint8_t> &CodeData) {
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
std::pair<DWORD, DWORD> ShellCodeBase::getFuncAddressAndSize(DWORD FunPtr, const std::uint8_t *EndSign, std::size_t SignSize) {
    std::size_t functionSize = 0;

    // 增量链接(debug模式) 需要 jmp 到函数实际地址再计算
    if (*(uint8_t *) (FunPtr) == 0xE9) {
        DWORD offset = *(DWORD *) (FunPtr + 1) + 5;
        FunPtr = (DWORD) (FunPtr + offset);
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
DWORD ShellCodeBase::patchAddress(std::uint8_t *DataPtr, std::size_t Begin, std::size_t End, DWORD Sign, DWORD PatchValue) {
    for (std::size_t i = Begin; i < End; ++i) {
        DWORD &value = *(DWORD *) (DataPtr + i);
        if (value == Sign) {
            value = PatchValue;
            return i;
        }
    }
    throw MyException("can not find sign", __FUNCTION__);
}



