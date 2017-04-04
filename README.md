# LKM-interception

To compile the files, run make with the Makefile in the present directory. The Makefile will create a .ko file (Different versions for each part of the assignment). Execute the makeful to create the .ko file. Run "sudo insmod [.ko file]" to insert the system call interception. To remove it, call "sudo rmmod [.ko file]".

Part 2 of the assignment included text files that can be piped into the program using standard terminal notation, ie: ./p2<test.txt

# Part 1
The assignment consistst of simple alterations to existing system calls. Every time a regular user opens or closes a file, the program will print this information to the system log. However, we do not want this to happen for non-user actions (such as those by the root user, known service user accounts, etc.). Accordingly, we intercepted and modified the existing system calls for open and close to add kprint statements for regular users. In the syslog, the open messages should look like Jan 6 18:24:52 kernel: [ 105.033521] User 1000 is opening file: /etc/motd, while the close messages may look like Jan 6 18:24:53 kernel: [ 108.511234] User 1000 is closing file descriptor: 2. The program will also look at every read call to determine if the file contains the string VIRUS. If it does, it will write a warning to the system log such as: Jan 6 18:24:52 kernel: [ 105.033521] User 1000 read from file descriptor 2, but that file contained the word VIRUS!

# Part 2
In this phase 2 of the project, I created a new executable named procAncestry. The procAncestry command takes one parameter: the process ID (PID) to examine. The procAncestry command will go through all the processes running on the system and find the targeted PID. Once it does, it will traverse the process’s children and siblings, noting the PIDs for each. Once that’s done, it will begin climbing the process tree ancestry, beginning with the target process’s parent, followed by it’s grandparent, and so on until it reaches a parentless node (likely init). 
