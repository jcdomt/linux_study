#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int key;
module_param(key, int, S_IRUGO);
MODULE_PARM_DESC(key, "A parameter for the param driver");

static int b[5];
static int count;
module_param_array(b, int, &count, S_IRUGO);
MODULE_PARM_DESC(b, "An array of integers for the param driver");


static int __init param_init(void)
{
    printk("param driver loaded\n");
    printk("key = %d\n", key);
    printk("b[0] = %d\n", b[0]);
    printk("count = %d\n", count);
    return 0;
}

static void __exit param_exit(void)
{
    printk("param driver unloaded\n");
}

module_init(param_init);
module_exit(param_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WZJian");