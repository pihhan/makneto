
/*
 * Minimalistic tool to enumerate IPv6 addresses assigned to local systems
 */

#include <stdio.h>
#include <netinet/ip6.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

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


