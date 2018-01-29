#include "coll.h"

#include <stdbool.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct jcPairing
{
  jcObject * objects[2];
  collHandler handler;
} jcPairing;

typedef struct jcRegisteredCollHandler
{
  juint groupNums[2];
  collHandler handler;
} jcRegisteredCollHandler;

#include "listHeaders/jcObjectList.h"
#include "listHeaders/jcPairingList.h"
#include "listHeaders/jcRegisteredCollHandlerList.h"
typedef struct jcEngInternal
{
  jcObjectList * objectList;
  jcPairingList * pairingList;
  jcRegisteredCollHandlerList * registeredCollHandlerList;
} jcEngInternal;
jcEngInternal * initJcEngInternal(jcEngInternal * eng);

void processCollisions(jcEngInternal * eng);

typedef enum AXIS
{
    AXIS_X = 0,
    AXIS_Y = 1,
    NUM_AXIS = 2
} AXIS;

bool circleWithCircleCollDetect(jcircle c1, jvec v, jcircle c2, jfloat * t);
bool circleWithRectCollDetect(jcircle c, jvec v, jrect r, jfloat * t, JC_SIDE * side);
bool circleWithAxisParallelSegCollDetect(jcircle c, jvec v, jvec b, jfloat h, AXIS ax, jfloat * t);

typedef enum COLL_TYPE
{
    COLL_TYPE_NONE,
    COLL_TYPE_SMALL_SIDE,
    COLL_TYPE_LONG_SIDE,
    COLL_TYPE_INSIDE
} COLL_TYPE;

typedef struct oneDimCollObj
{
    COLL_TYPE coll_type;
    float t_start;
    float t_end;
} oneDimCollObj;

oneDimCollObj oneDCollDetect(uint32_t a, int32_t b1, int32_t b2, int32_t v);

#include "listCode/jcObjectList.c"
#include "listCode/jcPairingList.c"
#include "listCode/jcRegisteredCollHandlerList.c"

jcEngInternal * initJcEng(jcEngInternal * eng)
{
    eng->objectList = NULL;
    eng->pairingList = NULL;
    eng->registeredCollHandlerList = NULL;
    return eng;
}

jcEng * createJcEng()
{
    jcEngInternal * eng = malloc(sizeof(*eng));
    initJcEng(eng);

    return (jcEng *)eng;
}

jfloat * jObjectGetPosn(jcObject *object)
{
    if (object->shapeType == SHAPE_TYPE_CIRCLE)
    {
        return object->shape.circle->c;
    }
    else
    {
        return object->shape.rect->bl;
    }
}

void jcObjectTranslate(jcObject *object, jvec v)
{
    if (object->shapeType == SHAPE_TYPE_CIRCLE)
    {
        object->shape.circle->c[0] += v[0];
        object->shape.circle->c[1] += v[1];
    }
    else
    {
        jfloat w = object->shape.rect->tr[0] - object->shape.rect->bl[0];
        jfloat h = object->shape.rect->tr[1] - object->shape.rect->bl[1];

        object->shape.rect->bl[0] += v[0];
        object->shape.rect->bl[1] += v[1];

        object->shape.rect->tr[0] = object->shape.rect->bl[0] + w;
        object->shape.rect->tr[1] = object->shape.rect->bl[1] + h;
    }
}

bool pairingGroupNumCmp(const jcPairing * a, const jcPairing * b)
{
    if ((a->objects[0]->groupNum == b->objects[0]->groupNum)
            && (a->objects[1]->groupNum == b->objects[1]->groupNum))
        return true;

    if ((a->objects[0]->groupNum == b->objects[1]->groupNum)
            && (a->objects[1]->groupNum == b->objects[0]->groupNum))
        return true;

    return false;
}

void removePairings(jcEngInternal * eng, juint groupNum1, juint groupNum2)
{
    jcPairing * removedVal = NULL;
    jcObject testObjects[2] = {{groupNum : groupNum1}, {groupNum : groupNum2}};
    jcPairing testPairing = {{&testObjects[0], &testObjects[1]}, NULL};

    while (1)
    {
        eng->pairingList = jcPairingListRm(eng->pairingList, &testPairing, pairingGroupNumCmp, &removedVal);
        if (removedVal == NULL)
            break;
        free(removedVal);
    }
}

