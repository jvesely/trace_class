/*
 * Copyright (c) 2013 Jan Vesely
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#define __USE_BSD
#include <string.h>

#include "data_point_heap.h"

struct data_point {
	const char *class;
	double distance;
};

static int data_point_is_greater(const data_point_t *a, const data_point_t *b)
{
	assert(a);
	assert(b);

	return a->distance == -1 || (b->distance != -1 && (a->distance > b->distance));
}

static void data_point_swap(data_point_t *a, data_point_t *b)
{
	assert(a);
	assert(b);

	data_point_t tmp  = *a;
	*a = *b;
	*b = tmp;
}

static int data_point_set(data_point_t *p, double d, const char *class)
{
	assert(p);
	char *cpy = strdup(class);
	if(!cpy)
		return -ENOMEM;

	free((char*)p->class);
	p->distance = d;
	p->class = cpy;
	return 0;
}

int data_point_heap_init(data_point_heap_t *h, size_t k)
{
	assert(h);
	h->heap = calloc(k + 1, sizeof(data_point_t));
	if (!h->heap)
		return -ENOMEM;
	h->size = k + 1;
	for (size_t i = 0; i < h->size; ++i) {
		h->heap[i].distance = -1;
	}
	return 0;
}

void data_point_heap_fini(data_point_heap_t *h)
{
	if (h)
		for (size_t i = 0; i < h->size; ++i)
			free((char*)h->heap[i].class);
}

static void dump(data_point_heap_t *h)
{
	assert(h);
	for (unsigned i = 0; i < h->size; ++i)
		printf("nearest: <%s, %f>\n", h->heap[i].class,
			h->heap[i].distance);
}

int data_point_heap_insert(data_point_heap_t *h,
	double distance, const char *name)
{
	assert(h);

	/* The new distance is greater than the biggest in our heap */
	if (h->heap[0].distance != -1 && h->heap[0].distance < distance)
		return 0;

	/* replace the biggest with the new one */
	int ret = data_point_set(h->heap, distance, name);
	if (ret)
		return ret;
	/* Now sort the heap */
	for (unsigned i = 0; i < h->size;) {
		const unsigned r = (i + 1) * 2;
		const unsigned l = r - 1;

		unsigned greater = 0;
		if (r < h->size && l < h->size)
			greater = data_point_is_greater(
				&h->heap[r], &h->heap[l]) ? r : l;
		else if (r < h->size)
			greater = r;
		else if (l < h->size)
			greater = l;

		if (greater && data_point_is_greater(
			&h->heap[greater], &h->heap[i])) {
			data_point_swap(&h->heap[greater], &h->heap[i]);
			i = greater;
		} else {
			break;
		}
	}
	return 0;
}

int data_point_heap_get_elect(data_point_heap_t *h, const char **class)
{
	assert(h);
	assert(class);

	const char * best = NULL;
	unsigned votes = 0;

	for (unsigned i = 1; i < h->size; ++i) {
		unsigned ballot = 0;
		for (unsigned j = 1; j < h->size; ++j)
			if (strcmp(h->heap[i].class, h->heap[j].class) == 0)
				++ballot;
		if (ballot >= votes) {
			best = h->heap[i].class;
			votes = ballot;
		}
	}

	printf("Ballot won by '%s' with %u votes.\n", best, votes);

	*class = strdup(best);
	return 0;
}
