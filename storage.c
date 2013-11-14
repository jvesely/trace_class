#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "storage.h"

static const char * table_name_template = "%zumil";

static const char * create_table_template = " \
	CREATE TABLE IF NOT EXISTS `%s` ( \
		class VARCHAR(50) NOT NULL, \
		rw_ratio DOUBLE NOT NULL, \
		mc_ratio DOUBLE NOT NULL, \
		avg_repeat DOUBLE NOT NULL, \
		avg_reuse_time DOUBLE NOT NULL, \
		avg_distance DOUBLE NOT NULL \
	);";

static const char * insert_vector_template = " \
	INSERT INTO `%s` ( class, rw_ratio, mc_ratio, avg_repeat, \
		avg_reuse_time, avg_distance) VALUES ( \
		%Q, %f, %f, %f, %f, %f); \
	";	

int storage_init(storage_t *storage, const char *file, size_t size)
{
	assert(storage);

	int ret = snprintf(storage->table_name, sizeof(storage->table_name),
		table_name_template, size / 1000000);
	if (ret < 0)
		return ret;

	char * statement =
		sqlite3_mprintf(create_table_template, storage->table_name);
	if (!statement)
		return -ENOMEM;

	ret = sqlite3_open(file, &storage->db);
	if (!ret) {
		ret = sqlite3_exec(storage->db, statement, NULL, NULL, NULL);
	}
	sqlite3_free(statement);
	if (ret)
		sqlite3_close(storage->db);
	return ret;
}

void storage_fini(storage_t *storage)
{
	if (storage && storage->db) {
		sqlite3_close(storage->db);
	}
}	

int storage_store_vector(storage_t *storage, const char* class,
	const feature_vector_t *data)
{
	assert(storage);
	char * statement = sqlite3_mprintf(insert_vector_template,
		storage->table_name, class, data->rw_ratio,
		data->move_compute_ratio, data->avg_repeat_access,
		data->avg_access_distance, data->avg_reuse_time);
	if (!statement)
		return -ENOMEM;
	const int ret = sqlite3_exec(storage->db, statement, NULL, NULL, NULL);
	sqlite3_free(statement);
	return ret;
}
