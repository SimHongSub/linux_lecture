#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/time.h>

long long insert_time[2];
long long search_time[2];

int search_time_index = 0;

struct my_node{
	struct list_head entry;
	int data;
	int find_data;
};

int insert_thread(void *data){
	int i;
	struct my_node* temp = (struct my_node*)data;
	struct list_head *list = &(temp->entry);
	ktime_t start, end;
	int startInd = temp->data * temp->find_data;
	int endInd = (temp->data + 1) * temp->find_data;
	int size = 0;

	start = ktime_get_ns();
	for(i=startInd;i<endInd;i++){
		struct my_node *new = (struct my_node*)kmalloc(sizeof(struct my_node), GFP_KERNEL);
		new->data = i;
		list_add(&new->entry, list);
		size++;
	}
	end = ktime_get_ns();

	insert_time[temp->data] = (long long)ktime_sub(end, start);

	temp->data = size;

	//__sync_fetch_and_add(&insert_time, (long long)ktime_sub(end, start));

	return 0;
}

void my_insert(struct my_node **_arg, int index){
	
	int i;
	for(i=0;i<2;i++){

		_arg[i] = (struct my_node*)kmalloc(sizeof(struct my_node), GFP_KERNEL);
		_arg[i]->data = i;
		_arg[i]->find_data = index/2;

		INIT_LIST_HEAD(&(_arg[i]->entry));

		kthread_run(insert_thread, (void*)_arg[i], "insert_thread");
	}
}

int search_thread(void* data){
	int index = search_time_index;
	struct my_node *current_node;
	struct my_node* temp = (struct my_node*)data;
	ktime_t start, end;

	start = ktime_get_ns();
        list_for_each_entry(current_node, &(temp->entry), entry){
		if(current_node->data == temp->find_data){
                	printk("%d\n", current_node->data);
			break;
		}
        }
	end = ktime_get_ns();

	search_time[index] = (long long)ktime_sub(end, start);

	return 0;

}

void my_search(struct my_node **_arg, int data){
	int i;

	for(i=0;i<2;i++){
		_arg[i]->find_data = data;
		kthread_run(search_thread, (void*)_arg[i], "search_thread");
		search_time_index++;
	}
}

void example(void){
	struct my_node* lists[2];
	struct my_node *current_node;

	my_insert(lists, 10000);

	msleep(10000);

	/*list_for_each_entry(current_node, &(lists[0]->entry), entry){
		
		printk("%d\n", current_node->data);
	}*/

	if(insert_time[0] < insert_time[1]){
		printk("insert time : %lld ns\n", insert_time[1]);
	}else{
		printk("insert time : %lld ns\n", insert_time[0]);
	}

	my_search(lists, 6000);

	msleep(10000);

	if(search_time[0] < search_time[1]){
                printk("search time : %lld ns\n", search_time[1]);
        }else{
                printk("search time : %lld ns\n", search_time[0]);
        }

	//printk("size : %d\n", lists[0]->data);

}

int __init my_linked_list_init(void){
	
	printk("my linked list!\n");

	example();

	return 0;
}

void __exit my_linked_list_exit(void){
	printk("Bye\n");

}

module_init(my_linked_list_init);
module_exit(my_linked_list_exit);

MODULE_LICENSE("GPL");