bool makePairing(jcEngInternal * eng, jcObject * o1, jcObject * o2, collHandler handler)
{
    jcPairing * pairing = malloc(sizeof(*pairing));
    if (!pairing)
    {
        return false;
    }
    pairing->objects[0] = o1;
    pairing->objects[1] = o2;
    pairing->handler = handler;
    eng->pairingList = jcPairingListAdd(eng->pairingList, pairing);

    return true;
}

bool constructPairings(jcEngInternal * eng, jcRegisteredCollHandler * ch)
{
    jcObjectList * objectsGroup1 = NULL;
    jcObjectList * objectsGroup2 = NULL;
    jcObjectList * ln;

    removePairings(eng, ch->groupNums[0], ch->groupNums[1]);

    for (ln = eng->objectList; ln != NULL; ln = ln->next)
    {
        if (ln->val->groupNum == ch->groupNums[0])
        {
            objectsGroup1 = jcObjectListAdd(objectsGroup1, ln->val);
            continue;
        }
        if (ln->val->groupNum == ch->groupNums[1])
        {
            objectsGroup2 = jcObjectListAdd(objectsGroup2, ln->val);
        }
    }

    // In special case of group self-interaction,
    // this step ensures we don't get double-counting
    if  (ch->groupNums[0] == ch->groupNums[1])
    {
        jcObjectList * ln2;
        for (ln = objectsGroup1; ln != NULL; ln = ln->next)
        {
            for (ln2 = ln->next; ln2 != NULL; ln2 = ln2->next)
            {
                if (!makePairing(eng, ln->val, ln2->val, ch->handler))
                {
                    // TODO destroy objectsGroup1, objectsGroup2
                    return false;
                }
            }
        }
        return true;
    }

    jcObjectList * ln2;
    for (ln = objectsGroup1; ln != NULL; ln = ln->next)
    {
        for (ln2 = objectsGroup2; ln2 != NULL; ln2 = ln2->next)
        {
            if (!makePairing(eng, ln->val, ln2->val, ch->handler))
            {
                // TODO destroy objectsGroup1, objectsGroup2
                return false;
            }
        }
    }
    return true;
}

bool registerCollHandler(jcEng * enge, juint groupNum1, juint groupNum2, collHandler handler)
{
    jcEngInternal * eng = (jcEngInternal *)enge;
    jcRegisteredCollHandler * collHandler = malloc(sizeof(*collHandler));
    if (!collHandler)
        return false;
    collHandler->groupNums[0] = groupNum1;
    collHandler->groupNums[1] = groupNum2;
    collHandler->handler = handler;

    eng->registeredCollHandlerList = jcRegisteredCollHandlerListAdd(eng->registeredCollHandlerList, collHandler);

    constructPairings(eng, collHandler);
    return true;
}

bool constructObjectPairings(jcEngInternal * enge, jcObject * ob)
{
    jcEngInternal * eng = (jcEngInternal *)enge;
    jcRegisteredCollHandlerList * ln;
    for (ln = eng->registeredCollHandlerList; ln != NULL; ln = ln->next)
    {
        unsigned int i;

        for (i = 0; i < 2; i++)
        {
            if (ln->val->groupNums[i] == ob->groupNum)
            {
                jcObjectList * pl;
                for (pl = eng->objectList; pl != NULL; pl = pl->next)
                {
                    if (pl->val->groupNum == ln->val->groupNums[(i+1)%2])
                    {
                        jcPairing * pairing = malloc(sizeof(*pairing));

                        if (!pairing)
                        {
                            return false;
                        }
                        pairing->objects[i] = ob;
                        pairing->objects[(i+1)%2] = pl->val;
                        pairing->handler = ln->val->handler;
                        eng->pairingList = jcPairingListAdd(eng->pairingList, pairing);
                    }
                }
            }
        }
    }
    return true;
}

