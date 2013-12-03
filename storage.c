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

/** Initialize sqlite backend storage */
int storage_init(storage_t *storage, const char *file, size_t size)
{
	assert(storage);
	if (!file)
		return -EINVAL;

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

/** Close sqlite backend storage */
void storage_fini(storage_t *storage)
{
	if (storage && storage->db) {
		sqlite3_close(storage->db);
	}
}	

/** Store datapoint in the database. */
int storage_store_vector(storage_t *storage, const char* class,
	const feature_vector_t *data)
{
	assert(storage);
	if (!data)
		return -EINVAL;

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

int storage_classify_vector(storage_t *storage, const feature_vector_t *data,
	const char **class)
{
	assert(storage);
	if (!data || !class)
		return -EINVAL;
	*class = "unknown";
	return 0;
}
