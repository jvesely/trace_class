
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

int feature_vector_print(feature_vector_t *v)
{
	assert(v);
	return printf("Vector: R/W ratio: %f, M/C ratio: %f, avg repeats: %f, "
		"avg access distance: %f, avg reuse time: %f\n",
		v->rw_ratio, v->move_compute_ratio, v->avg_repeat_access,
		v->avg_access_distance, v->avg_reuse_time);
}
