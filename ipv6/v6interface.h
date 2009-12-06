
#ifndef V6INTERFACE_H
#define V6INTERFACE_H
#ifdef __cplusplus
extern "C" {
#endif

#define PROC_IP6_PATH   "/proc/net/if_inet6"
// TODO: zjistit nekde skutecnou hodnotu
#define MAX_IFACE_NAME_LEN  256
#define MAX_HOST_LEN        256

typedef enum {
    SCOPE_UNDEFINED = 0,
    SCOPE_HOST  = (1 << 1),
    SCOPE_SITE  = (1 << 2),
    SCOPE_LINK  = (1 << 3),
    SCOPE_GLOBAL= (1 << 4)
} Ipv6Scope;

typedef enum {
    AFB_NONE    = 0,
    AFB_INET    = (1 << 1),
    AFB_INET6   = (1 << 2),
    AFB_ANY     = (AFB_INET|AFB_INET6)
} AddressFamilyBits;

typedef struct {
    size_t          size;
    struct sockaddr_in6 *address;
} Ipv6AddressArray;


// funkce
void initAddressArray(Ipv6AddressArray *array);
size_t addAddressToList(Ipv6AddressArray *list, struct sockaddr_in6 addr);
int getIpv6AddressArray(const char *interface, Ipv6AddressArray *array);
char ** getIpv6AddressList(const char *interface);
char ** getIpv6AddressList2(const char *interface);

char ** getAddressList(const char *interface, AddressFamilyBits families, Ipv6Scope scopes);
AddressFamilyBits v6AFtoBits(unsigned char af);
Ipv6Scope v6AddressScope(struct sockaddr *addr);
void v6Destroylist(char **list);

#ifdef __cplusplus
} // extern C
#endif
#endif

