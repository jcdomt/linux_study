cmd_/root/work/drivers/13.irq/irq.mod := printf '%s\n'   irq.o | awk '!x[$$0]++ { print("/root/work/drivers/13.irq/"$$0) }' > /root/work/drivers/13.irq/irq.mod