bool registerCircle(jcEng * enge, jcircle * c, jvec * v, juint groupNum, void * owner)
{
    jcEngInternal * eng = (jcEngInternal *)enge;
    jcObject * object = malloc(sizeof(*object));

    if (!object)
        return false;

    object->shapeType = SHAPE_TYPE_CIRCLE;
    object->shape.circle = c;
    object->v = v;
    object->groupNum = groupNum;
    object->owner = owner;

    if (!constructObjectPairings(eng, object))
    {
        free(object);
        return false;
    }
    
    eng->objectList = jcObjectListAdd(eng->objectList, object);
    return true;
}

bool registerRect(jcEng * enge, jrect * r, jvec * v, juint groupNum, void * owner)
{
    jcEngInternal * eng = (jcEngInternal *)enge;
    jcObject * object = malloc(sizeof(*object));

    if (!object)
        return false;

    object->shapeType = SHAPE_TYPE_RECT;
    object->shape.rect = r;
    object->v = v;
    object->groupNum = groupNum;
    object->owner = owner;

    if (!constructObjectPairings(eng, object))
    {
        free(object);
        return false;
    }
    
    eng->objectList = jcObjectListAdd(eng->objectList, object);
    return true;
}

typedef struct collision
{
    jcPairing * pairing;
    jfloat t;
    JC_SIDE side; // TODO replace this with jvec n
    jvec deltav[2];
} collision;

enum { MAX_COLLISIONS = 40 };

void initCollisionList(collision * cl)
{
    juint i;
    for (i = 0; i < MAX_COLLISIONS; i++)
    {
        cl[i].pairing = NULL;
        cl[i].t = 2;
        cl[i].side = JC_SIDE_NONE;

        cl[i].deltav[0][0] = 0; cl[i].deltav[0][1] = 0; cl[i].deltav[1][0] = 0; cl[i].deltav[1][1] = 0;
    }
}

int clCompar(const void * a, const void * b)
{
    if (((collision *)a)->t > ((collision *)b)->t)
        return 1;
    return -1;
}

#include <stdio.h>
bool checkCollision(jcPairing *pairing, jfloat tRem, jfloat *t, JC_SIDE * side)
{
    jvec v;
    jfloat t_temp;

    // calculate velocity of object 0 relative to object 1
    v[0] = (*pairing->objects[0]->v)[0] - (*pairing->objects[1]->v)[0];
    v[1] = (*pairing->objects[0]->v)[1] - (*pairing->objects[1]->v)[1];

    if (pairing->objects[0]->shapeType == SHAPE_TYPE_CIRCLE
            && pairing->objects[1]->shapeType == SHAPE_TYPE_CIRCLE)
    {
        if (circleWithCircleCollDetect(*pairing->objects[0]->shape.circle, v, *pairing->objects[1]->shape.circle, &t_temp) &&
                t_temp >= (1-tRem))
        {
            *t = t_temp;// + (1-tRem);
            return true;
        }
        return false;
    }

    if ((pairing->objects[0]->shapeType == SHAPE_TYPE_CIRCLE
            && pairing->objects[1]->shapeType == SHAPE_TYPE_RECT))
    {
        if (circleWithRectCollDetect(*pairing->objects[0]->shape.circle, v, *pairing->objects[1]->shape.rect, &t_temp, side) &&
                t_temp > (1-tRem))
        {
            *t = t_temp;// + (1-tRem);
            return true;
        }
        return false;
    }

    if ((pairing->objects[1]->shapeType == SHAPE_TYPE_CIRCLE
            && pairing->objects[0]->shapeType == SHAPE_TYPE_RECT))
    {
        v[0] *= -1;
        v[1] *= -1;
        if (circleWithRectCollDetect(*pairing->objects[1]->shape.circle, v, *pairing->objects[0]->shape.rect, &t_temp, side) &&
                t_temp > (1-tRem))
        {
            *t = t_temp; // + (1-tRem);
            return true;
        }
        return false;
    }
    // TODO rect-rect collision
    return false;
}

