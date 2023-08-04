# A simple hook that can be used to send data (via Socket/SharedMemory) across processes
## Notes:
- Due to the use of shellcode, it can currently only be injected into x86 applications now

### For Socket Method:
- TCP protocol (sync):

    Using the tcp protocol can cause the data to be overwritten after the callback function is processed, so it may cause the program to freeze

- UDP protocol (async):

    Using the udp protocol allows the data to be overwritten without being processed by the callback function, so it is not safe for reading and writing register-based memory data (but reading registers is safe)

### For Shared Memory Method:
- Process Mutex (sync):

    May cause lag

- Without Mutex (async):

    Data may be overwritten

## Deprecate:

Deprecated due to possible poor performance

~~### For CreateRemoteThread Method:~~
- ~~ShellCode will call 'OpenProcess' get HookProcess's Handle, you have to make sure that the target program has permission (administrator privileges) to open the HookProcess Handle (if administrator privileges)~~
