#define BEGIN_MSG 0 /* Constants to identify the different messages */
#define END_MSG 1
#define REPLY_MSG 2

#define MAXDATA 3

typedef struct  {
    unsigned int
    msg_type_name : MSG_TYPE_BYTE, /* Type of data */
    msg_type_size : 8,             /* Number of bits per item */
msg_type_number : 12,          /* Number of items */
msg_type_inline : 1,           /* If true, data follows; else
a ptr to data follows */
msg_type_longform : 1,         /* Name, size, number follow */
msg_type_deallocate : 1,       /* Deallocate port rights or
memory */
msg_type_unused : 1;
} msg_type_t;

typedef struct {
msg_header_t Head;
msg_type_t   aType;
int          a;
msg_type_t   bType;
int          b;
} Request;

typedef struct {
unsigned int msg_unused : 24,
msg_simple : 8;
unsigned int msg_size;
int          msg_type;
port_t       msg_local_port;
port_t       msg_remote_port;
int          msg_id;
} msg_header_t;

struct simp_msg_struct {
msg_header_t  h;               /* message header */
msg_type_t    t;               /* type descriptor */
int           inline_data1;    /* start of data array */
int           inline_data2[2];
};
struct simp_msg_struct  msg_xmt;
port_t                  comm_port, reply_port;

/* Fill in the message header. */
msg_xmt.h.msg_simple = TRUE;
msg_xmt.h.msg_size = sizeof(struct simp_msg_struct);
msg_xmt.h.msg_type = MSG_TYPE_NORMAL;
msg_xmt.h.msg_local_port = reply_port;
msg_xmt.h.msg_remote_port = comm_port;
msg_xmt.h.msg_id = BEGIN_MSG;

/* Fill in the type descriptor. */
msg_xmt.t.msg_type_name = MSG_TYPE_INTEGER_32;
msg_xmt.t.msg_type_size = 32;
msg_xmt.t.msg_type_number = MAXDATA;
msg_xmt.t.msg_type_inline = TRUE;
msg_xmt.t.msg_type_longform = FALSE;
msg_xmt.t.msg_type_deallocate = FALSE;

/* Fill in the array of data items. */
msg_xmt.inline_data1 = value1;
msg_xmt.inline_data2[1] = value2;
msg_xmt.inline_data2[2] = value3; 