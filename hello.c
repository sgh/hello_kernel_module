#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

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


/*
 * The sequence iterator functions.  We simply use the count of the
 * next line as our internal position.
 */
static void *ct_seq_start(struct seq_file *s, loff_t *pos)
{
	if ((*pos) > 10)
		return NULL;
	return pos;
}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;
	return ct_seq_start(s, pos);
}

static void ct_seq_stop(struct seq_file *s, void *v)
{
}

/*
 * The show function.
 */
static int ct_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *) v;
	seq_printf(s, "%Ld\n", *spos);
	return 0;
}

/*
 * Tie them all together into a set of seq_operations.
 */
static struct seq_operations ct_seq_ops = {
	.start = ct_seq_start,
	.next  = ct_seq_next,
	.stop  = ct_seq_stop,
	.show  = ct_seq_show
};


/*
 * Time to set up the file operations for our /proc file.  In this case,
 * all we need is an open function which sets up the sequence ops.
 */

static int ct_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &ct_seq_ops);
};

/*
 * The file operations structure contains our open function along with
 * set of the canned seq_ ops.
 */
static struct file_operations ct_file_ops = {
	.owner   = THIS_MODULE,
	.open    = ct_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};


static int hello_init(void) {
// 	printk_time = 0;
	struct proc_dir_entry *entry;
// 	ts=kthread_run(thread,NULL,"hello_kthread");


	entry = create_proc_entry("psmux_stats", 0, NULL);
	if (entry)
		entry->proc_fops = &ct_file_ops;


	printk(KERN_INFO "Hello, world\n");
	return 0;
}

static void hello_exit(void) {
// 	send_sig_info(SIGKILL, (struct siginfo*)1, ts);
// 	kthread_stop(ts);
	remove_proc_entry("psmux_stats", NULL);
	printk(KERN_INFO "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
