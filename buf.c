#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#include "buf.h"

buf_s *init_buf()
{
	buf_s *buf = calloc(1, sizeof(buf_s));
	if (buf == NULL) {
		return NULL;
	}

	buf->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

	return buf;
}

void free_buf(buf_s *buf)
{
	free(buf);
}

int add_buf(buf_s *buf, uint64_t num)
{
	int ret_val = 0;
	// lock
	pthread_mutex_lock(&(buf->mutex));
	if (buf->size == BUF_LEN) {
		ret_val = 0;
		goto add_buf_return;
	}
	// add
	int pos = (buf->start + buf->size) % BUF_LEN;
	buf->buf[pos] = num;
	buf->size++;

	ret_val = 1;

add_buf_return:
	// unlock
	pthread_mutex_unlock(&(buf->mutex));
	return ret_val;
}

// sets num to the number retrived
int retrieve_buf(buf_s *buf, uint64_t *num)
{
	int ret_val = 0;
	// lock
	pthread_mutex_lock(&(buf->mutex));
	if (buf->size == 0) {
		ret_val = 0;
		goto retrieve_buf_return;
	}

	*num = buf->buf[buf->start];
	buf->start = (buf->start + 1) % BUF_LEN;
	buf->size--;

	ret_val = 1;

retrieve_buf_return:
	// unlock
	pthread_mutex_unlock(&(buf->mutex));
	return ret_val;
}