void removeCollisionsInvolvingObjects(collision * collisionList, juint numCollisions, jcObject ** objectList, juint numObjects, juint * numCollisionsRemoved)
{
    *numCollisionsRemoved = 0;
    juint i;
    for (i = 0; i < numCollisions; i++)
    {
        collision * cc = &collisionList[i];

        juint j;
        for  (j = 0; j < numObjects; j++)
        {
            if (cc->pairing->objects[0] == objectList[j])
            {
                *numCollisionsRemoved += 1;
                cc->t = 2;
                break;
            }

            if (cc->pairing->objects[1] == objectList[j])
            {
                *numCollisionsRemoved += 1;
                cc->t = 2;
                break;
            }
        }
    }
}

void processCollisions(jcEngInternal * eng)
{
    jcPairingList * p;
    collision collisionList[MAX_COLLISIONS];
    initCollisionList(collisionList);
    juint num_collisions = 0;

    JC_SIDE side;

    for (p = eng->pairingList; p != NULL; p = p->next)
    {
        jcPairing * pairing = p->val;

        jfloat t;
        if (checkCollision(pairing, 1.0, &t, &side))
        {
            collisionList[num_collisions].pairing = pairing;
            collisionList[num_collisions].t = t;
            collisionList[num_collisions].side = side;
            num_collisions++;
            if (num_collisions == MAX_COLLISIONS)
            {
                fprintf(stderr, "ERROR! MAX_COLLISIONS was reached!\n");
                break;
            }
        }
    }

    while (num_collisions > 0)
    {
        qsort(collisionList, MAX_COLLISIONS, sizeof(collisionList[0]), clCompar);
        jfloat tColl = collisionList[0].t;
        jfloat tRem = 1 - collisionList[0].t; 


        jcObject * o1 = collisionList[0].pairing->objects[0];
        jcObject * o2 = collisionList[0].pairing->objects[1];

        // translate object to collision point
        jvec r = {(*o1->v)[0] * tColl, (*o1->v)[1] * tColl};
        jcObjectTranslate(o1, r);

        r[0] = (*o2->v)[0] * tColl;
        r[1] = (*o2->v)[1] * tColl;
        jcObjectTranslate(o2, r);

        jvec deltavs[2];
        collisionList[0].pairing->handler(collisionList[0].pairing->objects, collisionList[0].t, collisionList[0].side, deltavs);

        // add delta v to velocity
        jvecAdd((*o1->v), deltavs[0]);
        jvecAdd((*o2->v), deltavs[1]);

        // retard collision objects v t
        r[0] = (*o1->v)[0] * -tColl;
        r[1] = (*o1->v)[1] * -tColl;
        jcObjectTranslate(o1, r);

        r[0] = (*o2->v)[0] * -tColl;
        r[1] = (*o2->v)[1] * -tColl;
        jcObjectTranslate(o2, r);

        juint numCollisionsRemoved = 0;
        removeCollisionsInvolvingObjects(collisionList, num_collisions, collisionList[0].pairing->objects, 2, &numCollisionsRemoved);
        num_collisions -= numCollisionsRemoved;

        qsort(collisionList, MAX_COLLISIONS, sizeof(collisionList[0]), clCompar);

        for (p = eng->pairingList; p != NULL; p = p->next)
        {
            jcPairing * pairing = p->val;
            jfloat t;

            if (pairing->objects[0] == collisionList[0].pairing->objects[0]
                    || pairing->objects[0] == collisionList[0].pairing->objects[1]
                    || pairing->objects[1] == collisionList[0].pairing->objects[0]
                    || pairing->objects[1] == collisionList[0].pairing->objects[1])
            {
                // JC_SIDE side;
                if (checkCollision(pairing, tRem, &t, &side))
                {
                    collisionList[num_collisions].pairing = pairing;
                    collisionList[num_collisions].t = t;
                    collisionList[num_collisions].side = side;
                    num_collisions++;
                    if (num_collisions == MAX_COLLISIONS)
                    {
                        fprintf(stderr, "ERROR! MAX_COLLISIONS was reached!\n");
                        break;
                    }
                }
            }
        }
    }
}

