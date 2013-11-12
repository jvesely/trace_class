#ifndef FEATURE_VECTOR_H_
#define FEATURE_VECTOR_H_

#include "processor.h"

typedef struct {
	double rw_ratio;
	double move_compute_ratio;
	double avg_repeat_access;
	double avg_reuse_time;
	double avg_access_distance;
} feature_vector_t;

int feature_vector_init(feature_vector_t *v, processor_t *p);
int feature_vector_print(feature_vector_t *v, const char *class);


#endif
