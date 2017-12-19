#include <stdbool.h>

// typedef struct <pf>List <pf>List;
typedef struct <pf>List <pf>List;
struct <pf>List {
    <tp> * val;
    <pf>List * next;    
};

bool <pf>Cmp(const <tp> * a, const <tp> * b);

// <pf>ListAdd: adds element to head of list
<pf>List * <pf>ListAdd(<pf>List * l, <tp> * val);

// <pf>ListRmremove: removes element from list
<pf>List * <pf>ListRm(<pf>List * l, const <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b), <tp> ** removedVal);

<pf>List * <pf>ListSearch(<pf>List * l, const <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b));