void integrate(jcEngInternal * eng)
{
    jcObjectList * p;

    for (p = eng->objectList; p != NULL; p = p->next)
    {
        jcObjectTranslate(p->val, *p->val->v);
    }
}

void jcEngDoStep(jcEng * eng)
{
    jcEngInternal * engi = (jcEngInternal *)eng;

    processCollisions(engi);
    integrate(engi);
}

// use doubles here as you need more precision than floats
// for when an object is 'grazing' another
bool solveQuadratic(jdouble a, jdouble b, jdouble c, jfloat *x)
{
    jdouble d = b * b - 4 * a * c;
    if (d < 0)
    {
        return false;
    }
    if (d == 0)
    {
        x[0] = -b / (2*a);
        x[1] = x[0];
        return true;
    }

    x[0] = (-b + sqrt(d))/(2*a);
    x[1] = (-b - sqrt(d))/(2*a);

    return true;
}

jfloat * jvecAdd(jvec a, jvec b)
{
    a[0] += b[0];
    a[1] += b[1];

    return a;
}

jfloat * jvecSub(jvec a, jvec b)
{
    a[0] -= b[0];
    a[1] -= b[1];

    return a;
}

jfloat * jvecNorm(jvec a)
{
    jfloat mag = sqrtf(a[0]*a[0] + a[1]*a[1]);
    a[0] /= mag;
    a[1] /= mag;
    return a;
}

jfloat jvecDot(jvec a, jvec b)
{
    return a[0]*b[0] + a[1]*b[1];
}

bool circleLineCollDetect(jcircle ci, jvec p, jvec v, jfloat * t)
{
    jfloat a = jvecDot(v, v);
    jfloat b = 2 * (jvecDot(p, v) - jvecDot(ci.c, v));
    jfloat c = jvecDot(p, p) + jvecDot(ci.c, ci.c) - 2 * jvecDot(p, ci.c) - ci.r * ci.r;
    return solveQuadratic(a, b, c, t);
}

bool betweenPoints(jfloat a, jfloat b, jfloat c)
{
    if (a <= b && a >= c)
        return true;

    if (a >= b && a <= c)
        return true;

    return false;
}

bool getDesiredSolutionIfExtant(const jfloat * solns, jint num_solns, jfloat * t)
{
    // soln will contain the time of first collision (if
    // there was a collision)
    jfloat soln = -1;
    jint i;
    for (i = 0; i < num_solns; i++) // find the minimum value in solns
    {
        if (solns[i] >= 0 && (solns[i] < soln || soln < 0))
        {
            soln = solns[i];
        }
    }

    if (soln < 0 || soln > 1) // collision did not happen this timestep
    {
        return false;
    }

    *t = soln;
    return true;
}

bool circleWithAxisParallelSegCollDetect(jcircle c, jvec v, jvec b, jfloat h, AXIS ax, jfloat * tc)
{
    jfloat t[4]; // times when circle crosses extended line
    jfloat cp[2]; // two points where circle first touches extended line, and finishes touching it
    AXIS ax2 = (ax + 1) % NUM_AXIS;

    if (v[ax2] == 0)
        return false;

    // calculate two times when circle first crosses extended line, and next crosses it
    t[0] = (b[ax2] - c.c[ax2] - c.r) / v[ax2];
    t[1] = (b[ax2] - c.c[ax2] + c.r) / v[ax2];

    // sort them so first element is earliest time
    if (t[0] > t[1])
    {
        jfloat temp = t[0];
        t[0] = t[1];
        t[1] = temp;
    }

    // if the collision happened at a time either before or after this
    // frame there was no collision this frame
    if (t[0] > 1 || t[1] < 0)
    {
        return false;
    }

    // calculate point where extended line would first be intersected
    // by circle, and then where it would be last left by the circle
    cp[0] = c.c[ax] + t[0] * v[ax];
    cp[1] = c.c[ax] + t[1] * v[ax];

    // if the circle first intersects the extended line within the
    // boundaries of the line, and it happened in the time of the 
    // frame, then we know the time of first collision
    if (t[0] > 0 && betweenPoints(cp[0], b[ax], b[ax] + h))
    {
        *tc = t[0];
        return true;
    }

    // if reached this far then either the circle has either collided with
    // an endpoint of the line section, or hasn't hit the line segment
    // at all

    // place a test circle at the first endpoint (with radius same
    // as radius of circle) and find the times the vector of the
    // original circle's movement would intersect this test circle
    jcircle endPointCircle = {{b[0], b[1]}, c.r};
    juint num_solns = 0;
    if (circleLineCollDetect(endPointCircle, c.c, v, t))
    {
        num_solns += 2;
    }

    // do the same with the other endpoint of the line segment
    endPointCircle.c[ax] += h;
    if (circleLineCollDetect(endPointCircle, c.c, v, &t[num_solns]))
    {
        num_solns += 2;
    }

    return getDesiredSolutionIfExtant(t, num_solns, tc);
}

