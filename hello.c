#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/signal.h>

MODULE_LICENSE("Dual BSD/GPL");

// static int printk_time = 0;
// module_param_named(time, printk_time, int, S_IRUGO);
struct task_struct *ts;

int thread(void *data) {
	allow_signal(SIGKILL);
	while(1) {
		printk("Hi I am kernel thread! Going to sleep\n");
		set_current_state(TASK_INTERRUPTIBLE);
    schedule(); 

		 if (signal_pending(current)) {
			 printk("Received signal. Break!");
			 break;
		 }

		 if (kthread_should_stop()) {
			printk("I should stop\n");
			break;
		}
	}
	return 0;
}

static int hello_init(void) {
// 	printk_time = 0;
	ts=kthread_run(thread,NULL,"hello_kthread");
	printk(KERN_INFO "Hello, world\n");
	return 0;
}

static void hello_exit(void) {
	send_sig_info(SIGKILL, (struct siginfo*)1, ts);
	kthread_stop(ts);
	printk(KERN_INFO "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
