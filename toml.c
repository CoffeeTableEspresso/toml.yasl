#include "yasl/yasl.h"
#include "yasl/yasl_aux.h"
#include "tomlc99/toml.h"

#include <string.h>
#include <errno.h>

static void YASL_pushtomltable(struct YASL_State *S, toml_table_t *table);
static void YASL_pushtomlarray(struct YASL_State *S, toml_array_t *array);

static void YASL_pushtomlvalue_table(struct YASL_State *S, toml_table_t *conf, const char *key) {
	toml_array_t *array = toml_array_in(conf, key);
	if (array) {
		YASL_pushtomlarray(S, array);
		return;
	}

	toml_table_t *table = toml_table_in(conf, key);
	if (table) {
		YASL_pushtomltable(S, table);
		return;
	}

	toml_datum_t value = toml_string_in(conf, key);
	if (value.ok) {
		YASL_pushzstr(S, value.u.s);
		return;
	}

	value = toml_bool_in(conf, key);
	if (value.ok) {
		YASL_pushbool(S, (bool) value.u.b);
		return;
	}

	value = toml_int_in(conf, key);
	if (value.ok) {
		YASL_pushint(S, value.u.i);
		return;
	}

	value = toml_double_in(conf, key);
	if (value.ok) {
		YASL_pushfloat(S, value.u.d);
		return;
	}

	YASL_print_err(S, "ValueError: unsupported value.");
	YASL_throw_err(S, YASL_VALUE_ERROR);
}

static void YASL_tomlpushvalue_array(struct YASL_State *S, toml_array_t *arr, int i) {
	toml_array_t *array = toml_array_at(arr, i);
	if (array) {
		YASL_pushtomlarray(S, array);
		return;
	}

	toml_table_t *table = toml_table_at(arr, i);
	if (table) {
		YASL_pushtomltable(S, table);
		return;
	}

	toml_datum_t value = toml_string_at(arr, i);
	if (value.ok) {
		YASL_pushzstr(S, value.u.s);
		return;
	}

	value = toml_bool_at(arr, i);
	if (value.ok) {
		YASL_pushbool(S, (bool) value.u.b);
		return;
	}

	value = toml_int_at(arr, i);
	if (value.ok) {
		YASL_pushint(S, value.u.i);
		return;
	}

	value = toml_double_at(arr, i);
	if (value.ok) {
		YASL_pushfloat(S, value.u.d);
		return;
	}

	YASL_print_err(S, "ValueError: unsupported value.");
	YASL_throw_err(S, YASL_VALUE_ERROR);
}

static void YASL_pushtomlarray(struct YASL_State *S, toml_array_t *array) {
	YASL_pushlist(S);

	const int len = toml_array_nelem(array);
	for (int i = 0; i < len; i++) {
		YASL_tomlpushvalue_array(S, array, i);
		YASL_listpush(S);
	}
}

static void YASL_pushtomltable(struct YASL_State *S, toml_table_t *conf) {
	YASL_pushtable(S);

	int i = 0;
	while (true) {
		const char *tmp = toml_key_in(conf, i++);
		if (!tmp) break;

		YASL_pushzstr(S, tmp);
		YASL_pushtomlvalue_table(S, conf, tmp);
		YASL_tableset(S);

	}
}

int YASL_toml_parse(struct YASL_State *S) {
	if (!YASL_isstr(S)) {
		YASLX_print_err_bad_arg_type(S, "toml.parse", 0, "str", YASL_peektypename(S));
		YASL_throw_err(S, YASL_TYPE_ERROR);
	}

	char *str = YASL_peekcstr(S);

	char errbuff[200];
	toml_table_t *conf = toml_parse(str, errbuff, sizeof errbuff);

	if (!conf) {
		YASL_print_err(S, "ValueError: %s", errbuff);
		YASL_throw_err(S, YASL_VALUE_ERROR);
	}

	YASL_pushtomltable(S, conf);

	return 1;
}

int YASL_toml_open(struct YASL_State *S) {
	if (!YASL_isstr(S)) {
		YASLX_print_err_bad_arg_type(S, "toml.open", 0, "str", YASL_peektypename(S));
		YASL_throw_err(S, YASL_TYPE_ERROR);
	}

	char *str = YASL_peekcstr(S);

	FILE *f = fopen(str, "r");
	if (!f) {
		YASL_print_err(S, "ValueError: %s", strerror(errno));
		YASL_throw_err(S, YASL_VALUE_ERROR);
	}

	char errbuff[200];
	toml_table_t *conf = toml_parse_file(f, errbuff, sizeof errbuff);
	fclose(f);


	if (!conf) {
		YASL_print_err(S, "ValueError: %s", errbuff);
		YASL_throw_err(S, YASL_VALUE_ERROR);
	}

	YASL_pushtomltable(S, conf);

	return 1;
}

int YASL_load_dyn_lib(struct YASL_State *S) {
	YASL_pushtable(S);

	YASL_pushlit(S, "parse");
	YASL_pushcfunction(S, YASL_toml_parse, 1);
	YASL_tableset(S);

	YASL_pushlit(S, "open");
	YASL_pushcfunction(S, YASL_toml_open, 1);
	YASL_tableset(S);

	return 1;
}
