#include <linux/kernel.h>

asmlinkage long sys_mycall(void){
        printk("System Call Example : hongkernel 20161850\n");

	return 0;
}
