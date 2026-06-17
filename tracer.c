#include "syscalls.h"
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stddef.h>
char *get_syscall_name(long orig_rax) {
  for (int i = 0; syscall_table[i].number != -1; i++) {
    if (syscall_table[i].number == orig_rax) {
      return syscall_table[i].name;
    }
  }
  return "unknown";
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <program> [args...]\n", argv[0]);
        return 1;
    }
    
    pid_t child;
    int call_count = 0;
    
    child = fork();
    
    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], &argv[1]);  // argv[1] is the program name
    } else {
        int status;
        struct user_regs_struct regs;
        int entering = 1;
        
        while (1) {
            wait(&status);
            
            if (WIFEXITED(status))
                break;
            
            ptrace(PTRACE_GETREGS, child, NULL, &regs);
            
            if (entering) {
                call_count++;
                printf("[%3d] %s(%lld, %lld, %lld, %lld, %lld, %lld)\n",
                    call_count,
                    get_syscall_name(regs.orig_rax),
                    regs.rdi, regs.rsi, regs.rdx, regs.r10, regs.r8, regs.r9);
            }
            
            entering = !entering;
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
        
        printf("\n[*] Total syscalls: %d\n", call_count);
    }
    return 0;
}