cmd_/root/work/drivers/8.char/char.mod := printf '%s\n'   char.o | awk '!x[$$0]++ { print("/root/work/drivers/8.char/"$$0) }' > /root/work/drivers/8.char/char.mod
