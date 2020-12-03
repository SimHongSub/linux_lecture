#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/time.h>

struct rw_semaphore list_rwse;

struct list_head my_list;

struct my_node{
	struct list_head list;
	int data;
};

long long insert_time[4];
long long search_time = 0;
long long delete_time = 0;

int insert_thread(void *_arg){
	int i;
	int *arg = (int*)_arg;
	ktime_t start, end;
	int startIndex = (*arg) * 25000;
	int endIndex = ((*arg) + 1) * 25000;

	start = ktime_get_ns();
	for(i=startIndex; i<endIndex; i++){
		struct my_node *new = (struct my_node*)kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		down_write(&list_rwse);
		list_add(&new->list, &my_list);
		up_write(&list_rwse);
	}
	end = ktime_get_ns();

	insert_time[*arg] = (long long)ktime_sub(end, start);

	return 0;
}

int search_thread(void *_arg){
	ktime_t start, end;
	struct my_node *current_node;

	start = ktime_get_ns();
	down_read(&list_rwse);
	list_for_each_entry(current_node, &my_list, list){
		if(current_node->data == 50000){
			break;
		}
	}
	up_read(&list_rwse);
	end = ktime_get_ns();

	__sync_fetch_and_add(&search_time, (long long)ktime_sub(end, start));

	return 0;
}

int delete_thread(void *_arg){
	ktime_t start, end;
	struct my_node *current_node;
	struct my_node *tmp;
	int check = 0;

	start = ktime_get_ns();
	down_write(&list_rwse);
	list_for_each_entry_safe(current_node, tmp, &my_list, list){
		if(check > 25000){
			break;
		}
		list_del(&current_node->list);
		kfree(current_node);
		check++;
	}
	up_write(&list_rwse);
	end = ktime_get_ns();

	__sync_fetch_and_add(&delete_time, (long long)ktime_sub(end, start));

	return 0;
}

static int __init my_mod_init(void){
	
	int i;

	printk("%s, Entering semaphore module\n", __func__);

	INIT_LIST_HEAD(&my_list);

	init_rwsem(&list_rwse);

	for(i=0; i<4; i++){
		int *arg = (int*)kmalloc(sizeof(int), GFP_KERNEL);
		*arg = i;
		kthread_run(&insert_thread, (void*)arg, "insert_thread");
	}

	msleep(10000);

	for(i=0; i<4; i++){
		kthread_run(&search_thread, NULL, "search_thread");
	}

	msleep(10000);

	for(i=0; i<4; i++){
		kthread_run(delete_thread, NULL, "delete_thread");
	}

	msleep(10000);

	for(i=0; i<4; i++){
		if(insert_time[0] < insert_time[i]){
			insert_time[0] = insert_time[i];
		}
	}

	printk("insert time : %llu ns\n", insert_time[0]);
	printk("serarch time : %llu ns\n", search_time);
	printk("delete time : %llu ns\n", delete_time);	
	
	return 0;
}

static void __exit my_mod_exit(void){
	
	printk("%s, Exiting semaphore module\n", __func__);
}

module_init(my_mod_init);
module_exit(my_mod_exit);

MODULE_LICENSE("GPL");
