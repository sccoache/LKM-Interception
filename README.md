# LKM-interception

To compile the files, run make with the Makefile in the present directory. The Makefile will create a .ko file (Different versions for each part of the assignment). Execute the makeful to create the .ko file. Run "sudo insmod [.ko file]" to insert the system call interception. To remove it, call "sudo rmmod [.ko file]".

Part 2 of the assignment included text files that can be piped into the program using standard terminal notatio, ie: "./p2<test.txt"
