cmd_/root/work/drivers/4.misc/misc.mod := printf '%s\n'   misc.o | awk '!x[$$0]++ { print("/root/work/drivers/4.misc/"$$0) }' > /root/work/drivers/4.misc/misc.mod
