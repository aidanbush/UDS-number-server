#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "msg.h"

#define RTRV_PKT_STR		"RTRV"
#define OK_PKT_STR		"OK"
#define STOR_PKT_STR		"STOR"
#define ERR_PKT_STR		"ERR"

// check if account for null terminator
#define RTRV_PKT_STR_LEN	(sizeof(RTRV_PKT_STR) - 1)
#define OK_PKT_STR_LEN		(sizeof(OK_PKT_STR) - 1)
#define STOR_PKT_STR_LEN	(sizeof(STOR_PKT_STR) - 1)
#define ERR_PKT_STR_LEN		(sizeof(ERR_PKT_STR) - 1)

#define PKT_NUM_LEN		8

#define STOR_PKT_LEN		(STOR_PKT_STR_LEN + PKT_NUM_LEN + 1)
#define RTRV_PKT_LEN		(RTRV_PKT_STR_LEN + 1)
#define OK_PKT_LEN		(OK_PKT_STR_LEN + 1)
#define RNUM_PKT_LEN		(PKT_NUM_LEN + 1)
#define ERR_PKT_LEN		(ERR_PKT_STR_LEN + 1)

#define MAX_PKT_LEN		(STOR_PKT_LEN)

static packet_s *init_pkt(pkt_type type, uint64_t num)
{
	packet_s *pkt = malloc(sizeof(packet_s));
	if (pkt == NULL)
		return NULL;

	pkt->type = type;
	pkt->num = num;

	return pkt;
}

void free_pkt(packet_s *pkt)
{
	free(pkt);
}

/* SEND PACKETS */

// send_store
int send_stor_pkt(int sfd, uint64_t num)
{
	char msg[STOR_PKT_LEN] = {0};

	strncpy(msg, STOR_PKT_STR, STOR_PKT_STR_LEN);
	*((uint64_t *)(msg + STOR_PKT_STR_LEN)) = num;

	// send packet
	if (write(sfd, msg, STOR_PKT_LEN) != STOR_PKT_LEN)
		return 0;// error
	return 1;
}

// send_rtrv
int send_rtrv_pkt(int sfd)
{
	char msg[RTRV_PKT_LEN] = {0};
	strncpy(msg, RTRV_PKT_STR, RTRV_PKT_STR_LEN);

	// send packet
	if (write(sfd, msg, RTRV_PKT_LEN) != RTRV_PKT_LEN)
		return 0;
	return 1;
}

// send_num
int send_num_pkt(int sfd, uint64_t num)
{
	char msg[RNUM_PKT_LEN] = {0};
	*((uint64_t *)(msg)) = num;

	// send packet
	if (write(sfd, msg, RNUM_PKT_LEN) != RNUM_PKT_LEN)
		return 0;
	return 1;
}

// send_ok
int send_ok_pkt(int sfd)
{
	char msg[OK_PKT_LEN] = {0};
	strncpy(msg, OK_PKT_STR, OK_PKT_STR_LEN);

	// send packet
	if (write(sfd, msg, OK_PKT_LEN) != OK_PKT_LEN)
		return 0;
	return 1;
}

// send err
int send_err_pkt(int sfd)
{
	char msg[ERR_PKT_LEN] = {0};
	strncpy(msg, ERR_PKT_STR, ERR_PKT_STR_LEN);

	// send packet
	if (write(sfd, msg, ERR_PKT_LEN) != ERR_PKT_LEN)
		return 0;
	return 1;
}

/* HANDLE PACKETS */

static packet_s *handle_rtrv_pkt(char *buf, int len)
{
	if (len != RTRV_PKT_LEN)
		return NULL;

	// if RTRV
	if (strncmp(buf, RTRV_PKT_STR, RTRV_PKT_STR_LEN) == 0)
		return init_pkt(PKT_OP_RTRV, 0);

	return NULL;

}

static packet_s *handle_ok_pkt(char *buf, int len)
{
	if (len != OK_PKT_LEN)
		return NULL;

	if (strncmp(buf, OK_PKT_STR, OK_PKT_STR_LEN) == 0)
		return init_pkt(PKT_OP_OK, 0);

	return NULL;
}

static packet_s *handle_stor_pkt(char *buf, int len)
{
	if (len != STOR_PKT_LEN) {
		printf("len != STOR_PKT_LEN\n");
		return NULL;
	}

	if (strncmp(buf, STOR_PKT_STR, STOR_PKT_STR_LEN) == 0) {
		return init_pkt(PKT_OP_STOR, *((uint64_t*)(buf + STOR_PKT_STR_LEN)));
	}

	return NULL;
}

static packet_s *handle_rnum_pkt(char *buf, int len)
{
	if (len != RNUM_PKT_LEN)
		return NULL;

	return init_pkt(PKT_OP_RNUM, (uint64_t)(buf));
}

static packet_s *handle_err_pkt(char *buf, int len)
{
	if (len != ERR_PKT_LEN)
		return NULL;

	if (strncmp(buf, ERR_PKT_STR, ERR_PKT_STR_LEN) == 0)
		return init_pkt(PKT_OP_ERR, 0);

	return NULL;
}

static packet_s *internal_err_pkt()
{
	return init_pkt(PKT_TYPE_INTER_ERR, 0);
}

packet_s *read_pkt(int sfd)
{
	int rd;
	// TODO make sure have room and has null terminator
	int len = MAX_PKT_LEN;
	char buf[MAX_PKT_LEN] = {0};
	packet_s *pkt = NULL;

	rd = read(sfd, buf, len);

	for (int i = 0; i < rd; i++)
		printf("%x", buf[i]);
	printf("\n");


	switch (rd) {
		case RTRV_PKT_LEN:
			pkt = handle_rtrv_pkt(buf, rd);
			break;
		case OK_PKT_LEN:
			pkt = handle_ok_pkt(buf, rd);
			break;
		case STOR_PKT_LEN:
			printf("STOR\n");
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
