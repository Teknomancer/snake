/** @file
 * Generic double linked list, header.
 */

#ifndef LIST_H___
#define LIST_H___

#include <stdbool.h>
#include <inttypes.h>

typedef struct LISTITEM
{
    void               *pvData;
    struct LISTITEM    *pNext;
    struct LISTITEM    *pPrev;
} LISTITEM;
typedef LISTITEM *PLISTITEM;
typedef const LISTITEM *PCLISTITEM;

typedef struct LIST
{
    PLISTITEM           pHead;
    PLISTITEM           pTail;
    uint32_t            cItems;
} LIST;
typedef LIST *PLIST;
typedef const LIST *PCLIST;

void ListInit(PLIST pList);
uint32_t ListSize(PLIST pList);
bool ListIsEmpty(PLIST pList);
int ListAdd(PLIST pList, void *pvData);
void ListRemove(PLIST pList, void *pvData);
void *ListRemoveItemAt(PLIST pList, uint32_t uIndex);
void *ListItemAt(PLIST pList, uint32_t uIndex);
void ListAppend(PLIST pList, PLIST pSrcList);
void ListMoveTailToHead(PLIST pList);
void *ListTail(PLIST pList);
void *ListHead(PLIST pList);

#endif /* LIST_H___ */

