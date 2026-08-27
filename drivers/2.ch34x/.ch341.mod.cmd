cmd_/home/wzj/work/drivers/ch34x/ch341.mod := printf '%s\n'   ch341.o | awk '!x[$$0]++ { print("/home/wzj/work/drivers/ch34x/"$$0) }' > /home/wzj/work/drivers/ch34x/ch341.mod
