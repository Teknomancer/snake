/** @file
 * Generic double linked list, very simple implementation.
 */

#include "List.h"
#include <stdlib.h>

void ListInit(PLIST pList)
{
    pList->pHead = NULL;
    pList->pTail = NULL;
    pList->cItems = 0;
}

uint32_t ListSize(PLIST pList)
{
    return pList->cItems;
}

bool ListIsEmpty(PLIST pList)
{
    return pList->cItems == 0 ? true : false;
}

static int ListAddNode(PLIST pList, PLISTITEM pNode, void *pvData)
{
    if (pNode)
    {
        if (!pList->pHead)
        {
            pList->pHead = pNode;
            pList->pTail = pNode;
            pNode->pPrev = NULL;
        }
        else
        {
            pNode->pPrev = pList->pTail;
            pList->pTail->pNext = pNode;
            pList->pTail = pNode;
        }
        pNode->pNext = NULL;
        pNode->pvData = pvData;
        pList->cItems++;
        return 0;
    }
    return -1;
}

int ListAdd(PLIST pList, void *pvData)
{
    PLISTITEM pNode = (PLISTITEM)malloc(sizeof(LISTITEM));
    return ListAddNode(pList, pNode, pvData);
}

static int ListAddNodeAsHead(PLIST pList, PLISTITEM pNode)
{
    if (!pNode)
        return -1;

    if (!pList->pHead)
    {
        pList->pHead = pNode;
        pList->pTail = pNode;
        pNode->pPrev = NULL;
        pNode->pNext = NULL;
    }
    else
    {
        pNode->pPrev = NULL;
        pNode->pNext = pList->pHead;
        pList->pHead->pPrev = pNode;
        pList->pHead = pNode;
    }
    pList->cItems++;
    return 0;
}

static void ListRemoveNode(PLIST pList, PLISTITEM pNode, bool fFree)
{
    if (pNode->pNext)
        pNode->pNext->pPrev = pNode->pPrev;
    if (pNode->pPrev)
        pNode->pPrev->pNext = pNode->pNext;
    if (pNode == pList->pHead)
        pList->pHead = pNode->pNext;
    else if (pNode == pList->pTail)
        pList->pTail = pNode->pPrev;
    if (fFree)
        free(pNode);
    pList->cItems--;
}

void ListRemove(PLIST pList, void *pvData)
{
    PLISTITEM pNode = pList->pHead;
    while (pNode)
    {
        if (pNode->pvData == pvData)
        {
            ListRemoveNode(pList, pNode, true);
            return;
        }
        pNode = pNode->pNext;
    }
}

static PLISTITEM ListRemoveTailNode(PLIST pList)
{
    PLISTITEM pNode = pList->pTail;
    if (pList->pTail)
    {
        ListRemoveNode(pList, pNode, false /* don't free pNode */);
        return pNode;
    }
    return NULL;
}

void ListMoveTailToHead(PLIST pList)
{
    if (pList->pHead != pList->pTail)
    {
        PLISTITEM pNode = ListRemoveTailNode(pList);
        ListAddNodeAsHead(pList, pNode);
    }
}

void *ListRemoveItemAt(PLIST pList, uint32_t uIndex)
{
    if (uIndex >= pList->cItems)
        return NULL;

    PLISTITEM pNode = pList->pHead;
    uint32_t i = 0;
    while (pNode)
    {
        if (i == uIndex)
        {
            void *pvData = pNode->pvData;
            ListRemoveNode(pList, pNode, true);
            return pvData;
        }
        pNode = pNode->pNext;
        ++i;
    }
    return NULL;
}

void *ListHead(PLIST pList)
{
    PLISTITEM pNode = pList->pHead;
    if (pNode)
        return pNode->pvData;
    return NULL;
}


void *ListTail(PLIST pList)
{
    PLISTITEM pNode = pList->pTail;
    if (pNode)
        return pNode->pvData;
    return NULL;
}

void *ListItemAt(PLIST pList, uint32_t uIndex)
{
    if (uIndex >= pList->cItems)
        return NULL;

    PLISTITEM pNode = pList->pHead;
    uint32_t i = 0;
    while (pNode)
    {
        if (i == uIndex)
            return pNode->pvData;

        pNode = pNode->pNext;
        ++i;
    }
    return NULL;
}

void ListAppend(PLIST pList, PLIST pSrcList)
{
    PLISTITEM pNode = pSrcList->pHead;
    while (pNode)
    {
        ListAdd(pList, pNode->pvData);
        pNode = pNode->pNext;
    }
}

