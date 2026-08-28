cmd_/root/work/drivers/17.timer/timer.mod := printf '%s\n'   timer.o | awk '!x[$$0]++ { print("/root/work/drivers/17.timer/"$$0) }' > /root/work/drivers/17.timer/timer.mod
