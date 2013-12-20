/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <capsaleres.h>

#include <mm_address.h>

#include <errno.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}


int ret_from_fork() {
  return 0;
}

int sys_fork()
{
  int error = 0;
  int new_frames[NUM_PAG_DATA];
  int pag;

  if (list_empty(&freequeue)) return -1;  
  for (pag=0;pag<NUM_PAG_DATA;pag++){
    new_frames[pag] = alloc_frame();
    if (new_frames[pag] < 0) {
      for (error = pag-1; error >= 0; --error) free_frame(new_frames[error]);
      return new_frames[pag]; //Mirar quin error es    
    }
  }

  struct list_head * lh = list_first(&freequeue);
	struct task_struct * tsk = list_head_to_task_struct(lh);
  
  list_del(&tsk->list);

  struct task_struct * tskc = current();
  int PID = nextFreePID++;
  copy_data(tskc, tsk, KERNEL_STACK_SIZE);
  tsk->PID = PID;

  int retalloc = allocate_DIR(tsk);
  if (retalloc < 0) return retalloc;
  
  // CREAR COPIA KERNEL?????
  for (pag=0;pag<NUM_PAG_KERNEL + NUM_PAG_CODE;pag++){
    int frame = get_frame(tskc->dir_pages_baseAddr, pag);
    set_ss_pag(get_PT(tsk), pag, frame);
    
  }
  
  int end = NUM_PAG_KERNEL + NUM_PAG_CODE + NUM_PAG_DATA + 1;
  for (pag=0;pag<NUM_PAG_DATA;pag++){
    set_ss_pag(get_PT(tsk), PAG_LOG_INIT_DATA_P0 + pag, new_frames[pag]);
    set_ss_pag(get_PT(tskc), end + pag, new_frames[pag]);
    copy_data((void*)((NUM_PAG_KERNEL + NUM_PAG_CODE + pag)*PAGE_SIZE), (void*)((end + pag)*PAGE_SIZE), PAGE_SIZE);
    del_ss_pag(tskc->dir_pages_baseAddr, end + pag);
  }

  union task_union * tsku = (union task_union *)tsk;
  tsku->stack[KERNEL_STACK_SIZE-19] = &ret_from_fork;
  tsku->stack[KERNEL_STACK_SIZE-20] = 0;
  tsk->pointer = &tsku->stack[KERNEL_STACK_SIZE-20];

  list_add_tail(&tsk->list, &readyqueue);
  return PID;
}


void sys_exit()
{  
  struct task_struct * tsk = current();
 /* int pag;
  for (pag = 0; pag < NUM_PAG_DATA; ++pag) {
    free_frame(get_frame(tsk->dir_pages_baseAddr, NUM_PAG_KERNEL + NUM_PAG_CODE + pag));
  }*/
  free_user_pages(&tsk);
  struct list_head * lh = &tsk->list;
  list_add_tail(lh, &freequeue);
  

}


int sys_write(int fd, char * buffer, int size) {
/*    fd: file descriptor. In this delivery it must always be 1.
      buffer: pointer to the bytes.
      size: number of bytes.
      return ’ Negative number in case of error (specifying the kind of error) and
      the number of bytes written if OK.*/
      // Checks the parametres
      int size_original = size;
      int check = check_fd(fd, ESCRIPTURA);
      if(check != 0) return check;
      if (buffer == NULL) return -EFAULT;
      if (size < 0) return -EINVAL;
      
      char buff[4];
      int num = 0;
      while(size >= 4) {
      	check = copy_from_user(buffer, buff, 4);
      	num += sys_write_console(buff, 4);
	buffer += 4;
	size -= 4;
      }
      check = copy_from_user(buffer, buff, size);
      num += sys_write_console(buff, size);
      if (num != size_original) return -ENODEV;
      else return num;
}


int sys_gettime() {
      return zeos_ticks;
}
