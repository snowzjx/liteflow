#include "liblf.h"

#define APPID 31
#define MODEL_UUID 2333

int main(int argc, char** argv)
{
    int ret = activate_mode(APPID, MODEL_UUID);
    printf ("ret = %d\n", ret);
    return 0;
}