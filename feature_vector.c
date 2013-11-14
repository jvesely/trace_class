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
#include <stdio.h>
#include "feature_vector.h"

int feature_vector_init(feature_vector_t *v, processor_t *p)
{
	assert(v);
	if (!p)
		return -EINVAL;

	v->rw_ratio = (double)p->reads / (double)p->writes;
	v->move_compute_ratio = (double)p->move / (double)p->compute;
	v->avg_access_distance =
		(double)p->atable.distance_sum / (double)p->atable.total_accesses;
	v->avg_repeat_access =
		(double)p->atable.total_accesses / (double)p->atable.count;

	double reuse_time_avg_sum = 0;

	for (size_t i = 0; i < p->atable.table_size; ++i) {
		node_t *it = p->atable.table[i];
		while (it) {
			const double avg = (double)it->total_reuse_time_sum /
				(double)(it->reads + it->writes);
			reuse_time_avg_sum += avg;
			it = it->next;
		}
	}

	v->avg_reuse_time = reuse_time_avg_sum / (double)p->atable.count;
	
	return 0;
}

int feature_vector_print(feature_vector_t *v, const char *class)
{
	assert(v);
	return printf("%s vector: R/W ratio: %f, M/C ratio: %f, avg repeats: "
		"%f, avg access distance: %f, avg reuse time: %f\n",
		class, v->rw_ratio, v->move_compute_ratio, v->avg_repeat_access,
		v->avg_access_distance, v->avg_reuse_time);
}
