#include "ultrajson.h"
#include <stdlib.h>
#include <string.h>

static void (*new_string)(wchar_t *s);

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
  memarea[len / sizeof(wchar_t)] = '\0';
  memarea[len / sizeof(wchar_t) + 1] = '\0';
  new_string(memarea);
}

void JSONToObj(char *str, int len,
	       void (*_object_add_key)(),
	       void (*_array_add_item)(),
	       void (*_new_string)(wchar_t *),
	       void (*_new_true)(),
	       void (*_new_false)(),
	       void (*_new_null)(),
	       void (*_new_object)(),
	       void (*_new_array)(),
	       void (*_new_integer)(JSINT32),
	       void (*_new_long)(JSINT64),
	       void (*_new_double)(double),
	       void (*_error)(char *))
{
  new_string = _new_string;
  
  JSONObjectDecoder decoder = {
    Object_newString,
    _object_add_key,
    _array_add_item,
    _new_true,
    _new_false,
    _new_null,
    _new_object,
    _new_array,
    _new_integer,
    _new_long,
    _new_double,
    malloc,
    free,
    realloc,
  };

  decoder.errorStr = NULL;
  decoder.errorOffset = NULL;
	
  JSON_DecodeObject(&decoder, str, len);
  if (decoder.errorStr)
    _error(decoder.errorStr);
}
