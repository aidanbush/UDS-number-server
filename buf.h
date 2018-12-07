/* Author: Aidan Bush
 * Assign: Assign 3
 * Course: CMPT 360
 * Date: Dec. 3, 18
 * File: buf.h
 * Description: thread safe ring buffer library, that uses a statically sized
 *	buffer.
 */
#ifndef BUF_H
#define BUF_H

#ifndef BUF_LEN
#define BUF_LEN		64
#endif /* BUF_LEN */

#include <pthread.h>
#include <stdint.h>

typedef struct buf {
	int64_t buf[BUF_LEN];
	int start;
	int size;
	pthread_mutex_t mutex;
} buf_s;

// initializes and returns a newly allocated buf_s struct
// Returns NULL on failure
buf_s *init_buf();

// frees the given buf_s struct
void free_buf(buf_s *buf);

// Retrives a single value from the gven buf_s and stores it in num
// Also locks the buf_s when preforming this task
// Returns 1 on success and 0 on failure
int retrieve_buf(buf_s *buf, int64_t *num);

// Adds the given number to the provided buf_s
// Also locks the buf_s when preforming this task
// Returns 1 on success and 0 on failure
int add_buf(buf_s *buf, int64_t num);

// prints the buffer to stdout, with one number per line
// Also locks the buf_s when preforming this task
// Returns 1 on success and 0 on failure
int print_buf(buf_s *buf);

#endif /* BUF_H */
