
/*
 * Minimalistic tool to enumerate IPv6 addresses assigned to local systems
 */

#include <stdio.h>
#include <netinet/ip6.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <ifaddrs.h>

#include "v6interface.h"

Ipv6AddressArray newAddressArray()
{
    Ipv6AddressArray x = { 0, NULL };
    return x;
}

void initAddressArray(Ipv6AddressArray *array)
{
    if (!array) return;
    array->size = 0;
    array->address = NULL;
}

/** @brief Add next address to list.
    @param list List of addresses to add to
    @param addr Address to add
    @return New number of addresses in list, 0 if allocation failed.
    */
size_t addAddressToList(Ipv6AddressArray *list, struct sockaddr_in6 addr)
{
    size_t newsize;
    struct sockaddr_in6 *newlist;
    if (!list)
        return 0;
    newsize = (list->size + 1) * sizeof(struct sockaddr_in6);
    newlist = realloc(list->address, newsize);
    if (!newlist)
        return 0;
    newlist[list->size] = addr;
    ++list->size;
    list->address = newlist;
    return list->size;
}

/** @brief Return array of found addresses.
    @interface  name of interface to list addresses from, or NULL, in that case, all addresses are returned. 

    NOTE: this reads addresses from Linux specific file, it is not
    expected to work on other POSIX systems or any other than linux.
*/
int getIpv6AddressArray(const char *interface, Ipv6AddressArray *array)
{
    struct sockaddr_in6 addr;
    FILE *proc;
    int i;
    int iface;
    int scope;
    int dynamic;
    char if_name[MAX_IFACE_NAME_LEN];
    Ipv6AddressArray arr;
    int c;

    initAddressArray(&arr);

    proc = fopen(PROC_IP6_PATH, "r");
    if (!proc)
        return 0;

    do {
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;

    for (i = 0; i<16; ++i) {
        unsigned int x;
        if (fscanf(proc, "%02x", &x) <= 0) {
            if (i==0) {
                fclose(proc);
                if (array)
                    *array = arr;
                return 1;
            } else
                return 0;
        } else {
            addr.sin6_addr.s6_addr[i] = x;
        }

    }

    if (fscanf(proc, " %02x", &iface) <= 0) // what is it?
        return 0;
    if (fscanf(proc, " %02x", &i) <= 0) 
        return 0;
    if (fscanf(proc, " %02x", &scope) <= 0)
        return 0;
    if (fscanf(proc, " %02x", &dynamic) <= 0) // is dynamic, or manual address?
        return 0;
    // FIXME: jak specifikovat delku if_name, kterou je mozne cist?
    if_name[0] = '\0';
    if (fscanf(proc, " %s", if_name) <=0) {
        return 0;
    } else {
        if (!interface || !strcmp(if_name, interface)) {
            addAddressToList(&arr, addr);
        }
    }

    c = getc(proc);
    if (c == EOF) 
        break;
    if (c != '\n') // error in format, newline expected!
        break;

    } while (!feof(proc));

    fclose(proc);
    if (array)
    *array = arr;
    return 1;
}

/** @brief Return list of addreses in text format.
    @return Array of pointers to address, terminated by NULL pointer, or NULL if error occurred.
*/
char ** getIpv6AddressList(const char *interface)
{
    Ipv6AddressArray arr;
    size_t  p;
    char name[MAX_HOST_LEN];
    char **list;

    if (getIpv6AddressArray(interface, &arr) == 0)
        return NULL;

    list = malloc((sizeof(char *)+1) * arr.size);
    if (!list)
        return NULL;
    
    for (p = 0; p<arr.size; ++p) {
        if (getnameinfo((struct sockaddr *) &arr.address[p], sizeof(struct sockaddr_in6),
                name, MAX_HOST_LEN, NULL, 0, NI_NUMERICHOST)==0) {
            char *a;
            a = malloc(strlen(name)+1);
            strcpy(a, name);
            list[p] = a;
        } else {
            fprintf(stderr, "Problem with conversion to string\n");
        }

    }
    list[p] = NULL;
    return list;
}

