cmd_/root/work/drivers/3.text_dev/text_dev.mod := printf '%s\n'   text_dev.o | awk '!x[$$0]++ { print("/root/work/drivers/3.text_dev/"$$0) }' > /root/work/drivers/3.text_dev/text_dev.mod
