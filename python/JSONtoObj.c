#include <Python.h>
#include <ultrajson.h>
#include <stdlib.h>

/* object stack */
static JSOBJ *_stack = 0;
static int _max, _p, _f;

void push(JSOBJ obj) {
  if (!_stack) {
    _max = 256;
    _stack = malloc(sizeof(JSOBJ) * _max);
    _p = 0;
    _f = 0;
  }

  if (_p == _max) {
    _max *= 2;
    _stack = realloc(_stack, sizeof(JSOBJ) * _max);
  }

  if (_p < _f)
    Py_DECREF( (PyObject *) _stack[_p] );
  _stack[_p] = obj;
  ++_p;
  if (_f < _p)
    ++_f;
}

JSOBJ top() {
  return _stack[_p-1];
}

JSOBJ peek(int pos) {
  return _stack[_p-pos];
}

JSOBJ pop() {
  --_p;

  return _stack[_p];
}

int clean() {
  int ret = 0, i;

  if (_p != 0) {
    _p = -1;
    ret = -1;
  }

  for (i = _p+1; i < _f; i++)
    Py_DECREF( (PyObject *) _stack[i] );
  _p = 0;
  _f = 0;

  //if (_stack) {
    //free(_stack);
    //_stack = 0;
  //} 
  return ret;
}

void Object_objectAddKey()
{
  PyObject *value = pop();
  PyObject *name = pop();

  PyDict_SetItem ( (PyObject *) top(), name, value);
  return;
}

void Object_arrayAddItem()
{
  PyObject *value = pop();

  PyList_Append( (PyObject *) top(), value);
  return;
}

void Object_newString(wchar_t *start, wchar_t *end)
{
  push(PyUnicode_FromWideChar (start, (end - start)));
}

void Object_newTrue(void)
{ 
  Py_INCREF(Py_True);
  push(Py_True);
}

void Object_newFalse(void)
{
  Py_INCREF(Py_False);
  push(Py_False);
}

void Object_newNull(void)
{
  Py_INCREF(Py_None);
  push(Py_None);
}

void Object_newObject(void)
{
  push(PyDict_New());
}

void Object_newArray(void)
{
  push(PyList_New(0));
}

void Object_newInteger(JSINT32 value)
{
  push(PyInt_FromLong( (long) value));
}

void Object_newLong(JSINT64 value)
{
  push(PyLong_FromLongLong (value));
}

void Object_newDouble(double value)
{ 
  push(PyFloat_FromDouble(value));
}

PyObject* JSONToObj(PyObject* self, PyObject *arg)
{
	PyObject *ret;
	JSONObjectDecoder decoder = 
	{
		Object_newString,
		Object_objectAddKey,
		Object_arrayAddItem,
		Object_newTrue,
		Object_newFalse,
		Object_newNull,
		Object_newObject,
		Object_newArray,
		Object_newInteger,
		Object_newLong,
		Object_newDouble,
		top,
		PyObject_Malloc,
		PyObject_Free,
		PyObject_Realloc
	};

	if (!PyString_Check(arg))
	{
		PyErr_Format(PyExc_TypeError, "Expected string");
		clean();
		return NULL;
	}

	decoder.errorStr = NULL;
	decoder.errorOffset = NULL;
	
	JSON_DecodeObject(&decoder, PyString_AS_STRING(arg), PyString_GET_SIZE(arg)); 

	if (decoder.errorStr)
	{
		/*
		FIXME: It's possible to give a much nicer error message here with actual failing element in input etc*/
		
		PyErr_Format (PyExc_ValueError, "%s", decoder.errorStr);
		clean();
		
		return NULL;
	}

	ret = pop();
	if (clean()) {
	  PyErr_Format(PyExc_ValueError, "JSOBJ stack had more than one final value!");
	}
	return ret;
}

PyObject* JSONFileToObj(PyObject* self, PyObject *file)
{
	PyObject *read;
	PyObject *string;
	PyObject *result;

	if (!PyObject_HasAttrString (file, "read"))
	{
		PyErr_Format (PyExc_TypeError, "expected file");
		return NULL;
	}

	read = PyObject_GetAttrString (file, "read");

	if (!PyCallable_Check (read)) {
		Py_XDECREF(read);
		PyErr_Format (PyExc_TypeError, "expected file");
		return NULL;
	}

	string = PyObject_CallObject (read, NULL);
	Py_XDECREF(read);

	if (string == NULL)
	{
		return NULL;
	}

	result = JSONToObj (self, string);
	Py_XDECREF(string);

	if (result == NULL) {
		return NULL;
	}

	return result;
}

