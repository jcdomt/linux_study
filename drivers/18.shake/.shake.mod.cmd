cmd_/root/work/drivers/18.shake/shake.mod := printf '%s\n'   shake.o | awk '!x[$$0]++ { print("/root/work/drivers/18.shake/"$$0) }' > /root/work/drivers/18.shake/shake.mod