jfloat jvecMagSq(jvec v)
{
    return v[0]*v[0] + v[1]*v[1];
}

bool circleWithCircleCollDetect(jcircle c1, jvec v, jcircle c2, jfloat * t)
{
    // if circles are initially colliding return false, I think
    // this makes more sense than returning t=0
    jvec centreLine = {(c1.c[0] - c2.c[0]), (c1.c[1] - c2.c[1])};
    if (jvecMagSq(centreLine) <  (c1.r + c2.r) * (c1.r + c2.r))
        return false;

    jcircle c3 = {{c2.c[0], c2.c[1]}, c1.r + c2.r};
    jfloat tt[2];

    bool ret = circleLineCollDetect(c3, c1.c, v, tt);

    if (!ret)
        return ret;

    return getDesiredSolutionIfExtant(tt, 2, t);
}

bool circleWithRectStaticDetect(jcircle c, jrect r)
{
    // first off, check if circle lies entirely inside the rectangle
    jfloat lSide = c.c[0] - c.r;
    jfloat rSide = c.c[0] + c.r;
    jfloat bSide = c.c[1] - c.r;
    jfloat tSide = c.c[0] + c.r;

    if (lSide > r.bl[0] && rSide < r.tr[0] && bSide > r.bl[1] && tSide < r.tr[1])
    {
        return true;
    }

    // next, check if circle is currently intersecting any of the sides
    // left side
    if ((c.c[0] - r.bl[0]) * (c.c[0] - r.bl[0]) < c.r * c.r)
    {
        jfloat x = sqrtf(c.r * c.r - (r.bl[0] - c.c[0]) * (r.bl[0] - c.c[0]));
        jfloat intP[2] = {c.c[1] - x, c.c[1] + x}; 

        if (intP[0] > r.bl[1] && intP[0] < r.tr[1])
            return true;

        if (intP[1] > r.bl[1] && intP[0] < r.tr[1])
            return true;
    }

    // right side
    if ((c.c[0] - r.tr[0]) * (c.c[0] - r.tr[0]) < c.r * c.r)
    {
        jfloat x = sqrtf(c.r * c.r - (r.tr[0] - c.c[0]) * (r.tr[0] - c.c[0]));
        jfloat intP[2] = {c.c[1] - x, c.c[1] + x}; 

        if (intP[0] > r.bl[1] && intP[0] < r.tr[1])
            return true;

        if (intP[1] > r.bl[1] && intP[0] < r.tr[1])
            return true;
    }

    // bottom side
    if ((c.c[1] - r.bl[1]) * (c.c[1] - r.bl[1]) < c.r * c.r)
    {
        jfloat x = sqrtf(c.r * c.r - (r.bl[1] - c.c[1]) * (r.bl[1] - c.c[1]));
        jfloat intP[2] = {c.c[0] - x, c.c[0] + x}; 

        if (intP[0] > r.bl[0] && intP[0] < r.tr[0])
            return true;

        if (intP[1] > r.bl[0] && intP[0] < r.tr[0])
            return true;
    }

    // top side
    if ((c.c[1] - r.tr[1]) * (c.c[1] - r.tr[1]) < c.r * c.r)
    {
        jfloat x = sqrtf(c.r * c.r - (r.tr[1] - c.c[1]) * (r.tr[1] - c.c[1]));
        jfloat intP[2] = {c.c[0] - x, c.c[0] + x}; 

        if (intP[0] > r.bl[0] && intP[0] < r.tr[0])
            return true;

        if (intP[1] > r.bl[0] && intP[0] < r.tr[0])
            return true;
    }

    return false;
}

