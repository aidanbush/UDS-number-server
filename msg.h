/* Author: Aidan Bush
 * Assign: Assign 3
 * Course: CMPT 360
 * Date: Dec. 2, 18
 * File: msg.h
 * Description: Simple message passing library.
 */
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

// frees the given packet_s struct
void free_pkt(packet_s *pkt);

/* SEND FUNCTIONS */

// sends a store packet with the given number to the given socket file
// descriptor
// returns 1 on success and 0 on failure
int send_stor_pkt(int sfd, int64_t num);

// sends a retrieve packet to the given socket file descriptor
// returns 1 on success and 0 on failure
int send_rtrv_pkt(int sfd);

// sends a number response packet to the given socket file descriptor
// returns 1 on success and 0 on failure
int send_num_pkt(int sfd, int64_t num);

// sends an OK packet to the given socket file descriptor
// returns 1 on success and 0 on failure
int send_ok_pkt(int sfd);

// sends a error packet to the given socket file descriptor
// returns 1 on success and 0 on failure
int send_err_pkt(int sfd);

/* READ FUNCTIONS */

// reads from a given socket file descriptor and return a packet_s struct
packet_s *read_pkt(int sfd);

#endif /* MSG_H */
