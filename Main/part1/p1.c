#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

unsigned long **sys_call_table;

/* Part 1 added code ------------------------------------------------------------------------------------------------
 * sccoache
 * jcbaker
 */
asmlinkage long (*ref_sys_open)(const char __user *file, int flags, umode_t mode);
asmlinkage long (*ref_sys_read)(int filename, void *buf, size_t count);
asmlinkage long (*ref_sys_close)(unsigned int filename);

asmlinkage long newSysOpen(const char __user *file, int flags, umode_t mode) {
	int uid = current_uid().val; //Stores UID

    if (uid >= 1000){ //If the UID is in user space
      printk(KERN_INFO "User %d opened the file: %s\n", uid, file); //prints to syslog
    }
    return ref_sys_open(file, flags, mode); //returns the real system call
}

asmlinkage long newSysRead(int filename, void *buf, size_t count) {
	int uid = current_uid().val; //Stores UID

	if(uid >= 1000){
    	if(strstr(buf, "VIRUS")){
    		printk(KERN_INFO "User %d read from file: %d -- VIRUS!!\n", current_uid().val, filename);
    	}
    	else{
      		printk(KERN_INFO "User %d read from file: %d -- No Virus Found\n", current_uid().val, filename);
    	}
  }
  return ref_sys_read(filename, buf, count);
}


asmlinkage long newSysClose(unsigned int filename) {
    int uid = current_uid().val; //Stores UID

    if (uid >= 1000){ //If the UID is in the user space
        printk(KERN_INFO "User %d closed the file: %d\n", uid, filename); //prints to syslog
    }
    return ref_sys_close(filename); //returns the real system call
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
    ref_sys_open = (void *)sys_call_table[__NR_open];
    ref_sys_close = (void *)sys_call_table[__NR_close];
    ref_sys_read = (void *)sys_call_table[__NR_read];

    /* Replace the existing system calls */
    disable_page_protection();

    sys_call_table[__NR_open] = (unsigned long *)newSysOpen; //replace entries in the syscall table
    sys_call_table[__NR_close] = (unsigned long *)newSysClose;
    sys_call_table[__NR_read] = (unsigned long *)newSysRead; //adds newsysread

    enable_page_protection();

    /* And indicate the load was successful */
    printk(KERN_INFO "Loaded interceptor!");

    return 0;
}

static void __exit interceptor_end(void) {
    /* If we don't know what the syscall table is, don't bother. */
    if(!sys_call_table)
        return;

    /* Revert all system calls to what they were before we began. */
    disable_page_protection();
    sys_call_table[__NR_open] = (unsigned long *)ref_sys_open; //return entries in the syscall table
    sys_call_table[__NR_close] = (unsigned long *)ref_sys_close;
    sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;
    enable_page_protection();

    printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);