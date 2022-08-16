from ctypes import *

CBFUNC = CFUNCTYPE(c_bool, POINTER(c_longlong), c_size_t)

def reg_notification(func):
    func_p = CBFUNC(func)

    liblf = CDLL("liblf.so")
    ret_u = liblf.rx_tcp_dp_notification(func_p)
    return c_int(ret_u).value