#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/delay.h>

int test_Value = 0;

int test_thread(void *param){

	int i = 0;

	for(i=0;i<100000;i++){
		int *temp = (int*)kmalloc(sizeof(int), GFP_KERNEL);
		test_Value++;
		//__sync_fetch_and_add(&test_Value, 1);
		kfree(temp);
	}

	return 0;
}

void thread_create(void){
	int i;

	for(i=0;i<4;i++){
		
		kthread_run(&test_thread, NULL, "test_thread");
	}
}

int __init simple_thread_module_init(void){

	thread_create();

	mdelay(100);

	printk("test_Value : %d\n", test_Value);

	printk(KERN_EMERG "Simple Thread Module!\n");

	return 0;
}

void __exit simple_thread_module_cleanup(void){

	printk("Bye simple thread module!\n");
}

module_init(simple_thread_module_init);
module_exit(simple_thread_module_cleanup);

MODULE_LICENSE("GPL");
