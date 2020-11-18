#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/time.h>

#define FALSE 0
#define TRUE 1

struct my_node{
	struct rb_node node;
	int value;
};

void RB_example(void){
        struct rb_root root_node = RB_ROOT;
	struct my_node *new_node;
	struct rb_node **new;
	struct rb_node *parent;
	struct my_node *this;
	struct rb_node *current_node;

        int i;
	int j;
	int result;
	int check = 0;
        long long running_time[3];
        ktime_t start, end;

	for(i=1000;i<=100000;i=i*10){
        	/* Insert */
        	start = ktime_get();
        	for(j=0;j<i;j++){
			check = 0;
                	new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
                	new_node->value = j;

			new = &(root_node.rb_node);
			parent = NULL;

			while(*new){
				this = rb_entry(*new, struct my_node, node);
				result = (new_node->value - this->value);
				parent = *new;
				if(result < 0) new = &((*new)->rb_left);
				else if(result > 0) new = &((*new)->rb_right);
				else{
					check = 1;
				       	break;
				}
			}

			if(check == 0){
				rb_link_node(&new_node->node, parent, new);
                		rb_insert_color(&new_node->node, &root_node);
			}
        	}
        	end = ktime_get();
        	running_time[0] = (long long)ktime_sub(end, start);
	
        	/* Search */
		start = ktime_get();
		current_node = root_node.rb_node;
		while(current_node){
			this = rb_entry(current_node, struct my_node, node);
			result = (i/2 - this->value);
			if(result<0) current_node = current_node->rb_left;
			else if(result>0) current_node = current_node->rb_right;
			else{
				printk("value %d search success!\n", i/2);
				break;
			}
		}		
		end = ktime_get();
		running_time[1] = (long long)ktime_sub(end, start);

        	/* Delete */
		start = ktime_get();
		new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        	rb_erase(&new_node->node, &root_node);
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
	RB_example();
	printk("Red-Black Tree test module init!\n");
	return 0;
}

void __exit my_simple_module_cleanup(void)
{
	printk("Bye Red-Black Tree test module!\n");
}


module_init(my_simple_module_init);
module_exit(my_simple_module_cleanup);
MODULE_LICENSE("GPL");
