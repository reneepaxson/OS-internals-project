// #define BEGIN_MSG 0 /* Constants to identify the different messages */
// #define END_MSG 1
// #define REPLY_MSG 2

// #define MAXDATA 3

// struct simp_msg_struct {
// msg_header_t  h;               /* message header */
// msg_type_t    t;               /* type descriptor */
// int           inline_data1;    /* start of data array */
// int           inline_data2[2];
// };
// struct simp_msg_struct  msg_xmt;
// port_t                  comm_port, reply_port;

// /* Fill in the message header. */
// msg_xmt.h.msg_simple = TRUE;
// msg_xmt.h.msg_size = sizeof(struct simp_msg_struct);
// msg_xmt.h.msg_type = MSG_TYPE_NORMAL;
// msg_xmt.h.msg_local_port = reply_port;
// msg_xmt.h.msg_remote_port = comm_port;
// msg_xmt.h.msg_id = BEGIN_MSG;

// /* Fill in the type descriptor. */
// msg_xmt.t.msg_type_name = MSG_TYPE_INTEGER_32;
// msg_xmt.t.msg_type_size = 32;
// msg_xmt.t.msg_type_number = MAXDATA;
// msg_xmt.t.msg_type_inline = TRUE;
// msg_xmt.t.msg_type_longform = FALSE;
// msg_xmt.t.msg_type_deallocate = FALSE;

// /* Fill in the array of data items. */
// msg_xmt.inline_data1 = value1;
// msg_xmt.inline_data2[1] = value2;
// msg_xmt.inline_data2[2] = value3; 