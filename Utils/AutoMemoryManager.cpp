
#include "AutoMemoryManager.h"

namespace Utils {
    std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_GetProcessHandle> moveHandleOwner(HANDLE Handle) {
        return std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_GetProcessHandle>(Handle);
    }

    std::unique_ptr<std::remove_pointer_t<HANDLE>, AutoDelete_GetProcessHandle> getProcessHandle(DWORD ProcessID) {
        return moveHandleOwner(OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID));
    }
}