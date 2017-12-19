#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

bool <pf>Cmp(const <tp> * a, const <tp> * b)
{
    return (memcmp(a, b, sizeof(*a)) == 0);
}

<pf>List * <pf>ListAdd(<pf>List * l, <tp> * val)
{
    <pf>List * nn = malloc(sizeof(*nn));
    if (!nn)
    {
        fprintf(stderr, "error allocating memory for list\n");
        exit(1);
        return l;
    }
    nn->val = val;
    nn->next = l;

    return nn;
}

<pf>List * <pf>ListRm(<pf>List * l, const <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b), <tp> ** removedVal)
{
    <pf>List * n;
    <pf>List * nPrev = NULL;
    if (removedVal)
        *removedVal = NULL;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            if (!nPrev)
            {
                <pf>List * newList = n->next;
                if (removedVal)
                    *removedVal = n->val;
                free(n);
                return newList;
            }
            nPrev->next = n->next;
            if (removedVal)
                *removedVal = n->val;
            free(n);
            return l;
        }
    }

    return l;
}

<pf>List * <pf>ListSearch(<pf>List * l, const <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b))
{
    <pf>List * n;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            return n;
        }
    }

    return NULL;
}
