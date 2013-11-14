

#ifndef STORAGE_H_
#define STORAGE_H_

#include <sqlite3.h>
#include "feature_vector.h"

typedef struct {
	sqlite3 *db;
	char table_name[10];
} storage_t;

int storage_init(storage_t *storage, const char *file, size_t size);
void storage_fini(storage_t *storage);
int storage_store_vector(storage_t *storage, const char* class,
	const feature_vector_t *data);

#endif
