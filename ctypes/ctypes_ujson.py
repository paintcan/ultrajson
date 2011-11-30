import ctypes

# decoding functionality

def object_add_key():
    print 'object add key'
    return 0

def array_add_item():
    print 'array add item'
    return 0

def new_string(s):
    print 'new string: %s' % s
    return 0

def new_true():
    print 'new true'
    return 0

def new_false():
    print 'new false'
    return 0

def new_null():
    print 'new null'
    return 0

def new_object():
    print 'new object'
    return 0

def new_array():
    print 'new array'
    return 0

def new_integer(i):
#    print i
#    print 'new integer: %s' % i
    return 0

def new_long(l):
    print 'new long: %d' % l
    return 0

def new_double(d):
    print 'new double: %f' % d
    return 0

def raise_error(e):
    print 'error: %s' % e
    return 0

int_void = ctypes.CFUNCTYPE(ctypes.c_int)
int_unicode = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_wchar_p)
int_int = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int32)
int_long = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int64)
int_double = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_double)
int_str = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_char_p)

# load and initialize ctypes_ujson
ctypes_ujson = ctypes.CDLL('ctypes_ujson.so')
ctypes_ujson_init = ctypes_ujson.init
ctypes_ujson_init.argtypes = [
    int_void,
    int_void,
    int_unicode,
    int_void,
    int_void,
    int_void,
    int_void,
    int_void,
    int_int,
    int_long,
    int_double,
    int_str ]

ctypes_ujson_init(
    int_void(object_add_key),
    int_void(array_add_item),
    int_unicode(new_string),
    int_void(new_true),
    int_void(new_false),
    int_void(new_null),
    int_void(new_object),
    int_void(new_array),
    int_int(new_integer),
    int_long(new_long),
    int_double(new_double),
    int_str(raise_error))

decode_internal = ctypes_ujson.JSONToObj
decode_internal.argtypes = [ctypes.c_char_p, ctypes.c_int]

# ujson functions

def decode(s):
    # marshal s into a BUFFER (ensure utf-8, ensure it is a byte buffer)
    # set up stacks etc

    decode_internal(s, len(s))

    # report any errors
    # return result
