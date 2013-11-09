
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
	v->avg_repeat_access = 0;
	v->avg_reuse_distance = 0;
	
	return 0;
}

int feature_vector_print(feature_vector_t *v)
{
	assert(v);
	return printf("Vector: R/W ratio: %f, M/C ratio: %f, avg repeat: %f, "
		"avg reuse distance: %f\n",
		v->rw_ratio, v->move_compute_ratio, v->avg_repeat_access,
		v->avg_reuse_distance);
}
