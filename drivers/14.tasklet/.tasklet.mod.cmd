cmd_/root/work/drivers/14.tasklet/tasklet.mod := printf '%s\n'   tasklet.o | awk '!x[$$0]++ { print("/root/work/drivers/14.tasklet/"$$0) }' > /root/work/drivers/14.tasklet/tasklet.mod
