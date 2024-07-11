#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <time.h>

int main() {
    // reset the default umask bits so that 
    // it doesn't mess with the permissions on the fifo
    umask(0000);

    // int to refer to the number in the file descriptor table
    // most people just call this variable 'fd'
    int fileDescriptor = -1;
    
    // FIFOs are stored as files 
    // This is the file path to where we wish to store our FIFO 
    // and how we wish to name it
    char * myfifo = "./myfifo"; 

    // make fifo with RW permissions for all
    mkfifo(myfifo, 0666);

    // open fifo for write only 
    // program will hold here until the read end of the pipe is open in fifo2
    while (fileDescriptor == -1) fileDescriptor = open(myfifo, O_WRONLY, 0666);
    
    // buffer to hold the data to send through the pipe
    int buff[100]; 
    // seed rand to make it rand
    srand(time(NULL));

    for (int i = 0; i < 100; i++){
        // fill buffer with random numbers
        buff[i] = rand();
    }

    // write buff to the fifo 
    write(fileDescriptor, buff, sizeof(buff)); 

    // close fifo
    close(fileDescriptor);
    

    return 0;
}