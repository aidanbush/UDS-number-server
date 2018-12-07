/* Author: Aidan Bush
 * Assign: Assign 3
 * Course: CMPT 360
 * Date: Dec. 3, 18
 * File: buf.c
 * Description: thread safe ring buffer library, that uses a statically sized
 *	buffer.
 */
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "buf.h"

// initializes and returns a newly allocated buf_s struct
// Returns NULL on failure
buf_s *init_buf()
{
	buf_s *buf = calloc(1, sizeof(buf_s));

	if (buf == NULL)
		return NULL;

	buf->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

	return buf;
}

// frees the given buf_s struct
void free_buf(buf_s *buf)
{
	free(buf);
}

// Retrives a single value from the gven buf_s and stores it in num
// Also locks the buf_s when preforming this task
// Returns 1 on success and 0 on failure
int add_buf(buf_s *buf, int64_t num)
{
	int ret_val = 0;
	int pos;

	pthread_mutex_lock(&(buf->mutex));

	if (buf->size == BUF_LEN) {
		ret_val = 0;
		goto add_buf_return;
	}

	pos = (buf->start + buf->size) % BUF_LEN;

	buf->buf[pos] = num;
	buf->size++;

	ret_val = 1;

add_buf_return:
	pthread_mutex_unlock(&(buf->mutex));

	return ret_val;
}

// Retrives a single value from the gven buf_s and stores it in num
// Also locks the buf_s when preforming this task
// Returns 1 on success and 0 on failure
int retrieve_buf(buf_s *buf, int64_t *num)
{
	int ret_val = 0;

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
	pthread_mutex_unlock(&(buf->mutex));

	return ret_val;
}

// Aligns the buffer to start a index 0, does not attempt to keep order nor sort
// requires the buffer to already be locked
static int align_buffer(buf_s *buf)
{
	// make sure lock is locked before proceeding
	if (pthread_mutex_trylock(&(buf->mutex)) != EBUSY) {
		pthread_mutex_unlock(&(buf->mutex));
		return 0;
	}

	int64_t tmp_buf[BUF_LEN];

	// copy buf to tmp_buf
	for (int i = 0; i < buf->size && i < BUF_LEN; i++)
		tmp_buf[i] = buf->buf[(buf->start + i) % BUF_LEN];

	// copy buf back aligning it
	for (int i = 0; i < buf->size && i < BUF_LEN; i++)
		buf->buf[i] = tmp_buf[i];

	buf->start = 0;
	return 1;
}

// compares two int64_t values for use in qsort
static int cmp_ints(const void *a, const void *b)
{
	return *(int64_t *) a - *(int64_t *)b;
}

// prints the buffer to stdout, with one number per line
// Also locks the buf_s when preforming this task
// Returns 1 on success and 0 on failure
int print_buf(buf_s *buf)
{
	pthread_mutex_lock(&(buf->mutex));

	// align buffer
	if (!align_buffer(buf))
		return 0;

	// sort
	qsort(buf->buf, buf->size, sizeof(int64_t), cmp_ints);

	// print buffer
	for (int i = 0; i < buf->size && i < BUF_LEN; i++)
		printf("%ld\n", buf->buf[i]);

	pthread_mutex_unlock(&(buf->mutex));

	return 1;
}
