#include <linux/module.h>

#include <linux/timer.h>

MODULE_LICENSE("GPL");

void my_timer_callback(struct timer_list *t);

static DEFINE_TIMER(my_timer, my_timer_callback);

void my_timer_callback(struct timer_list *t)
{
    printk(KERN_INFO "Timer callback executed\n");
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
}


static int __init _timer_init(void)
{
    printk(KERN_INFO "Timer module init\n");

    my_timer.expires = jiffies + msecs_to_jiffies(2000); // 2 seconds
    add_timer(&my_timer);

    return 0;
}

static void __exit timer_exit(void)
{
    printk(KERN_INFO "Timer module exit\n");
}

module_init(_timer_init);
module_exit(timer_exit);