/** @brief Get list of addresses in BSD compatible way.
    @return list of address strings, or NULL on error.
*/
char ** getIpv6AddressList2(const char *interface)
{
    struct ifaddrs *addrlist;
    char ** stringlist;

    if (getifaddrs(&addrlist)==0) {
        size_t count = 0;
        struct ifaddrs *it=NULL;
        int i;
        char name[MAX_HOST_LEN];

        for (it = addrlist; it; it = it->ifa_next) {
            if (it->ifa_addr->sa_family == AF_INET6) 
                ++count;
        }
        stringlist = malloc(sizeof(char *) * (count + 1));

        name[0] = '\0';
        for (it = addrlist, i = 0; it; it = it->ifa_next) {
            if (it->ifa_addr->sa_family == AF_INET6) {
                if (getnameinfo(it->ifa_addr, 
                        sizeof(struct sockaddr_in6),
                        name, MAX_HOST_LEN, NULL, 0, NI_NUMERICHOST)==0) 
                {
                    char *s;
                    s = malloc(strlen(name)+1);
                    if (!s) {
                        freeifaddrs(addrlist);
                        free(stringlist);
                        return NULL;
                    }
                    strcpy(s, name);
                    stringlist[i++] = s;
                } else {
                    freeifaddrs(addrlist);
                    free(stringlist);
                    return NULL;
                }
            }
        }
        stringlist[i] = NULL;
        freeifaddrs(addrlist);

        return stringlist;
    } else {
        return NULL;
    }
}

AddressFamilyBits v6AFtoBits(unsigned char af)
{
    switch (af) {
        case AF_INET:   return AFB_INET;
        case AF_INET6:  return AFB_INET6;
        default:        return AFB_NONE;
    }
}

Ipv6Scope v6AddressScope(struct sockaddr *addr)
{
    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *a = (struct sockaddr_in *) addr;

        if (a->sin_addr.s_addr == htonl(INADDR_LOOPBACK)) {
            return SCOPE_HOST;
        } else if ((a->sin_addr.s_addr & 0xFFFF0000) == (0xA9FE0000)) {
            // host lies in 169.254.x.y network
            return SCOPE_LINK;
        } else
            return SCOPE_GLOBAL;
    } else if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6 *a = (struct sockaddr_in6 *) addr;
        if ((a->sin6_addr.s6_addr[0] & 0xFE)==0xFE 
            && (a->sin6_addr.s6_addr[1] & 0xC0)==0x80) {
            return SCOPE_LINK;
        } else if ((a->sin6_addr.s6_addr[0] & 0xFE)==0xFE 
            && (a->sin6_addr.s6_addr[1] & 0xC0)==0xC0) {
            return SCOPE_SITE;
        } else {
            bool ishost = true;
            int i;
            for (i=0; i<15; i++) {
                if (a->sin6_addr.s6_addr[i] != 0) {
                    ishost = false;
                    break;
                }
            }
            if (ishost && a->sin6_addr.s6_addr[15] == 1)
                return SCOPE_HOST;
            else
                return SCOPE_GLOBAL;
        }
    }
    return SCOPE_UNDEFINED;
}

/** @brief Get list of addresses, based on 
    @param interface interface to get addresses from, or NULL for all addresses on system
    @param families Bits of families to include in list, 0 for default
    @param scopes Which types address to include, 0 for default
*/
char ** getAddressList(const char *interface, AddressFamilyBits families, Ipv6Scope scopes)
{
    struct ifaddrs *addrlist;
    char ** stringlist;

    if (families == 0) 
        families = AFB_ANY;
    if (scopes == 0)
        scopes = SCOPE_GLOBAL;

    if (getifaddrs(&addrlist)==0) {
        size_t count = 0;
        struct ifaddrs *it=NULL;
        int i;
        char name[MAX_HOST_LEN];

        for (it = addrlist; it; it = it->ifa_next) {
            if (!it->ifa_addr)
                continue;
            if ((v6AFtoBits(it->ifa_addr->sa_family) & families)
                && (v6AddressScope(it->ifa_addr) & scopes)
                && (!interface || !strcmp(interface, it->ifa_name))) 
                ++count;
        }
        stringlist = malloc(sizeof(char *) * (count + 1));

        name[0] = '\0';
        for (it = addrlist, i = 0; it; it = it->ifa_next) {
            if (!it->ifa_addr)
                continue;
            if ((v6AFtoBits(it->ifa_addr->sa_family) & families)
                && (v6AddressScope(it->ifa_addr) & scopes)
                && (!interface || !strcmp(interface,it->ifa_name)) ) {
                // FIXME: jak zjistovat velikost adresy pro neznamy AF?
                if (getnameinfo(it->ifa_addr, 
                        sizeof(struct sockaddr_in6),
                        name, MAX_HOST_LEN, NULL, 0, NI_NUMERICHOST)==0) 
                {
                    char *s;
                    s = malloc(strlen(name)+1);
                    if (!s) {
                        freeifaddrs(addrlist);
                        free(stringlist);
                        return NULL;
                    }
                    strcpy(s, name);
                    stringlist[i++] = s;
                } else {
                    freeifaddrs(addrlist);
                    free(stringlist);
                    return NULL;
                }
            }
        }
        stringlist[i] = NULL;
        freeifaddrs(addrlist);

        return stringlist;
    } else {
        return NULL;
    }
}

