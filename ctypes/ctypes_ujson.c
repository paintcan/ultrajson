#include "ultrajson.h"
#include <stdlib.h>
#include <string.h>

static int (*object_add_key)();
static int (*array_add_item)();
static int (*new_string)(wchar_t *s);

void Object_newString(wchar_t *start, wchar_t *end)
{
  static wchar_t *memarea = 0;
  static size_t memsz = 0;
  size_t len = (void *) end - (void *) start;

  if (memsz < len) {
    memsz = len * 2;
    memarea = realloc(memarea, memsz);
  }

  memcpy(memarea, start, len);
  memarea[len / sizeof(wchar_t) + 1] = '\0';
  new_string(memarea);
}

static int (*new_true)();
static int (*new_false)();
static int (*new_null)();
static int (*new_object)();
static int (*new_array)();
static int (*new_integer)(JSINT32 value);
static int (*new_long)(JSINT64 value);
static int (*new_double)(double value);
static int (*error)(char *str);

int init(int (*_object_add_key)(),
	 int (*_array_add_item)(),
	 int (*_new_string)(wchar_t *),
	 int (*_new_true)(),
	 int (*_new_false)(),
	 int (*_new_null)(),
	 int (*_new_object)(),
	 int (*_new_array)(),
	 int (*_new_integer)(JSINT32),
	 int (*_new_long)(JSINT64),
	 int (*_new_double)(double),
	 int (*_error)(char *)) {
  object_add_key = _object_add_key;
  array_add_item = _array_add_item;
  new_string = _new_string;
  new_true = _new_true;
  new_false = _new_false;
  new_null = _new_null;
  new_object = _new_object;
  new_array = _new_array;
  new_integer = _new_integer;
  new_long = _new_long;
  new_double = _new_double;
  error = _error;

  return 0;
}

void JSONToObj(char *str, int len)
{
  JSONObjectDecoder decoder = {
    Object_newString,
    object_add_key,
    array_add_item,
    new_true,
    new_false,
    new_null,
    new_object,
    new_array,
    new_integer,
    new_long,
    new_double,
    malloc,
    free,
    realloc,
  };

  decoder.errorStr = NULL;
  decoder.errorOffset = NULL;
	
  JSON_DecodeObject(&decoder, str, len);
  if (decoder.errorStr)
    error(decoder.errorStr);
}
