cmd_/home/wzj/work/drivers/hello/hello.mod := printf '%s\n'   hello.o | awk '!x[$$0]++ { print("/home/wzj/work/drivers/hello/"$$0) }' > /home/wzj/work/drivers/hello/hello.mod
