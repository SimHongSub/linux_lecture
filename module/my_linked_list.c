#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/time.h>

long long insert_time;

struct my_node{
	struct list_head entry;
	int data;
};

int insert_thread(void *data){
	int i;
	struct list_head *list = (struct list_head*)data;
	ktime_t start, end;

	start = ktime_get_ns();
	for(i=0;i<5000;i++){
		struct my_node *new = (struct my_node*)kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		list_add(&new->entry, list);
	}
	end = ktime_get_ns();

	__sync_fetch_and_add(&insert_time, (long long)ktime_sub(end, start));

	return 0;
}

void my_insert(struct list_head *_arg){
	
	struct list_head *list1 = (struct list_head*)kmalloc(sizeof(struct list_head), GFP_KERNEL);
	struct list_head *list2 = (struct list_head*)kmalloc(sizeof(struct list_head), GFP_KERNEL);

	INIT_LIST_HEAD(list1);
	INIT_LIST_HEAD(list2);

	_arg[0] = *list1;
	_arg[1] = *list2;

	kthread_run(insert_thread, (void*)list1, "insert_thread");

	kthread_run(insert_thread, (void*)list2, "insert_thread");

}

void example(void){
	
	struct list_head lists[2];
	struct my_node *current_node;

	my_insert(lists);

	msleep(20000);

	/*list_for_each_entry(current_node, &lists[0], entry){
		printk("%d\n", current_node->data);
	}*/

	printk("insert time : %lld ns\n", insert_time);

}

int __init my_linked_list_init(void){
	example();
	printk("my linked list!\n");

	return 0;
}

void __exit my_linked_list_exit(void){
	printk("Bye\n");

}

module_init(my_linked_list_init);
module_exit(my_linked_list_exit);

MODULE_LICENSE("GPL");
