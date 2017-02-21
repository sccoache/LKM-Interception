#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <asm-generic/current.h>
#include <linux/time.h>
#include <linux/cred.h>
#include <linux/list.h>
#include <linux/types.h>
#include "ancestrystruct.h"
#include <asm/uaccess.h>

unsigned long **sys_call_table;

asmlinkage long (*ref_sys_cs3013_syscall2)(void);
//asmlimkage long ref_cs3013_syscall2(unsigned short *target pid, struct ancestry *response);

/* Part 2 added code ------------------------------------------------------------------------------------------------
 * sccoache
 * jcbaker
 */
asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_pid, struct ancestry *response){
    struct task_struct *task_info;// = current; //temp task information structure
    struct task_struct *task;
    struct list_head *list;
    struct ancestry temp;
    int i = 0;
    int j = 0;
    int k = 0;

    task_info = pid_task(find_vpid(*target_pid), PIDTYPE_PID);

    if(copy_from_user(&temp, response, sizeof temp)){
      return EFAULT;
    }
    
    //prints the info about the target process
    //struct task_struct *curr_task = current;
    pid_t tpid = task_pid_nr(task_info);
    printk("Target process id: %i\n", (int)tpid); 
    temp.target[0] = tpid;

    //finds all the children
    list_for_each(list, &task_info->children){
      task = list_entry(list, struct task_struct, sibling);
      printk("Child process id: %i\n", task->pid);
      temp.children[i] = task->pid;
      i++;
    }

    //finds all ancestors
    while(task->pid > 1){
        task = task->real_parent;
        if (task->pid != tpid){
          printk(KERN_INFO "Ancestor process id: %d\n", task->pid);
          temp.ancestors[j] = task->pid;
          j++;
        }
    }

    //finds all siblings
    list_for_each(list, &task_info->real_parent->children){
      task = list_entry(list, struct task_struct, sibling);
      if (task->pid != tpid){
          printk("Sibling process id: %i\n", task->pid);
          temp.siblings[k] = task->pid;
          k++;
      }
    }

    if(copy_to_user(response, &temp, sizeof temp)){
      return EFAULT;
    }
    return 1;
}

/* Code supplied by assignment ---------------------------------------------------------------------------------------
 * Part 1
 */

static unsigned long **find_sys_call_table(void) {
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;

    while (offset < ULLONG_MAX) {
        sct = (unsigned long **)offset;

        if (sct[__NR_close] == (unsigned long *) sys_close) {
            printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                    (unsigned long) sct);
            return sct;
        }

        offset += sizeof(void *);
    }

    return NULL;
}

static void disable_page_protection(void) {
    /*
       Control Register 0 (cr0) governs how the CPU operates.
       Bit #16, if set, prevents the CPU from writing to memory marked as
       read only. Well, our system call table meets that description.
       But, we can simply turn off this bit in cr0 to allow us to make
       changes. We read in the current value of the register (32 or 64
       bits wide), and AND that with a value where all bits are 0 except
       the 16th bit (using a negation operation), causing the write_cr0
       value to have the 16th bit cleared (with all other bits staying
       the same. We will thus be able to write to the protected memory.
       It's good to be the kernel!
       */
    write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
    /*
       See the above description for cr0. Here, we use an OR to set the 
       16th bit to re-enable write protection on the CPU.
       */
    write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
    /* Find the system call table */
    if(!(sys_call_table = find_sys_call_table())) {
        /* Well, that didn't work. 
           Cancel the module loading step. */
        return -1;
    }

    /* Store a copy of all the existing functions */
    ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];

    /* Replace the existing system calls */
    disable_page_protection();

    sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;

    enable_page_protection();

    /* And indicate the load was successful */
    printk(KERN_INFO "Loaded interceptor!\n");

    return 0;
}

static void __exit interceptor_end(void) {
    /* If we don't know what the syscall table is, don't bother. */
    if(!sys_call_table)
        return;

    /* Revert all system calls to what they were before we began. */
    disable_page_protection();
    
    sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
    
    enable_page_protection();

    printk(KERN_INFO "Unloaded interceptor!\n");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);