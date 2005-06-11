/* BLURB lgpl

                           Coda File System
                              Release 6

          Copyright (c) 2002-2003 Carnegie Mellon University
                  Additional copyrights listed below

This  code  is  distributed "AS IS" without warranty of any kind under
the  terms of the  GNU  Library General Public Licence  Version 2,  as
shown in the file LICENSE. The technical and financial contributors to
Coda are listed in the file CREDITS.

                        Additional copyrights
                           none currently

#*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include <coda_config.h>
#include "codaconf.h"
#include "parse_realms.h"

#ifndef HAVE_STRUCT_IN6_ADDR
struct in6_addr {
        u_int8_t u6_addr[16];
};
#endif

#define MAXLINELEN 256
static char line[MAXLINELEN];

void SplitRealmFromName(char *name, char **realm)
{
    /* Here we do the following 'translation' */
    /* "name"     -> keep existing realm */
    /* "name@"    -> keep existing realm */
    /* "name@xxx" -> return realm 'xxx' */

    char *p;

    p = strrchr(name, '@');
    if (p) {
	*p = '\0';
	if (p[1])
	    *realm = &p[1];
    }
}

static void ResolveRootServers(char *servers, const char *service,
			       struct RPC2_addrinfo **res)
{
    struct RPC2_addrinfo hints = {
	.ai_family   = PF_INET,
	.ai_socktype = SOCK_DGRAM,
	.ai_protocol = IPPROTO_UDP,
	.ai_flags    = RPC2_AI_CANONNAME,
    };
    char *host;

    while ((host = strtok(servers, ", \t\n")) != NULL)
    {
	servers = NULL;
	coda_getaddrinfo(host, service, &hints, res);
    }
    /* sort preferred addresses to the head of the list */
    coda_reorder_addrinfo(res);
}
	
static int isbadaddr(struct RPC2_addrinfo *ai, const char *name)
{
    struct in_addr *ip;
    unsigned long h_ip;

#warning "assuming ipv4 only"
    if (ai->ai_family != PF_INET)
	return 0;

    ip = &((struct sockaddr_in *)ai->ai_addr)->sin_addr;
    h_ip = ntohl(ip->s_addr);

    if (h_ip == INADDR_ANY ||
	h_ip == INADDR_NONE ||
	h_ip == INADDR_LOOPBACK ||
	(h_ip & IN_CLASSA_NET) == IN_LOOPBACKNET ||
	IN_MULTICAST(h_ip) ||
	IN_BADCLASS(h_ip))
    {
	fprintf(stderr, "An address in realm '%s' resolved to unusable address '%s', ignoring it\n", name, inet_ntoa(*ip));
	return 1;
    }
    return 0;
}

void GetRealmServers(const char *name, const char *service,
		     struct RPC2_addrinfo **res)
{
    struct RPC2_addrinfo **p, *results = NULL;
    char *realmtab = NULL;
    FILE *f;
    int namelen, found = 0;

    if (!name || name[0] == '\0')
	CODACONF_STR(name, "realm", "localhost");

    if (!name || name[0] == '\0' || name[0] == '.' ||
	strcmp(name, "localhost") == 0) /* is not a globally accessible realm */
	return;

    CODACONF_STR(realmtab, "realmtab", SYSCONFDIR "/realms");

    f = fopen(realmtab, "r");
    if (f) {
	namelen = strlen(name);
	while (fgets(line, MAXLINELEN, f))
	{
	    if (line[0] == '#') continue;

	    if (strncmp(line, name, namelen) == 0 &&
		(line[namelen] == '\0' || isspace(line[namelen])))
	    {
		ResolveRootServers(&line[namelen], service, &results);
		found = 1;
		break;
	    }
	}
	fclose(f);
    }

    if (!found) {
	struct RPC2_addrinfo hints = {
	    .ai_family   = PF_INET,
	    .ai_socktype = SOCK_DGRAM,
	    .ai_protocol = IPPROTO_UDP,
	    .ai_flags    = RPC2_AI_CANONNAME | CODA_AI_RES_SRV,
	};

#ifdef PF_INET6
	/* As we expect only FQDNs, the name should contain at least one '.'
	 * This also prevents lookups for accidentally mistyped paths as well
	 * as things that the OS might look for like 'Recycle Bin'. */
	char tmp[sizeof(struct in6_addr)];
	if (inet_pton(PF_INET6, name, &tmp) <= 0)
#endif
	    if (strchr(name, '.') == NULL)
		return;

	coda_getaddrinfo(name, service, &hints, &results);
    }

    /* Is is really necessary to filter out loopback and other bad
     * addresses from the nameserver response? */
    for (p = &results; *p;) {
	if (isbadaddr(*p, name)) {
	    struct RPC2_addrinfo *cur = *p;
	    *p = cur->ai_next;
	    cur->ai_next = NULL;
	    RPC2_freeaddrinfo(cur);
	    continue;
	}
	p = &(*p)->ai_next;
    }
    *p = *res;
    *res = results;
}

