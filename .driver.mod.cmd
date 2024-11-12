savedcmd_/home/pi/programing/os/driver.mod := printf '%s\n'   driver.o | awk '!x[$$0]++ { print("/home/pi/programing/os/"$$0) }' > /home/pi/programing/os/driver.mod
