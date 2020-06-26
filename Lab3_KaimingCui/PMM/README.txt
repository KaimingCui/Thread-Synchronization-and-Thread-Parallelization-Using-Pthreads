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
For PMM, after get the two result matrix (one is computed by sequential method, another is computed by parallel multi-thread method). Compare each element of these two matrix. If these two matrix are same, print Success.

example output:

(base) [cuik@tesla PMM]$ ./KaimingCuiPMM.out 100 10
Two result Matrix are same. Success.

