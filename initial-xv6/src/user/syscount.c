// In user/syscount.c
#include "kernel/types.h"
#include "kernel/stat.h"
// #include "kernel/fcntl.h"
#include "user.h"
#include "kernel/syscall.h"


char *syscall_names[30] = {
    [SYS_fork] "fork",
    [SYS_exit] "exit",
    [SYS_wait] "wait",
    [SYS_pipe] "pipe",
    [SYS_read] "read",
    [SYS_kill] "kill",
    [SYS_exec] "exec",
    [SYS_fstat] "fstat",
    [SYS_chdir] "chdir",
    [SYS_dup] "dup",
    [SYS_getpid] "getpid",
    [SYS_sbrk] "sbrk",
    [SYS_sleep] "sleep",
    [SYS_uptime] "uptime",
    [SYS_open] "open",
    [SYS_write] "write",
    [SYS_mknod] "mknod",
    [SYS_unlink] "unlink",
    [SYS_link] "link",
    [SYS_mkdir] "mkdir",
    [SYS_close] "close",
};

void usage()
{
    printf("Usage: syscount <mask> command [args]\n");
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        usage();
        return -1;
    }

    int mask = atoi(argv[1]);
    if (mask <= 0)
    {
        printf("Invalid mask\n");
        return -1;
    }

    int count = 0;
    if (mask >= 0)
    {
        // // while (mask > 1)
        // // {
        // //     mask >>= 1;
        // //     count++;
        // // }

        // while(1)
        // {
        //     if(mask & 1<<count)
        //     {
        //         break;
        //     }
        // }
        while (mask > 1)
        {
            mask = mask >> 1;
            count++;
        }
        printf("count: - %d\n",count);
        
    }
    else
    {
        printf("Put mask more than 0\n");
    }

    //getting the name of the syscall
    char *syscall_name = "unknown";
    if (count >= 0 && count < 30 && syscall_names[count] != 0)
    syscall_name = syscall_names[count];

    int pid = fork();
    if (pid < 0)
    {
        printf("fork failed\n");
        return -1;
    }

    if (pid == 0)
    {
        // In child process: execute the command
        exec(argv[2], &argv[2]);
        printf("syscount exec failed\n");
        return -1;
    }
    else
    {
        // In parent process: wait for child to finish
        wait(0);
        int times_called = getSysCount(count);
        int fake_pid = getpid();
        
        printf("PID %d called %s %d times.\n", fake_pid, syscall_name, times_called);
        return 0;
    }

    return 0;
}
