typedef enum {
    PROTO_HELLO,
} proto_type_e ;

typedef struct {
    proto_type_e type;
    unsigned short len;
} proto_hdr_t;
