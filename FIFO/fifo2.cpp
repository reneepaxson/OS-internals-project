#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
//#include <iostream>
#include <fstream>

using namespace std;

int main() {
    // if the input file already exists, delete it
    std::remove("./fifo-output.txt");

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

    // open fifo for read only 
    // program will hold here until the write end of the pipe is open in fifo1
    while (fileDescriptor == -1) fileDescriptor = open(myfifo, O_RDONLY, 0666);
    
    // buffer to hold the data sent through the pipe
    // size 1 since we read one entry at a time
    int buff[1]; 

    string filepath = "./fifo-output.txt";
    ofstream file;
    file.open(filepath);
    
    // `read` will return -1 if there's an error  
    // and 0 if the buffer is empty
    // and holds indefinitely if the buffer hasn't been written to yet
    while (read(fileDescriptor, buff, sizeof(buff)) > 0) {
        if (file.is_open()){
            file << buff[0] << std::endl;
        }
    }
    
    // close fifo
    close(fileDescriptor);

    // close file
    file.close();
    
    return 0;
}