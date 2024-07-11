#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <iostream>

int main() {
    // resets the default umask bits so that 
    // it doesn't mess with the permissions on the fifo
    umask(0000);

    // int to refer to the number in the file descriptor table
    int fileDescriptor = -1;
    
    // FIFOs are stored as files 
    char * myfifo = "./myfifo"; 

    // make fifo with RW permissions for all UGO
    mkfifo(myfifo, 0666);

    // open fifo for read only 
    // will hold here until the write end of the pipe is open
    while (fileDescriptor == -1) fileDescriptor = open(myfifo, O_RDONLY, 0666);
    
    // buffer to hold the data sent through the pipe
    int buff[100]; 


    while (read(fileDescriptor, buff, sizeof(buff)) > 0) {
        for (int i = 0; i < 100; i++) {
            std::cout << buff[i]<< std::endl;
        }
    }
    
    
    // close fifo
    close(fileDescriptor);
    

    return 0;
}