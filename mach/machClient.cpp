#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <mach/mach.h>
#include <unistd.h>



struct message {
    mach_msg_header_t header;
    int data;
};

mach_port_t sendPort;
mach_port_t receivePort;

int main() {

	mach_port_allocate(
		mach_task_self(),
		MACH_PORT_RIGHT_SEND,
		&sendPort
	);

	mach_port_allocate(
		mach_task_self(),
		MACH_PORT_RIGHT_RECEIVE,
		&receivePort
	);

	int pid = fork();

	if (pid ==0) {
		// parent?

		message msgParent;

		//construct the header 
		msgParent.header.msgh_size = sizeof(msgParent);
		msgParent.header.msgh_remote_port = receivePort;
		msgParent.header.msgh_local_port = sendPort; 
		msgParent.header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_COPY_RECEIVE);

		// seed rand to make it rand
    	srand(time(NULL));
		
		msgParent.data = rand();
		
		// for (int i = 0; i < 100; i++){
        // 	// fill message with 100 random numbers
        // 	msgParent.data[i] = rand();
		// 	std::cout << msgParent.data[i] << std::endl;
        // 	//file << buff[i] << std::endl;
    	// }
		
		

		// send the message
		mach_msg_return_t ret;
		ret = mach_msg(&msgParent.header,
			MACH_SEND_MSG,
			sizeof(msgParent),
			0,
			MACH_PORT_NULL,
			MACH_MSG_TIMEOUT_NONE,
			MACH_PORT_NULL
		);
		
		//MACH_SEND_INVALID_HEADER 
		std::cout<<"parent (send confirm):" << ret << "\n";
		std::cout << msgParent.data << std::endl;
		
	}
	else {
		//child!

		message msgChild;
		//construct the header 
		//msgChild.header.msgh_size = sizeof(msgChild);
		//msgChild.header.msgh_remote_port = receivePort;
		//msgChild.header.msgh_local_port = sendPort; 
		
		//mach_msg_return_t ret;
		
		mach_msg(&msgChild.header,
			MACH_RCV_MSG,
			0,
			sizeof(msgChild),
			receivePort,
			MACH_MSG_TIMEOUT_NONE,
			MACH_PORT_NULL
		);

		

		

		std::cout<< "\nChild:\n";
		std::cout<< msgChild.data<<std::endl;
		// for (int i = 0; i< 100; i++){
		// 	std::cout<< msgChild.data[i]<<std::endl;
		// }
		

		
	}

}