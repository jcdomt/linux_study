cmd_/root/work/drivers/10.dts/dts.mod := printf '%s\n'   dts.o | awk '!x[$$0]++ { print("/root/work/drivers/10.dts/"$$0) }' > /root/work/drivers/10.dts/dts.mod
