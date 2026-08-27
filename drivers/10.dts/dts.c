#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>

MODULE_LICENSE("GPL");

/*
beep:test {
	compatible = "test";
	#address-cells = <1>;
	#size-cells = <1>;
	reg = <0x20ac000 0x0000004>;
	
	status = "disable";
}

&beep {
	status = "okay";
}
*/

const struct of_device_id beep_of_match[] = {
    {.compatible = "test"},
    {}
};

unsigned long *beep_base_addr;

static int beep_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "Beep device probed\n");

    // 获取设备树节点的资源
    // 匹配出来的资源存在于 pdev->dev.of_node 中
    printk(KERN_INFO "Device name: %s\n", pdev->dev.of_node->name);

    // 主流方法：通过查看设备树节点来实现功能
    struct device_node *test_device_node = pdev->dev.of_node;
    // 获取 reg 属性
    u32 reg_value[2];
    int ret = of_property_read_u32_array(test_device_node, "reg", reg_value, 2);
    if (ret) {
        printk(KERN_ERR "Failed to read reg property\n");
        return ret;
    }
    printk(KERN_INFO "Reg values: 0x%x, 0x%x\n", reg_value[0], reg_value[1]);

    // 映射 reg 的内存
    beep_base_addr = of_iomap(test_device_node, 0);
    if (!beep_base_addr) {
        printk(KERN_ERR "Failed to map reg memory\n");
        return -ENOMEM;
    }
    
    // 注册杂项设备等用于操作地址
    // ...

    return 0;
}

struct platform_driver beep_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "beep",
        .of_match_table = beep_of_match,
    },
    .probe = beep_probe,
};

module_platform_driver(beep_driver);