from ctypes import *

CBFUNC = CFUNCTYPE(c_bool, c_ushort, c_uint)

def reg_notification(func):
    func_p = CBFUNC(func)

    liblf = CDLL("liblf.so")
    ret_u = liblf.rx_activation_notification(func_p)
    return c_int(ret_u).value