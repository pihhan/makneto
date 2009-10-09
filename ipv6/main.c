
#include <stdio.h>
#include "v6interface.h"

char **list = NULL;

void print_list(char ** list, const char *caption)
{
    char **it;
    if (caption)
        printf("%s\n", caption);
    for (it = list; it && *it; it++) {
        printf("%s\n", *it);
    }
}

int main(int argc, char *argv[])
{
    const char *dev = "eth0";
    const char *dev2 = "wlan0";
    char **it;
    printf("Vystup pro %s:\n", dev);
    list = getIpv6AddressList(dev);
    for (it = list; it && *it; it++) {
        printf("Zarizeni %s ma IPv6 adresu: %s\n", dev, *it);
    }

    if (!list) {
        fprintf(stderr, "Pro zarizeni %s byl vracen prazdny list\n", dev);
        return 1;
    }

    printf("Vystup pro %s:\n", dev2);
    list = getIpv6AddressList(dev2);
    for (it = list; it && *it; it++) {
        printf("Zarizeni %s ma IPv6 adresu: %s\n", dev, *it);
    }

    printf("Vystup vsech adres:\n");
    list = getIpv6AddressList(NULL);
    if (!list) {
        fprintf(stderr, "Pro vsechna zarizeni byl vracen prazdny list!\n");
        return 2;
    }
    for (it = list; it && *it; it++) {
        printf("Adresa %s\n", *it);
    }

    printf("Vystup vsech pres getifaddrs()\n");
    list = getIpv6AddressList2(NULL);
    for (it = list; it && *it; it++) {
        printf("Adresa2: %s\n", *it);
    }

    list = getAddressList(NULL, AFB_ANY, SCOPE_GLOBAL);
    print_list(list, "Globalni adresy.");

    list = getAddressList(NULL, AFB_ANY, SCOPE_HOST);
    print_list(list, "Lokalni adresy.");

    return 0;
}

