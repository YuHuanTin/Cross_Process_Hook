# A simple hook that can be used to send data (via CreateRemoteThread) across processes
## Notes:
- Due to the use of shellcode, it can currently only be injected into x86 applications now
- ShellCode will call 'OpenProcess' get HookProcess's Handle, you have to make sure that the target program has permission (administrator privileges) to open the HookProcess Handle (if administrator privileges)