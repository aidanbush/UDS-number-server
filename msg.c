#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "msg.h"

#define STOR_PKT_LEN		13
#define RTRV_PKT_LEN		5
#define OK_PKT_LEN		3
#define RNUM_PKT_LEN		8
#define ERR_PKT_LEN		4

#define MAX_PKT_LEN		13

#define RTRV_PKT_STR		"RTRV"
#define OK_PKT_STR		"OK"
#define STOR_PKT_STR		"STOR"
#define ERR_PKT_STR		"ERR"

// check if account for null terminator
#define RTRV_PKT_STR_LEN	sizeof(RTRV_PKT_STR)
#define OK_PKT_STR_LEN		sizeof(OK_PKT_STR)
#define STOR_PKT_STR_LEN	sizeof(STOR_PKT_STR)
#define ERR_PKT_STR_LEN		sizeof(ERR_PKT_STR)

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

// send_store
// send_recv
// read_pkt

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

	if (strncmp(buf, OK_PKT_STR, OK_PKT_STR_LEN))
		return init_pkt(PKT_OP_OK, 0);

	return NULL;
}

static packet_s *handle_stor_pkt(char *buf, int len)
{
	if (len != STOR_PKT_LEN)
		return NULL;

	if (strncmp(buf, STOR_PKT_STR, STOR_PKT_STR_LEN))
		return init_pkt(PKT_OP_STOR, (uint64_t)(buf + STOR_PKT_STR_LEN));

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

	if (strncmp(buf, ERR_PKT_STR, ERR_PKT_STR_LEN))
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
// send_num
// send_set
