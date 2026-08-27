cmd_/root/work/drivers/7.param/param.mod := printf '%s\n'   param.o | awk '!x[$$0]++ { print("/root/work/drivers/7.param/"$$0) }' > /root/work/drivers/7.param/param.mod
