ccflags-y=-std=gnu99
obj-m += p2.o
KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	g++ p2tester.cpp -o p2tester
	g++ test1.cpp -o test1
	g++ test1.cpp -o test2
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm p2tester test1 test2
