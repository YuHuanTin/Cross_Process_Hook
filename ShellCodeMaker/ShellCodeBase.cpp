

#include "ShellCodeBase.h"
#include "../Utils/MyException.h"

/**
 * ����Ĵ����� $shellcode + 0xEF9 ƫ�ƴ����� RegisterStorageAddr ��������������д����
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
 * ������ͷβ��� pushad��0x60�� �� popad��0x61��
 *
 * @param CodeData
 * @return
 */
std::vector<std::uint8_t> & ShellCodeBase::insertPushadAndPopad(std::vector<std::uint8_t> &CodeData) {
    // ��� pushad
    CodeData.insert(CodeData.begin(), 0x60);

    // ��� popad
    CodeData.insert(CodeData.end(), 0x61);
    return CodeData;
}

/**
 * һ����ֵ�������쳣������Ҫ�ж��Ƿ���ֵ
 * �� FunPtr ����ָ�뿪ʼ�����Զ������������ӣ�����ȡ��������ʼ��ַ�ʹ�С
 *
 * @param FunPtr
 * @param EndSign
 * @param SignSize
 * @return
 */
std::pair<DWORD, DWORD> ShellCodeBase::getFuncAddressAndSize(DWORD FunPtr, const std::uint8_t *EndSign, std::size_t SignSize) {
    std::size_t functionSize = 0;

    // ��������(debugģʽ) ��Ҫ jmp ������ʵ�ʵ�ַ�ټ���
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
            // �ҵ� ret ���ǲ�Ҫ���� ret
            for (std::size_t i = 0; i < 0x10; ++i) {
                if (*(uint8_t *) (FunPtr + i + functionSize) == 0xC3) {
                    return {FunPtr, functionSize + i};
                }
            }
            // �Ҳ��� ret
            break;
        }
        // �Ҳ�������������ʶ������ 0x1000 ��Ŀ�����������ڴ����ռ�
        if (functionSize > 0x1000) {
            break;
        }
        ++functionSize;
    }
    throw MyException("calc shellcode size", __FUNCTION__);
}

/**
 * һ����ֵ�������쳣������Ҫ�ж��Ƿ���ֵ
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



