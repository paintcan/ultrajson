import ctypes
#from collections import deque
from lib_pypy import _collections as collections

# decoding functionality

objstack = deque()
error = []

def object_add_key():
    item = objstack.popleft()
    key = objstack.popleft()
    objstack[0][key] = item

def array_add_item():
    item = objstack.popleft()
    objstack[0].append(item)

def new_string(s):
    objstack.appendleft(s)

def new_true():
    objstack.appendleft(True)

def new_false():
    objstack.appendleft(False)

def new_null():
    objstack.appendleft(None)

def new_object():
    objstack.appendleft({})

def new_array():
    objstack.appendleft([])

def new_integer(i):
    objstack.appendleft(i)

def new_long(l):
    objstack.appendleft(l)

def new_double(d):
    objstack.appendleft(d)

def raise_error(e):
    error.append('(ctypes) ujson parsing error: %s' % e)

void_void = ctypes.CFUNCTYPE(None)
void_unicode = ctypes.CFUNCTYPE(None, ctypes.c_wchar_p)
void_int = ctypes.CFUNCTYPE(None, ctypes.c_int32)
void_long = ctypes.CFUNCTYPE(None, ctypes.c_int64)
void_double = ctypes.CFUNCTYPE(None, ctypes.c_double)
void_str = ctypes.CFUNCTYPE(None, ctypes.c_char_p)

ctypes_ujson = ctypes.CDLL('ctypes_ujson.so')

decode_internal = ctypes_ujson.JSONToObj
decode_internal.argtypes = [ctypes.c_char_p, ctypes.c_int,
    void_void,
    void_void,
    void_unicode,
    void_void,
    void_void,
    void_void,
    void_void,
    void_void,
    void_int,
    void_long,
    void_double,
    void_str]

# ujson functions

def decode(s):
    # marshal s into a BUFFER (ensure utf-8, ensure it is a byte buffer)

    objstack.clear()
    decode_internal(s, len(s), 
                    void_void(object_add_key),
                    void_void(array_add_item),
                    void_unicode(new_string),
                    void_void(new_true),
                    void_void(new_false),
                    void_void(new_null),
                    void_void(new_object),
                    void_void(new_array),
                    void_int(new_integer),
                    void_long(new_long),
                    void_double(new_double),
                    void_str(raise_error))

    # report any errors
    if error:
        raise Exception(error.pop())

    if len(objstack) != 1:
        raise Exception('(ctypes) ujson exceptional condition: at end, stack has %d items instead of 1' % len(objstack))

    return objstack[0]
