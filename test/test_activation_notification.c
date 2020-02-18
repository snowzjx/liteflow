#include "liblf.h"

bool test_rx (__u8 appid, __u32 model_uuid)
{
    static int count = 0;
    
    printf("appid = %u, model_uuid = %u\n", appid, model_uuid);
    count ++;
    if (count > 3) {
        return true;
    }
    return false;
}

int main(int argc, char** argv)
{
    rx_activation_notification(&test_rx);
    return 0;
}