
#ifndef V6INTERFACE_H
#define V6INTERFACE_H


#define PROC_IP6_PATH   "/proc/net/if_inet6"
// TODO: zjistit nekde skutecnou hodnotu
#define MAX_IFACE_NAME_LEN  256
#define MAX_HOST_LEN        256

typedef enum {
    SCOPE_UNDFINED = 0,
    SCOPE_LOCAL,
    SCOPE_SITE,
    SCOPE_LINK,
    SCOPE_GLOBAL
} Ipv6Scope;

typedef struct {
    size_t          size;
    struct sockaddr_in6 *address;
} Ipv6AddressArray;


// funkce
void initAddressArray(Ipv6AddressArray *array);
size_t addAddressToList(Ipv6AddressArray *list, struct sockaddr_in6 addr);
int getIpv6AddressArray(const char *interface, Ipv6AddressArray *array);
char ** getIpv6AddressList(const char *interface);

#endif

