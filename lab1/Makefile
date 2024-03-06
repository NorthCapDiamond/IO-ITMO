obj-m = ch_drv.o
PWD = $(shell pwd)
all:
        make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules
clean:
        make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean
restore:
        sudo rmmod ch_drv
run: clean all
        sudo insmod ch_drv.ko
        sudo chmod 777 /dev/var2
        sudo echo "3+3" > /dev/var2
        cat /dev/var2
        sudo dmesg
