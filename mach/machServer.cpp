// #include <mach/mach.h>
// #include <mach/mach_port.h>

// int main() {
//     struct MACH_MSG_BASE
//     {
// 	    //Base structure for sending a Mach message.
// 	    //It must always start with the header, and
// 	    //have the following members if we're sending 
// 	    //a block of memory to another process.
// 	    //
// 	    mach_msg_header_t hdr;
	
// 	    mach_msg_body_t body;
// 	    mach_msg_ool_descriptor_t data;
// 	    mach_msg_type_number_t count;
// 	};

//     struct MACH_MSG_RCV
//     {
// 	    //Base structure for receiving a Mach message.
// 	    //
// 	    MACH_MSG_BASE msg;

// 	    //Space for mach_msg_trailer_t, which could be of a variable size.
// 	    //It is provided by the kernel. Let's give it some extra space.
// 	    //
// 	    char trailing_bytes[sizeof(void*) * 8];
//     };
    
// }