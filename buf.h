#ifndef BUF_H
#define BUF_H

#ifndef BUF_LEN
#define BUF_LEN		1
#endif /* BUF_LEN */

#include <pthread.h>
#include <stdint.h>

typedef struct buf {
	uint64_t buf[BUF_LEN];
	int start;
	int size;
	pthread_mutex_t mutex;
} buf_s;

buf_s *init_buf();

void free_buf(buf_s *buf);

int retrieve_buf(buf_s *buf, uint64_t *num);

int add_buf(buf_s *buf, uint64_t num);

#endif /* BUF_H */
