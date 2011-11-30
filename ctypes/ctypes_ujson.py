import ctypes

# decoding functionality

def object_add_key():
    print 'object add key'

def array_add_item():
    print 'array add item'

def new_string(s):
    print 'new string: %s' % s

def new_true():
    print 'new true'

def new_false():
    print 'new false'

def new_null():
    print 'new null'

def new_object():
    print 'new object'

def new_array():
    print 'new array'

def new_integer(i):
    print 'new integer: %s' % i

def new_long(l):
    print 'new long: %d' % l

def new_double(d):
    print 'new double: %f' % d

def raise_error(e):
    print 'error: %s' % e

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
    # set up stacks etc

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
    # return result
