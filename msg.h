#ifndef MSG_H
#define MSG_H

typedef enum {
	PKT_OP_RTRV = 0,	// retrieve
	PKT_OP_STOR,		// store number
	PKT_OP_RNUM,		// retrieved number
	PKT_OP_OK,		// store success
	PKT_OP_ERR,		// error packet, was unable to complete request
	PKT_TYPE_INTER_ERR,	// internal error
} pkt_type;

typedef struct packet {
	pkt_type type;
	int64_t num;
} packet_s;

void free_pkt(packet_s *pkt);

/* SEND FUNCTIONS */
int send_stor_pkt(int sfd, int64_t num);

int send_rtrv_pkt(int sfd);

int send_num_pkt(int sfd, int64_t num);

int send_ok_pkt(int sfd);

int send_err_pkt(int sfd);

/* READ */

packet_s *read_pkt(int sfd);

#endif /* MSG_H */
