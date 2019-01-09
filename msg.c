/* Author: Aidan Bush
 * Assign: Assign 3
 * Course: CMPT 360
 * Date: Dec. 2, 18
 * File: msg.c
 * Description: Simple message passing library.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "msg.h"

// packet message strings
#define RTRV_PKT_STR		"RTRV"
#define OK_PKT_STR		"OK"
#define STOR_PKT_STR		"STOR"
#define ERR_PKT_STR		"ERR"

// packet message string lengths
#define RTRV_PKT_STR_LEN	(sizeof(RTRV_PKT_STR))
#define OK_PKT_STR_LEN		(sizeof(OK_PKT_STR))
#define STOR_PKT_STR_LEN	(sizeof(STOR_PKT_STR))
#define ERR_PKT_STR_LEN		(sizeof(ERR_PKT_STR))

// length of a number when sent in a packet
#define PKT_NUM_LEN		8

// total packet lengths
#define STOR_PKT_LEN		(STOR_PKT_STR_LEN + PKT_NUM_LEN - 1)
#define RTRV_PKT_LEN		(RTRV_PKT_STR_LEN - 1)
#define OK_PKT_LEN		(OK_PKT_STR_LEN - 1)
#define RNUM_PKT_LEN		(PKT_NUM_LEN)
#define ERR_PKT_LEN		(ERR_PKT_STR_LEN - 1)

// store packet number starting index
#define STOR_PKT_NUM_START	(STOR_PKT_STR_LEN - 1)

// maximum valid packet length
#define MAX_PKT_LEN		(STOR_PKT_LEN)

// initialized a new packet with the given type and num
static packet_s *init_pkt(pkt_type type, int64_t num)
{
	packet_s *pkt = malloc(sizeof(packet_s));

	if (pkt == NULL)
		return NULL;

	pkt->type = type;
	pkt->num = num;

	return pkt;
}

// frees the given packet_s struct
void free_pkt(packet_s *pkt)
{
	free(pkt);
}

/* SEND PACKETS */

// sends a store packet with the given number to the given socket file
// descriptor
// returns 1 on success and 0 on failure
int send_stor_pkt(int sfd, int64_t num)
{
	char msg[STOR_PKT_LEN + 1] = {0};

	strcpy(msg, STOR_PKT_STR);
	*((int64_t *)(msg + STOR_PKT_NUM_START)) = num;

	// send packet
	if (write(sfd, msg, STOR_PKT_LEN) != STOR_PKT_LEN)
		return 0;// error
	return 1;
}

// sends a retrieve packet to the given socket file descriptor
// returns 1 on success and 0 on failure
int send_rtrv_pkt(int sfd)
{
	char msg[RTRV_PKT_LEN + 1] = {0};

	strcpy(msg, RTRV_PKT_STR);

	// send packet
	if (write(sfd, msg, RTRV_PKT_LEN) != RTRV_PKT_LEN)
		return 0;
	return 1;
}

// sends a number response packet to the given socket file descriptor
// returns 1 on success and 0 on failure
int send_num_pkt(int sfd, int64_t num)
{
	char msg[RNUM_PKT_LEN + 1] = {0};
	*((int64_t *)(msg)) = num;

	// send packet
	if (write(sfd, msg, RNUM_PKT_LEN) != RNUM_PKT_LEN)
		return 0;
	return 1;
}

// sends an OK packet to the given socket file descriptor
// returns 1 on success and 0 on failure
int send_ok_pkt(int sfd)
{
	char msg[OK_PKT_LEN + 1] = {0};

	strcpy(msg, OK_PKT_STR);

	// send packet
	if (write(sfd, msg, OK_PKT_LEN) != OK_PKT_LEN)
		return 0;
	return 1;
}

// sends a error packet to the given socket file descriptor
// returns 1 on success and 0 on failure
int send_err_pkt(int sfd)
{
	char msg[ERR_PKT_LEN + 1] = {0};

	strcpy(msg, ERR_PKT_STR);

	// send packet
	if (write(sfd, msg, ERR_PKT_LEN) != ERR_PKT_LEN)
		return 0;
	return 1;
}

/* HANDLE PACKETS */

// creates a packet_s struct for a RTRV request
// Returns NULL if not a valid request
static packet_s *handle_rtrv_pkt(char *buf, int len)
{
	if (len != RTRV_PKT_LEN)
		return NULL;

	// if RTRV
	if (strncmp(buf, RTRV_PKT_STR, RTRV_PKT_STR_LEN) == 0)
		return init_pkt(PKT_OP_RTRV, 0);

	return NULL;

}

// creates a packet_s struct for a OK request
// Returns NULL if not a valid request
static packet_s *handle_ok_pkt(char *buf, int len)
{
	if (len != OK_PKT_LEN)
		return NULL;

	if (strncmp(buf, OK_PKT_STR, OK_PKT_STR_LEN) == 0)
		return init_pkt(PKT_OP_OK, 0);

	return NULL;
}

// creates a packet_s struct for a STOR request
// Returns NULL if not a valid request
static packet_s *handle_stor_pkt(char *buf, int len)
{
	if (len != STOR_PKT_LEN)
		return NULL;

	if (strncmp(buf, STOR_PKT_STR, STOR_PKT_STR_LEN - 1) == 0)
		return init_pkt(PKT_OP_STOR,
				*((int64_t *)(buf + STOR_PKT_NUM_START)));

	return NULL;
}

// creates a packet_s struct for a RNUM request
// Returns NULL if not a valid request
static packet_s *handle_rnum_pkt(char *buf, int len)
{
	if (len != RNUM_PKT_LEN)
		return NULL;

	return init_pkt(PKT_OP_RNUM, *((int64_t *)(buf)));
}

// creates a packet_s struct for a ERR request
// Returns NULL if not a valid request
static packet_s *handle_err_pkt(char *buf, int len)
{
	if (len != ERR_PKT_LEN)
		return NULL;

	if (strncmp(buf, ERR_PKT_STR, ERR_PKT_STR_LEN) == 0)
		return init_pkt(PKT_OP_ERR, 0);

	return NULL;
}

// creates a packet_s struct for an internal error
static packet_s *internal_err_pkt()
{
	return init_pkt(PKT_TYPE_INTER_ERR, 0);
}

/* READ FUNCTIONS */

// reads from a given socket file descriptor and return a packet_s struct
packet_s *read_pkt(int sfd)
{
	int rd;
	int len = MAX_PKT_LEN;
	char buf[MAX_PKT_LEN] = {0};
	packet_s *pkt = NULL;

	rd = read(sfd, buf, len);

	switch (rd) {
	case RTRV_PKT_LEN:
		pkt = handle_rtrv_pkt(buf, rd);
		break;
	case OK_PKT_LEN:
		pkt = handle_ok_pkt(buf, rd);
		break;
	case STOR_PKT_LEN:
		pkt = handle_stor_pkt(buf, rd);
		break;
	case RNUM_PKT_LEN:
		pkt = handle_rnum_pkt(buf, rd);
		break;
	case ERR_PKT_LEN:
		pkt = handle_err_pkt(buf, rd);
		break;
	default:
		pkt = internal_err_pkt();
		break;
	}

	return pkt;
}
