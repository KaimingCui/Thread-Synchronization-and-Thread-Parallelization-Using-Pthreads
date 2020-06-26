Project Name:CSCI 503 Operating System Lab3
Author: Kaiming Cui
Email: cuik@iu.edu

Document:
The tar ball "Lab3_KaimingCui.tar" contains 2 file directories: MPMC and PMM. Each directory contains four items: a executable file "KaimingCuiMPMC(PMM).out", a source code file "MPMC.c(PMM.c)", a Makefile and a README.txt.

Environment:
This project is developed in C language on tesla server by using vim.
It can run on tesla server.

Dependency:
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>

Building and Testing:
1. Upload the zip file to tesla server on local (before login).
    scp Lab3_KaimingCui.tar [user_name]@tesla.cs.iupui.edu:/home/[user_name]
    * [user_name] is your username in IUPUI.
2. Login to tesla server with your private password.
    ssh [user_name]@tesla.cs.iupui.edu
3. untar the tar ball.
	tar -xvf Lab3_KaimingCui.tar
4. Using Makefile to compile the source code
	make
5. The "KaimingCuiMPMC(KaimingCuiPMM).out" executable file is produced, run this file
	./KaimingCuiMPMC.out five arguments
	./KaimingCuiPMM.out two arguments
6. Then you will see the result. For MPMC, after run the program, wait a second(sleep time) then you will see the total number of items produced by producers and consumed by consumers. These two number should be same, I have tested it.
For PMM, you will see one line output that show "Two result matrix are same. Success." or an error message.

compare mechanism:
There are two global variable called pCounter and cCounter. whenever a producer produces an item, increase pCounter. Whenever a consumer consumes an item, decrease the cCounter by one. Finally, check pCounter and cCounter to see if they are same. If they same then that means all items are produced by all producer are consumed by all consumers, print Success.
And the number will should equal to the number of items that user inputs.


example output:

(base) [cuik@tesla MPMC]$ ./KaimingCuiMPMC.out 3 3 1 5 10
All threads produced 5 items.
All threads consumed 5 items.
(base) [cuik@tesla MPMC]$ ./KaimingCuiMPMC.out 3 3 3 5 10
All threads produced 5 items.
All threads consumed 5 items.
(base) [cuik@tesla MPMC]$ ./KaimingCuiMPMC.out 3 3 3 7 10
All threads produced 7 items.
All threads consumed 7 items.
(base) [cuik@tesla MPMC]$ ./KaimingCuiMPMC.out 4 5 3 10 15
All threads produced 10 items.
All threads consumed 10 items.
(base) [cuik@tesla MPMC]$ ./KaimingCuiMPMC.out 2 1 1 10 3
All threads produced 10 items.
All threads consumed 10 items.
Success!

