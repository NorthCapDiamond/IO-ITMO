obj-m = ch_drv.o
PWD = $(shell pwd)
all:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean
reset:
	sudo rmmod ch_drv
my: clean all
	sudo insmod ch_drv.ko
	sudo chmod 777 /dev/mychdev
	sudo echo "3+3" > /dev/mychdev
	cat /dev/mychdev
	sudo dmesg
run:  clean all my
