#include <stdio.h>
#include <fcntl.h>      // open(), O_RDWR
#include <unistd.h>     // close(), read(), write()
#include <sys/ioctl.h>  // ioctl()

#define CMD_TEST_O _IO('T', 0)
#define CMD_TEST_I _IOW('T', 1, int) 
#define CMD_TEST_R _IOR('T', 2, int)

int main(int argc, char *argv[])
{
    int fd = open("/dev/ioctl_test", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }


    ioctl(fd, CMD_TEST_O); // 调用无参数的 ioctl 命令
    ioctl(fd, CMD_TEST_I, 42); // 调用带参数的 ioctl 命令，传递整数 42
    int value;
    ioctl(fd ,CMD_TEST_R, &value); // 调用读取参数的 ioctl 命令，获取整数值
    printf("Value read from ioctl: %d\n", value);
    
    close(fd);
    return 0;
}
