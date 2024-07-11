# Inter-Process Communication: Named Pipe / FIFO
System: OSx

Language: C/C++

[fifo1.cpp](fifo1.cpp) writes 100 random integers to the fifo `myfifo`, and [fifo2.cpp](fifo2.cpp) reads the integers from `myfifo` and prints them out to the terminal. 

The user must build an executable from [fifo1.cpp](fifo1.cpp) and [fifo2.cpp](fifo2.cpp) in their local systems and run both at the same time for the program to work. 