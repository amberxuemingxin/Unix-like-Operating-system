Name and PennKey:
Donglun He, Donglun
Wenyi Huang, hwyhuang
Shufan Liu, shufanwl
Mingxin Xue, amberxue

A list of submitted source files:
Inside Kernel Folder: 
builtins.c
builtins.h
execute.c 
execute.h
handler.c
handler.h
header.h
jobs.c
jobs.h
kernel.c
kernel.h
logger.c
logger.h
parser.h
PCB.c
PCB.h
PennOS.c
queue.c
queue.h
scheduler.c
scheduler.h
shell.c
shell.h
stress.c
stress.h
user.c
user.h
Inside pennFAT folder:
FAT.c
FAT.h
macro.h
parser.h
pennFAT.c
pennfatlib.c
pennfatlib.h

Extra Credit Answers:
N/A

Compilation Instructions:
make dir
make

Overview of work accomplished:
A functional kernel that can schedule jobs by weight. 
Linked lists of stopped and running jobs
An interactive shell that can realize multiple builtin functions
A filesystem that has new/touch, remove, move, and search functionalities
Other functionalities like orphanize, zombify, block, unblock etc.

Description of code and code layout:
Builtins are builtin functions
execute is a single function that execute commands
handler handles incoming commands
jobs is the data structure the jobs linked list relies upon
kernel is where context switching, blocking, unblocking, process creating and killing happens
queue is the data structure that processes reside
scheduler schedules next process and finds a specific process
shell is the interactive part of the program where user input gets parsed and executed
user is our custom functions for spawn, waitpid, kill and spawn etc
FAT is the file stystem data structure
pennFAT is the implementation of the FAT
pennfatlib is a library of function calls like ls, rm, fread, fwrite

General comments and anything that can help us grade your code:
This has been an extremely difficult project for us to work on together because there are so many moving parts and delicate interlacing of codes that need to interact with each other very very well, otherwise it would lead to unimaginabily hard situations to debug, as sometimes we will be working on bugs that were unintentionally created because of some temporary revision to a previous version of the code, leading to a lot of collaboration overhead and some other headaches. We still have some bugs that we just don't have the time or energy to squash, but we are proud of what we made anyway because C is not the easiest language to work with and OS is not the most intuitive system to design either. 