#include "liblf.h"

bool test_rx (__s64 *nn, __u32 size)
{
    int pos = 0;

    for (pos = 0; pos < size; ++pos) {
        fprintf(stdout, "%lld\t", nn[pos]);
    }

    fprintf(stdout, "\n");

    return false;
}

int main(int argc, char** argv)
{
    rx_dp_notification(&test_rx);
    return 0;
}