#include <asm.h>
#include <segment.h>


ENTRY (sys_call_table)
	.long sys_ni_syscall//0
	.long sys_exit
	.long sys_fork
	.long sys_ni_syscall
	.long sys_write         // 4
	.long sys_read // 5
	.long sys_sbrk
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_gettime   // 10
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall// 15
	.long sys_ni_syscall	
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_clone
	.long sys_getpid// 20
	.long sys_sem_init
	.long sys_sem_wait
	.long sys_sem_signal
	.long sys_sem_destroy
	.long sys_ni_syscall// 25
	.long sys_ni_syscall	
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall// 30
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_get_stats// 35
	.long sys_ni_syscall	
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall// 40
MAX_SYSCALL = .-sys_call_table
