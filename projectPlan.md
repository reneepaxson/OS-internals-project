TODO: update for readability

IPC Methods:
1. Namedpipe/FIFO
IPC Project Plan
• In macOS, a named pipe is called a “FIFO”. Only half-duplex transmission is permitted, meaning data can only travel one way at a time, and the communication must reside on the same machine.
• OS / Language: macOS, C
• System Calls:
o mkfifo()–createnamedpipe/fifo o open()–openfifo
o read()–readfromfifo
o write()–writetofifo
o close()–closefifo
2. Indirectmessagepassing-Machmessagepassing
• Mach is its own operating system used in macOS and iOS systems. It works as a indirect messaging system, where the “mailboxes” of an indirect messaging system are instead called “ports”. There are separate ports for sending a message and replying to a message, and these ports are “conceptually similar to pipes” (Darling Docs).
• OS / Language: macOS, C
• System Calls:
o mach_msg();
1. includesimportantparameters,likesendorreceive,sizeof
message sent (0 if receiving), max size of received message (0 if sending), name of receive port (MACH_PORT_NULL if sending), timeout, and notify port
o write();
1. towritemessagetofilefortesting
Renee Paxson Dr. Soon M. Chung CEG 4350 – OS Internals and Design

REFERENCES / helpful links for later: FIFO:
Renee Paxson Dr. Soon M. Chung CEG 4350 – OS Internals and Design
o GFG:NamedPipeofFIFO(https://www.geeksforgeeks.org/named-pipe-fifo- example-c-program/)
Mach Messaging:
o DarlingDocs(https://docs.darlinghq.org/internals/macos-specifics/mach-
ports.html)
o NeXTComputer:UsingMachMessages
(https://www.nextop.de/NeXTstep_3.3_Developer_Documentation/OperatingSyste
m/Part1_Mach/02_Messages/Messages.htmld/index.html)
o Medium:IPC–machmessage(https://medium.com/@ali.pourhadi/ipc-mach-
message-cab64c1b569)
o HackTricks:MachmessagingviaPorts(https://book.hacktricks.xyz/macos-
hardening/macos-security-and-privilege-escalation/macos-proces-abuse/macos- ipc-inter-process-communication)
- operating system concepts 9th edition practice problem solutions: https://codex.cs.yale.edu/avi/os-book/OS9/practice-exer-dir/index.html      
