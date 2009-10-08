
#include <stdio.h>
#include "v6interface.h"

char **list = NULL;

int main(int argc, char *argv[])
{
    const char *dev = "eth0";
    char **it;
    printf("Vystup pro %s:\n", dev);
    list = getIpv6AddressList(dev);

    if (!list) {
        fprintf(stderr, "Pro zarizeni %s byl vracen prazdny list\n", dev);
        return 1;
    }
    for (it = list; it && *it; it++) {
        printf("Zarizeni %s ma IPv6 adresu: %s\n", dev, *it);
    }

    list = getIpv6AddressList(NULL);
    if (!list) {
        fprintf(stderr, "Pro vsechna zarizeni byl vracen prazdny list!\n");
        return 2;
    }
    for (it = list; it && *it; it++) {
        printf("Adresa %s\n", *it);
    }

    return 0;
}

