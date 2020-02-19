from ctypes import *

CBFUNC = CFUNCTYPE(c_bool, c_ushort, c_uint)

def activate(appid, model_uuid):
    appid_u = c_ushort(appid)
    model_uuid_u = c_uint(model_uuid)
    
    liblf = CDLL("liblf.so")
    ret_u = liblf.activate_model(appid_u, model_uuid_u)
    return c_int(ret_u).value

def reg_notification(func):
    func_p = CBFUNC(func)

    liblf = CDLL("liblf.so")
    ret_u = liblf.rx_activation_notification(func_p)
    return c_int(ret_u).value