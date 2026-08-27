cmd_/root/work/drivers/12.ioctl/ioctl.mod := printf '%s\n'   ioctl.o | awk '!x[$$0]++ { print("/root/work/drivers/12.ioctl/"$$0) }' > /root/work/drivers/12.ioctl/ioctl.mod