bool circleWithRectCollDetect(jcircle c, jvec v, jrect r, jfloat * tc, JC_SIDE * collSide)
{
    jfloat h = r.tr[1] - r.bl[1];
    jfloat w = r.tr[0] - r.bl[0];
    jfloat t = 2;
    jfloat tTemp = 2;
    JC_SIDE side = JC_SIDE_NONE;

    // don't deal with case of circle initially colliding with rectangle,
    // so return false in this case
    if (circleWithRectStaticDetect(c, r))
    {
        return false;
    }

    if (circleWithAxisParallelSegCollDetect(c, v, r.bl, h, AXIS_Y, &tTemp))
    {
        side = JC_SIDE_L;
        t = tTemp;
    }

    if (circleWithAxisParallelSegCollDetect(c, v, r.tr, -h, AXIS_Y, &tTemp))
    {
        if (tTemp < t)
        {
            side = JC_SIDE_R;
            t = tTemp;
        }
        if (tTemp == t)
        {
            side |= JC_SIDE_R;
        }
    }

    if (circleWithAxisParallelSegCollDetect(c, v, r.bl, w, AXIS_X, &tTemp))
    {
        if (tTemp < t)
        {
            side = JC_SIDE_B;
            t = tTemp;
        }
        if (tTemp == t)
        {
            side |= JC_SIDE_B;
        }
    }

    if (circleWithAxisParallelSegCollDetect(c, v, r.tr, -w, AXIS_X, &tTemp))
    {
        if (tTemp < t)
        {
            side = JC_SIDE_T;
            t = tTemp;
        }
        if (tTemp == t)
        {
            side |= JC_SIDE_T;
        }
    }

    if (side == JC_SIDE_NONE)
    {
        return false;
    }

    *tc = t;
    *collSide = side;
    return true;
}

/**
<<SMALL_SIDE>>                       <<LONG_SIDE>>
                         A
                        x-x
                 B    C     D    E
                x-x  x-x   x-x  x-x  
                         F
                    x---------x
                        ref 
                      x-----x
**/
typedef enum POS_TYPE
{
    A,
    B,
    C,
    D,
    E,
    F
} POS_TYPE;

POS_TYPE getPosType(uint32_t a, int32_t b1, int32_t b2)
{
    bool b1s = (b1 < 0);
    bool b2s = (b2 < 0);

    bool b1l = (b1 > a);
    bool b2l = (b2 > a);

    if (b1s)
    {
        if (b2s)
            return B;
        if (b2l)
            return F;
        return C;
    }

    if (b2l)
    {
        if (b1l)
            return E;
        return D;
    }

    return A;
}

oneDimCollObj oneDCollDetect(uint32_t a, int32_t b1, int32_t b2, int32_t v)
{
    POS_TYPE pt[] = {getPosType(a, b1, b2), getPosType(a, (int32_t)b1 + v, (int32_t)b2 + v)};

    if (pt[0] == B && pt[1] == B)
    {
        oneDimCollObj ret = {COLL_TYPE_NONE, -1, -1};
        return ret;
    }
    
    if (pt[0] == E && pt[1] == E)
    {
        oneDimCollObj ret = {COLL_TYPE_NONE, -1, -1};
        return ret;
    }

    if (pt[0] == B)
    {
        oneDimCollObj ret = {COLL_TYPE_SMALL_SIDE, -((float)b2) / (float)v, -1};
        return ret;
    }

    if (pt[0] == E)
    {
        oneDimCollObj ret = {COLL_TYPE_LONG_SIDE, (float)b1 / (float)v, -1};
        return ret;
    }

    oneDimCollObj ret = {COLL_TYPE_INSIDE, 999, 999};
    return ret;
}
