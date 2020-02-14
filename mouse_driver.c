#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#define MOUSE_IRQ 12
static int count;
int mydev;
struct semaphore sem; 
static struct task_struct *thread1;
char thread_name[10]="thread_fun";
//void task_fun();
/* Mouse handler function
 */
static irqreturn_t mouse_intr_handler(int irq , void * dev_id)
{
	count++;
	printk(KERN_INFO "Mouse handler count = %d\n",count);
	up(&sem);
//	return IRQ_HANDLED;
}

int thread_fun(void *data)
{
	printk(KERN_INFO "Iterrupt thread task function\n");
	while(!kthread_should_stop()){
		down(&sem);
		printk(KERN_INFO "Executing interrupt task %d\n",count);
	}
}

int my_init(void)
{
	int status;
	sema_init(&sem, 1);
	printk(KERN_INFO "Mouse driver module loaded\n");
	down(&sem);
	/* Drivers registers an interrupt handler and enable a given interrupt
	 * line for handling with the function request_irq(), which is declared
	 * in <linux/interrupt.h>
	 */
	status = request_irq(MOUSE_IRQ, mouse_intr_handler, IRQF_SHARED,
			"intr_handler", &mydev);
	if (status != 0) {
		printk (KERN_ERR "Handler registration to intr line failed\n");
		return -EIO;
	}
	thread1 = kthread_run(thread_fun, NULL, thread_name);
/*
        if((thread1)) {
                printk(KERN_INFO"in if");
                wake_up_process(thread1);
        }
*/
//	task_fun();
//	 printk(KERN_INFO "Iterrupt task function\n");
/*        while(1){
                down(&sem);
                printk(KERN_INFO "Executing interrupt task %d\n",count);
        }
*/	return 0;
}

void my_exit(void)
{
	printk(KERN_INFO "Mouse driver module unloaded\n");

	free_irq(MOUSE_IRQ, &mydev);
        kthread_stop(thread1);

}

MODULE_LICENSE("Dual BSD/GPL");

module_init(my_init);
module_exit(my_exit);
