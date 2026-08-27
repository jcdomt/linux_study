#include <linux/module.h>
#include <linux/platform_device.h>

void beep_release(struct device *dev)
{
    printk("beep device released\n");
}

struct resource beep_res[] = {
    [0] = {
        .start = 0x20AC000,
        .end   = 0x20AC000 + 0x1000 - 1,
        .flags = IORESOURCE_MEM,
    },
};
struct platform_device beep_device = {
    .name = "beep",
    .id = -1,
    .num_resources = ARRAY_SIZE(beep_res),
    .resource = beep_res, 

    .dev = {
        .release = beep_release,
    }
};

static int __init beep_device_init(void)
{
    return platform_device_register(&beep_device);
}

static void __exit beep_device_exit(void) {
    platform_device_unregister(&beep_device);
}

module_init(beep_device_init);
module_exit(beep_device_exit);

MODULE_LICENSE("GPL");