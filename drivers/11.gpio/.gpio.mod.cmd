cmd_/root/work/drivers/11.gpio/gpio.mod := printf '%s\n'   gpio.o | awk '!x[$$0]++ { print("/root/work/drivers/11.gpio/"$$0) }' > /root/work/drivers/11.gpio/gpio.mod
