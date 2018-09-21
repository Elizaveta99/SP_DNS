
/*************************************************************************
LAB 1

Edit this file ONLY!

*************************************************************************/
//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "dns.h"

typedef unsigned int BOOL;
#define FALSE 0
#define TRUE  1

#define MOD 7001
#define N 7002

typedef struct _DNS_ENTRY
{
	const char*        domainName;
	IPADDRESS    ip;
} DNS_ENTRY, *PDNS_ENTRY;


typedef struct 
{
	DNS_ENTRY Pair;
	struct list* next;
} list;

typedef list* plist;

plist* pList;

unsigned int HashFunc(char* hostName)
{
	/*unsigned int hash, i;
	for (hash = i = 0; i < strlen(hostName); ++i)
	{
		hash += hostName[i];
		hash %= MOD;
		hash += (hash << 10);
		hash %= MOD;
		hash ^= (hash >> 6);
		hash %= MOD;
	}
	hash += (hash << 3);
	hash %= MOD;
	hash ^= (hash >> 11);
	hash %= MOD;
	hash += (hash << 15);
	return hash % MOD;*/


	unsigned int hash = 5381;
	int c;

	while (c = *hostName++)
		hash = ((hash << 5) + hash) + c, hash %= MOD; /* hash * 33 + c */

	return hash;

}


BOOL LoadFromFile(const char* filename, PDNS_ENTRY* pDnsArray, unsigned int* size)
{
	FILE* fInput = NULL;
	unsigned int dnCount = 0;
	unsigned int i = 0;

	if (*pDnsArray)
		return FALSE;

	fInput = fopen_s(&fInput, filename, "r"); 

	if (NULL == fInput)
		return FALSE;

	fseek(fInput, 0, SEEK_SET);

	for (i = 0; i<dnCount && !feof(fInput); i++)
	{
		char buffer[201] = { 0 };
		char* pStringWalker = &buffer[0];
		unsigned int uHostNameLength = 0;
		unsigned int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;

		fgets(buffer, 200, fInput);

		if (5 != fscanf_s(fInput, "%d.%d.%d.%d %s", &ip1, &ip2, &ip3, &ip4, buffer, 200))
			continue;

		(*pDnsArray)[i].ip = (ip1 & 0xFF) << 24 |
			(ip2 & 0xFF) << 16 |
			(ip3 & 0xFF) << 8 |
			(ip4 & 0xFF);


		uHostNameLength = strlen(buffer);
		if (uHostNameLength)
		{
			(*pDnsArray)[i].domainName = (char*)malloc(uHostNameLength + 1);
			strcpy_s((*pDnsArray)[i].domainName, sizeof((*pDnsArray)[i].domainName), pStringWalker);
		}
	}

	fclose(fInput);
	return TRUE;
}




DNSHandle InitDNS()
{
	//pList = malloc(N * sizeof(list*));
	pList = malloc(N * sizeof(plist));
	//pList = malloc(N * sizeof(list));
	//if (pList != NULL)
		return (DNSHandle)1;
	//return INVALID_DNS_HANDLE;
}

void LoadHostsFile(DNSHandle hDNS, const char* hostsFilePath)
{
	PDNS_ENTRY pDnsArray = NULL;
	unsigned int size = 0;
	if (TRUE == LoadFromFile(hostsFilePath, &pDnsArray, &size))
	{
		for (int i = 0; i < size; i++)
		{
			int hash = HashFunc(pDnsArray[i].domainName);

			if (pList[hash] == NULL)
			{
				// or (*pList[hash])
				pList[hash]->Pair.domainName = (char*)malloc(strlen(pDnsArray[i].domainName) + 1) ;
				strcpy_s(pList[hash]->Pair.domainName, sizeof(pList[hash]->Pair.domainName), pDnsArray[i].domainName);
				pList[hash]->Pair.ip = pDnsArray[i].ip;
				pList[hash]->next = NULL;
			}
			else
			{
				while (pList[hash] != NULL && pList[hash]->Pair.ip != pDnsArray[i].ip)
					pList[hash] = pList[hash]->next;
				if (pList[hash] == NULL)
				{
					plist* tmp = malloc(sizeof(plist));					
					(*tmp)->Pair.domainName = (char*)malloc(strlen(pDnsArray[i].domainName) + 1);  //???
					strcpy_s((*tmp)->Pair.domainName, sizeof((*tmp)->Pair.domainName), pDnsArray[i].domainName);
					(*tmp)->Pair.ip = pDnsArray[i].ip; //???
					(*tmp)->next = NULL;
					pList[hash]->next = tmp;
				}

			}


			//list* temp = &pList[hash]; 
			//if (temp == NULL)
			//{
			//	pList[hash].Pair.domainName = (char*)malloc(strlen(pDnsArray[i].domainName) + 1) ;
			//	strcpy_s(pList[hash].Pair.domainName, sizeof(pList[hash].Pair.domainName), pDnsArray[i].domainName);
			//	pList[hash].Pair.ip = pDnsArray[i].ip;
			//	pList[hash].next = NULL;
			//}
			//else
			//{
			//	while (temp != NULL && temp->Pair.ip  != pDnsArray[i].ip)
			//		temp = temp->next;
			//	if (temp == NULL)
			//	{
			//		list* tmp = (list*)malloc(sizeof(list));					
			//		tmp->Pair.domainName = (char*)malloc(strlen(pDnsArray[i].domainName) + 1); 
			//		strcpy_s(tmp->Pair.domainName, sizeof(tmp->Pair.domainName), pDnsArray[i].domainName); 
			//		tmp->Pair.ip = pDnsArray[i].ip; //???
			//		tmp->next = NULL;
			//		pList[hash].next = tmp;
			//	}

			//}
		}
	}
}

IPADDRESS DnsLookUp(DNSHandle hDNS, const char* hostName)
{
	int hash = HashFunc(hostName);
	while (pList[hash] != NULL && &(pList[hash]->Pair.domainName) != NULL
		&& pList[hash]->Pair.domainName != hostName)
		pList[hash] = pList[hash]->next;
	printf("Test!\n\n %d", hash);
	if (pList[hash] != NULL)
	{
		return pList[hash]->Pair.ip;
	}
	return INVALID_IP_ADDRESS;
}

void ShutdownDNS(DNSHandle hDNS)
{
	for (int i = 0; i < N; i++)
		free(pList[i]->Pair.domainName);    // ??? всегда или сначала проверить, что выделена была
	free(pList); 
}
