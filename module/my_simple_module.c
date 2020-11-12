#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/time.h>

struct my_node{
	struct list_head list;
	int data;
};

void struct_example(void){
        struct list_head my_list;
	struct my_node *current_node;
        struct my_node *tmp;

        int i;
	int j;
        long long running_time[3];
        ktime_t start, end;

        INIT_LIST_HEAD(&my_list);

	for(i=1000;i<=100000;i=i*10){
        	/* Insert */
        	start = ktime_get();
        	for(j=0;j<i;j++){
                	struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
                	new->data = j;
                	list_add(&new->list, &my_list);
        	}
        	end = ktime_get();
        	running_time[0] = (long long)ktime_sub(end, start);
	
        	/* Search */
		start = ktime_get();
        	list_for_each_entry(current_node, &my_list, list){
			if(current_node->data == i/2){
				break;
			}
        	}
		end = ktime_get();
		running_time[1] = (long long)ktime_sub(end, start);

        	/* Delete */
		start = ktime_get();
        	list_for_each_entry_safe(current_node, tmp, &my_list, list){
                	if(current_node->data == 2){
                        	list_del(&current_node->list);
                        	kfree(current_node);
        		}
		}
		end = ktime_get();
                running_time[2] = (long long)ktime_sub(end, start);

                printk("Count : %d\n", i);
                printk("Insert running-time : %llu\n", running_time[0]);
                printk("Search running-time : %llu\n", running_time[1]);
                printk("Delete running-time : %llu\n", running_time[2]);

	}
}

int __init my_simple_module_init(void)
{
	struct_example();
	printk("Linked list test module init!\n");
	return 0;
}

void __exit my_simple_module_cleanup(void)
{
	printk("Bye linked list test module!\n");
}


module_init(my_simple_module_init);
module_exit(my_simple_module_cleanup);
MODULE_LICENSE("GPL");
