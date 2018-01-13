#include "coll.c"

#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

jfloat DELTA = 0.000001;

// test case
// registrations (shape type (letter), groupnum)
// coll handlers (groupnum1, groupnum2)
// expected pairings (indicies of shape1, shape2, collHandler)

typedef enum REGISTRATION_TEST_PARAM_TYPE
{
    REGISTRATION_TEST_PARAM_TYPE_CIRCLE,
    REGISTRATION_TEST_PARAM_TYPE_RECT,
    REGISTRATION_TEST_PARAM_TYPE_COLL_HANDLER,
} REGISTRATION_TEST_PARAM_TYPE;

typedef struct registrationTestParams
{
    REGISTRATION_TEST_PARAM_TYPE type;
    juint groupNum1;
    juint groupNum2;
    union
    {
        jcircle * circle;
        jrect * rect;
    } shape;
    jvec * v;
    union
    {
        collHandler handler;
        void * owner;
    } ownerHandler;
} registrationTestParams;

typedef struct expectedJcObjectParams
{
    SHAPE_TYPE shapeType;
    juint groupNum;
    void * owner;
} expectedJcObjectParams;

typedef struct expectedJcPairing
{
    jcObject objects[2];
    collHandler handler;
} expectedJcPairing;

typedef struct jcEngRegistrationsTestCase
{
    juint numRegistrations;
    registrationTestParams params[10];
    juint numExpectedObjects;
    jcObject expectedObjects[10];
    juint numExpectedCollHandlers;
    jcRegisteredCollHandler expectedCollHandlers[10];
    juint numExpectedPairings;
    expectedJcPairing expectedPairings[72];
} jcEngRegistrationsTestCase;

#define TEST_OWNER 22
#define TEST_HANDLER 23

bool jcObjectsIdentical(const jcObject * ob1, const jcObject * ob2)
{
    if (ob1->shapeType == ob2->shapeType &&
    ob1->v == ob2->v &&
    ob1->groupNum == ob2->groupNum &&
    ob1->owner == ob2->owner &&
    ((ob1->shapeType == SHAPE_TYPE_CIRCLE && ob1->shape.circle == ob2->shape.circle) || 
     (ob1->shapeType == SHAPE_TYPE_RECT && ob1->shape.rect == ob2->shape.rect)))
    {
        return true;
    }
    return false;
}

bool test_jcEngRegistrations()
{
    jcircle c;
    jrect r;
    jvec v;

    uint32_t i;
    jcEngRegistrationsTestCase tcs[] = {
        // tc 0
        {numRegistrations: 1, params: {
            {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}}
                                      },
         numExpectedObjects: 1, expectedObjects: {{shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER}},
         numExpectedCollHandlers: 0,
         numExpectedPairings: 0
        },
        // tc 1
        {numRegistrations: 1, params: {
            {type: REGISTRATION_TEST_PARAM_TYPE_RECT,  shape: {rect: &r}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}}
                                      },
         numExpectedObjects: 1, expectedObjects: {{shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER}},
         numExpectedCollHandlers: 0,
         numExpectedPairings: 0
        },
        // tc 2
        {numRegistrations: 2, params: {
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}}
            },
         numExpectedObjects: 2, expectedObjects: {
             {shapeType: SHAPE_TYPE_RECT,  shape: {rect: &r}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
             {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
            },
         numExpectedCollHandlers: 0,
         numExpectedPairings: 0
        },
        // tc 3
        {numRegistrations: 3, params: {
                {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_COLL_HANDLER, groupNum1: 1, groupNum2: 1, ownerHandler: {handler: (collHandler)TEST_HANDLER}},
            },
         numExpectedObjects: 2, expectedObjects: {
             {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
             {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
            },
         numExpectedCollHandlers: 1, expectedCollHandlers: {
             {{1, 1}, (collHandler)TEST_HANDLER},
            },
         numExpectedPairings: 1, expectedPairings: {
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
                                  },
                                  (collHandler)TEST_HANDLER
                                  }
                              }
        },
        // tc 4
        {numRegistrations: 3, params: {
                {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 2, ownerHandler: {owner: (void *)TEST_OWNER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_COLL_HANDLER, groupNum1: 1, groupNum2: 2, ownerHandler: {handler: (collHandler)TEST_HANDLER}},
            },
         numExpectedObjects: 2, expectedObjects: {
             {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
             {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)TEST_OWNER},
            },
         numExpectedCollHandlers: 1, expectedCollHandlers: {
             {{1, 2}, (collHandler)TEST_HANDLER},
            },
         numExpectedPairings: 1, expectedPairings: {
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)TEST_OWNER},
                                  },
                                  (collHandler)TEST_HANDLER
                                  }
                              }
        },
        // tc 5
        {numRegistrations: 3, params: {
                {type: REGISTRATION_TEST_PARAM_TYPE_COLL_HANDLER, groupNum1: 1, groupNum2: 2, ownerHandler: {handler: (collHandler)TEST_HANDLER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 2, ownerHandler: {owner: (void *)TEST_OWNER}},
            },
         numExpectedObjects: 2, expectedObjects: {
             {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
             {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)TEST_OWNER},
            },
         numExpectedCollHandlers: 1, expectedCollHandlers: {
             {{1, 2}, (collHandler)TEST_HANDLER},
            },
         numExpectedPairings: 1, expectedPairings: {
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)TEST_OWNER},
                                  },
                                  (collHandler)TEST_HANDLER
                                  }
                              }
        },
        // tc 6
        {numRegistrations: 3, params: {
                {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)TEST_OWNER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_COLL_HANDLER, groupNum1: 1, groupNum2: 2, ownerHandler: {handler: (collHandler)TEST_HANDLER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 2, ownerHandler: {owner: (void *)TEST_OWNER}},
            },
         numExpectedObjects: 2, expectedObjects: {
             {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
             {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)TEST_OWNER},
            },
         numExpectedCollHandlers: 1, expectedCollHandlers: {
             {{1, 2}, (collHandler)TEST_HANDLER},
            },
         numExpectedPairings: 1, expectedPairings: {
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)TEST_OWNER},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)TEST_OWNER},
                                  },
                                  (collHandler)TEST_HANDLER
                                  }
                              }
        },
        // tc 7
        {numRegistrations: 8, params: {
                {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)1}},
                {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)2}},
                {type: REGISTRATION_TEST_PARAM_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum1: 1, ownerHandler: {owner: (void *)3}},
                {type: REGISTRATION_TEST_PARAM_TYPE_COLL_HANDLER, groupNum1: 1, groupNum2: 2, ownerHandler: {handler: (collHandler)TEST_HANDLER}},
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 2, ownerHandler: {owner: (void *)4}},
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 2, ownerHandler: {owner: (void *)5}},
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 2, ownerHandler: {owner: (void *)6}},
                {type: REGISTRATION_TEST_PARAM_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum1: 2, ownerHandler: {owner: (void *)7}},
            },
         numExpectedObjects: 7, expectedObjects: {
             {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)1},
             {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)2},
             {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)3},
             {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)4},
             {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)5},
             {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)6},
             {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)7},
            },
         numExpectedCollHandlers: 1, expectedCollHandlers: {
             {{1, 2}, (collHandler)TEST_HANDLER},
            },
         numExpectedPairings: 12, expectedPairings: {
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)1},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)4},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)1},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)5},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)1},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)6},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)1},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)7},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)2},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)4},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)2},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)5},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)2},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)6},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)2},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)7},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)3},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)4},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)3},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)5},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)3},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)6},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                                  {
                                  {
                                      {shapeType: SHAPE_TYPE_CIRCLE, shape: {circle: &c}, v: &v, groupNum: 1, owner: (void *)3},
                                      {shapeType: SHAPE_TYPE_RECT, shape: {rect: &r}, v: &v, groupNum: 2, owner: (void *)7},
                                  },
                                  (collHandler)TEST_HANDLER
                                  },
                              }
        },
    };

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        jcEngRegistrationsTestCase * tc = &tcs[i];

        jcEng eng;
        initJcEng(&eng);

        uint32_t j;
        for (j = 0; j < tc->numRegistrations; j++)
        {
            registrationTestParams * param = &tc->params[j];
            switch (param->type)
            {
                case REGISTRATION_TEST_PARAM_TYPE_CIRCLE:
                    registerCircle(&eng, param->shape.circle, param->v, param->groupNum1, param->ownerHandler.owner);
                    break;
                case REGISTRATION_TEST_PARAM_TYPE_RECT:
                    registerRect(&eng, param->shape.rect, param->v, param->groupNum1, param->ownerHandler.owner);
                    break;
                case REGISTRATION_TEST_PARAM_TYPE_COLL_HANDLER:
                    registerCollHandler(&eng, param->groupNum1, param->groupNum2, param->ownerHandler.handler);
                    break;
            }
        }

        jcObjectList * p;
        juint numRegisteredObjects = 0;
        for (p = eng.objectList; p != NULL; p = p->next)
        {
            numRegisteredObjects++;
        }

        if (numRegisteredObjects != tc->numExpectedObjects)
        {
            printf("Number of objects: %u does not equal number of expected objects: %u, in test case: %u\n", numRegisteredObjects, tc->numExpectedObjects, i);
            return false;
        }


        for (j = 0; j < tc->numExpectedObjects; j++)
        {
            jcObject * expectedObject = &tc->expectedObjects[j];
            jcObjectList * pp = NULL;
            bool found = false;
            for (p = eng.objectList; p != NULL; p = p->next)
            {
                if (jcObjectsIdentical(p->val, expectedObject))
                {
                    found = true;
                    break;
                }
                pp = p;
            }
            if (!found)
            {
                printf("Failed to find expected object: %u in test case: %u\n", j, i);
                return false;
            }
            if (pp)
            {
                pp->next = p->next;
            }
        }

        jcRegisteredCollHandlerList * q; 
        juint numRegisteredCollHandlers = 0;
        for (q = eng.registeredCollHandlerList; q != NULL; q = q->next)
        {
            numRegisteredCollHandlers++;
        }

        if (numRegisteredCollHandlers != tc->numExpectedCollHandlers)
        {
            printf("Number of coll handlers: %u does not equal number of expected coll handlers: %u, in test case: %u\n", numRegisteredCollHandlers, tc->numExpectedCollHandlers, i);
            return false;
        }

        for (j = 0; j < tc->numExpectedCollHandlers; j++)
        {
            jcRegisteredCollHandler * expectedCollHandler = &tc->expectedCollHandlers[j];
            jcRegisteredCollHandlerList * qq = NULL;
            bool found = false;
            for (q = eng.registeredCollHandlerList; q != NULL; q = q->next)
            {
                if (q->val->groupNums[0] == expectedCollHandler->groupNums[0] 
                        && q->val->groupNums[1] == expectedCollHandler->groupNums[1]
                        && q->val->handler == expectedCollHandler->handler)
                {
                    found = true;
                    break;
                }
                if (q->val->groupNums[1] == expectedCollHandler->groupNums[0] 
                        && q->val->groupNums[0] == expectedCollHandler->groupNums[1]
                        && q->val->handler == expectedCollHandler->handler)
                {
                    found = true;
                    break;
                }
                qq = q;
            }

            if (found == false)
            {
                printf("Failed to find coll handler %u, in test case %u\n", j, i);
                return false;
            }

            if (qq)
            {
                qq->next = q->next;
            }
        }

        jcPairingList * r; 
        juint numPairings = 0;
        for (r = eng.pairingList; r != NULL; r = r->next)
        {
            numPairings++;
        }

        if (numPairings != tc->numExpectedPairings)
        {
            printf("Number of pairings: %u does not equal number of expected pairings: %u, in test case: %u\n", numPairings, tc->numExpectedPairings, i);
            return false;
        }

        for (j = 0; j < tc->numExpectedPairings; j++)
        {
            expectedJcPairing * expectedPairing = &tc->expectedPairings[j];
            jcPairingList * rr = NULL;
            bool found = false;
            for (r = eng.pairingList; r != NULL; r = r->next)
            {
                if (jcObjectsIdentical(r->val->objects[0], &expectedPairing->objects[0])
                        && jcObjectsIdentical(r->val->objects[1], &expectedPairing->objects[1])
                        && r->val->handler == expectedPairing->handler)
                {
                    found = true;
                    break;
                }
                if (jcObjectsIdentical(r->val->objects[1], &expectedPairing->objects[0])
                        && jcObjectsIdentical(r->val->objects[0], &expectedPairing->objects[1])
                        && r->val->handler == expectedPairing->handler)
                {
                    found = true;
                    break;
                }
                rr = r;
            }

            if (found == false)
            {
                printf("Failed to find pairing %u, in test case %u\n", j, i);
                return false;
            }

            if (rr)
            {
                rr->next = r->next;
            }
        }
    }
}

bool test_jcEngRegistrationsInit()
{
    jcEng eng;
    initJcEng(&eng);

    if (eng.objectList != NULL)
    {
        printf("ERROR! test_jcEngRegistrations: objectList not initialised to NULL\n");
        return false;
    }

    if (eng.pairingList != NULL)
    {
        printf("ERROR! test_jcEngRegistrations: pairingList not initialised to NULL\n");
        return false;
    }

    if (eng.registeredCollHandlerList != NULL)
    {
        printf("ERROR! test_jcEngRegistrations: registeredCollHandlerList not initialised to NULL\n");
        return false;
    }

    return true;
}

typedef struct completeObject
{
    union testShape
    {
        jcircle circle;
        jrect rect;
    } shape; 

    jvec v;

    jcObject obj;
} completeObject;

jcObject * fillObject(completeObject * co)
{
    jcObject * ret = &co->obj;
    ret->v = &co->v;

    switch (co->obj.shapeType)
    {
        case SHAPE_TYPE_CIRCLE:
            ret->shape.circle = &co->shape.circle;
            break;
        case SHAPE_TYPE_RECT:
            ret->shape.rect = &co->shape.rect;
            break;
    }

    return ret;
}

typedef struct checkCollisionTestCase
{
    completeObject obj1;
    completeObject obj2;
    jfloat tRem;
    bool collExpected;
    jfloat tExpected;
} checkCollisionTestCase;

bool test_checkCollision()
{
    juint i;
#include "testCases/checkCollision.inc"

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        checkCollisionTestCase * tc = &tcs[i];
        jfloat coll_time = 999;
        bool ret;
        jfloat tRecv;

        jcObject * obj1 = fillObject(&tc->obj1);
        jcObject * obj2 = fillObject(&tc->obj2);

        jcPairing testPairing = {{obj1, obj2}, NULL};

        bool collRecv;
        if ((collRecv = checkCollision(&testPairing, tc->tRem, &tRecv)) != tc->collExpected)
        {
            printf("TEST FAILED test_checkCollision(%u): checkCollision returned: %s, expected: %s\n",
                    i, collRecv ? "true" : "false", tc->collExpected ? "true" : "false");
            return false;
        }

        if (collRecv && tRecv != tc->tExpected)
        {
            printf("TEST FAILED test_checkCollision(%u): checkCollision set t: %f, expected: %f\n",
                    i, tRecv, tc->tExpected);
            return false;
        }
    }
    return true;
}

typedef struct sortCollisionListTestCase
{
    juint numCollisions;
    collision collisionList[MAX_COLLISIONS];
} sortCollisionListTestCase;

bool test_sortCollisionList()
{
    juint i;
#include "testCases/sortCollisionList.inc"

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        qsort(tcs[i].collisionList, tcs[i].numCollisions, sizeof(tcs[i].collisionList[0]), clCompar);

        juint j;
        for (j = 0; j < tcs[i].numCollisions-1; j++)
        {
           if (tcs[i].collisionList[j].t > tcs[i].collisionList[j+1].t)
           {
                printf("TEST FAILED test_sortCollisionList(%u): collisionList[%u].t: %f, collisionList[%u].t: %f\n",
                        i, j, tcs[i].collisionList[j].t, j+1, tcs[i].collisionList[j+1].t);
                return false;
           }
        }
    }
    return true;
}

typedef struct removeCollisionsInvolvingObjectsTestCase
{
    juint numCollisions;
    jcPairing pairings[MAX_COLLISIONS];
    collision collisionList[MAX_COLLISIONS];
    jcObject * objectList[2];
    juint numCollisionsRemovedExpected;
    juint collisionsRemovedIndicies[MAX_COLLISIONS];
} removeCollisionsInvolvingObjectsTestCase;


bool test_removeCollisionsInvolvingObjects()
{
    juint i;
#include "testCases/removeCollisionsInvolvingObjects.inc"

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        juint j;
        for (j = 0; j < tcs[i].numCollisions; j++)
        {
            tcs[i].collisionList[j].pairing = &tcs[i].pairings[j];
        }

        juint numCollisionsRemoved;
        removeCollisionsInvolvingObjects(tcs[i].collisionList, tcs[i].numCollisions, tcs[i].objectList, 2, &numCollisionsRemoved);

        if (numCollisionsRemoved != tcs[i].numCollisionsRemovedExpected)
        {
            printf("TEST FAILED test_removeCollisionsInvolvingObjects(%u): numCollisionsRemoved: %u, numCollisionsRemovedExpected: %u\n",
                    i, numCollisionsRemoved, tcs[i].numCollisionsRemovedExpected);
        }

        juint k = 0;
        for (j = 0; j < tcs[i].numCollisions; j++)
        {
            if (tcs[i].collisionList[j].t == 2) // this t signifies element has been removed
            {
                if (j != tcs[i].collisionsRemovedIndicies[k])
                {
                    printf("TEST FAILED test_removeCollisionsInvolvingObjects(%u): expected %u th removed index to be %u, instead was %u \n",
                            i, k, tcs[i].collisionsRemovedIndicies[k], j);
                    return false;
                }

                if (!((tcs[i].collisionList[j].pairing->objects[0] == tcs[i].objectList[0] && tcs[i].collisionList[j].pairing->objects[1] == tcs[i].objectList[1])
                            || (tcs[i].collisionList[j].pairing->objects[0] == tcs[i].objectList[1] && tcs[i].collisionList[j].pairing->objects[1] == tcs[i].objectList[0])))
                {
                    printf("TEST FAILED test_removeCollisionsInvolvingObjects(%u): collision removed without objects matching, collision obj1: %p, collision obj2: %p; obj1: %p, obj2: %p\n",
                            i, tcs[i].collisionList[j].pairing->objects[0], tcs[i].collisionList[j].pairing->objects[1], tcs[i].objectList[0], tcs[i].objectList[1]);
                    return false;
                }
                k++;
            }
            else if ((tcs[i].collisionList[j].pairing->objects[0] == tcs[i].objectList[0] && tcs[i].collisionList[j].pairing->objects[1] == tcs[i].objectList[1])
                            || (tcs[i].collisionList[j].pairing->objects[0] == tcs[i].objectList[1] && tcs[i].collisionList[j].pairing->objects[1] == tcs[i].objectList[0]))
            {
                    printf("TEST FAILED test_removeCollisionsInvolvingObjects(%u): collision not removed with objects matching, collision obj1: %p, collision obj2: %p; obj1: %p, obj2: %p\n",
                            i, tcs[i].collisionList[j].pairing->objects[0], tcs[i].collisionList[j].pairing->objects[1], tcs[i].objectList[0], tcs[i].objectList[1]);
                    return false;
            }
        }

        if (k != tcs[i].numCollisionsRemovedExpected)
        {
            printf("TEST FAILED test_removeCollisionsInvolvingObjects(%u): number of colllisions actually removed %u, expected %u\n",
                    i, k, tcs[i].numCollisionsRemovedExpected); 
            return false;
        }
    }
}

// TODO test_processCollisions 
// * testcase: list of pairings, list of id's of test handlers that should be called
// * make the completePairings into a pairing list and put this on a test engine
// * test handlers resolve a collision in a way determined by test
// * call `processCollisions` on this test engine
// * test handlers put their index on a special `testHandlerLedger`, use function `registerHandlerFired(id)`
// * check the ledger against what is expected
// * check that the handlers are passed the correct arguments

typedef struct testHandlerArgumentsExpected
{
    juint objectIndicies[2];
    jfloat t;
} testHandlerArgumentsExpected;

typedef struct testHandlerArgumentsReceived
{
    jcObject * objects[2];
    jfloat t;
} testHandlerArgumentsReceived;

typedef struct processCollisionsTestCase
{
    juint numPairings;
    completeObject completeObjectList[20];
    jcPairing pairingList[10];

    juint numHandlerCallsExpected;
    juint handlerIdsExpected[10];
    testHandlerArgumentsExpected handlerArgumentsExpected[10];
} processCollisionsTestCase;

typedef struct testHandlerContext
{
    juint numHandlersCalled;
    juint handlerIndicies[10];
    testHandlerArgumentsReceived handlerArgumentsReceived[10];
} testHandlerContext;

jcPairing * fillPairing(jcPairing * p, completeObject * objs, void * ctx)
{
    p->objects[0] = fillObject(&objs[0]);
    p->objects[0]->owner = ctx;
    p->objects[1] = fillObject(&objs[1]);
    p->objects[1]->owner = ctx;

    return p;
}

void testHandler1(jcObject ** objects, jfloat t)
{
    testHandlerContext * ctx = objects[0]->owner;
    ctx->handlerIndicies[ctx->numHandlersCalled] = 1;
    ctx->handlerArgumentsReceived[ctx->numHandlersCalled].objects[0] = objects[0];
    ctx->handlerArgumentsReceived[ctx->numHandlersCalled].objects[1] = objects[1];
    ctx->handlerArgumentsReceived[ctx->numHandlersCalled].t = t;
    ctx->numHandlersCalled++;

    objects[0]->shape.circle->c[0] = -5;

}

bool test_processCollisions()
{
    juint i;
#include "testCases/processCollisions.inc"

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        jcPairingList * p = NULL;
        testHandlerContext context;
        context.numHandlersCalled = 0;

        juint j;
        for (j = 0; j < tcs[i].numPairings; j++)
        {
            p = jcPairingListAdd(p, fillPairing(&(tcs[i].pairingList[j]), &(tcs[i].completeObjectList[2*j]), &context));
        }

        jcEng testEng = {pairingList:p};
        processCollisions(&testEng);

        if (context.numHandlersCalled != tcs[i].numHandlerCallsExpected)
        {
            printf("TEST FAILED test_processCollisions(%u): number of handlers  actually called %u, expected %u\n",
                    i, context.numHandlersCalled, tcs[i].numHandlerCallsExpected);
            return false;
        }

        for (j = 0; j < context.numHandlersCalled; j++)
        {
            if (context.handlerIndicies[j] != tcs[i].handlerIdsExpected[j])
            {
                printf("TEST FAILED test_processCollisions(%u), %u th handler called: got handler with index %u, expected %u\n",
                        i, j, context.handlerIndicies[j], tcs[i].handlerIdsExpected[j]);
                return false;
            }

            testHandlerArgumentsExpected * expectedArguments = &tcs[i].handlerArgumentsExpected[j];
            testHandlerArgumentsReceived * receivedArguments = &context.handlerArgumentsReceived[j];

            if (receivedArguments->objects[0] != &tcs[i].completeObjectList[expectedArguments->objectIndicies[0]].obj)
            {
                printf("TEST FAILED test_processCollisions(%u), when %u th handler (id: %u) called: didn't get expected object[0] index: %u\n",
                        i, j, context.handlerIndicies[j], expectedArguments->objectIndicies[0]);
            }

            if (receivedArguments->objects[1] != &tcs[i].completeObjectList[expectedArguments->objectIndicies[1]].obj)
            {
                printf("TEST FAILED test_processCollisions(%u), when %u th handler (id: %u) called: didn't get expected object[1] index: %u\n",
                        i, j, context.handlerIndicies[j], expectedArguments->objectIndicies[1]);
            }

            if (receivedArguments->t != expectedArguments->t)
            {
                printf("TEST FAILED test_processCollisions(%u), when %u th handler (id: %u) called: didn't get expected t: %f, got: %f\n",
                        i, j, context.handlerIndicies[j], expectedArguments->t, receivedArguments->t);
            }
        }
    }

    return true;
}

typedef struct circleWithCircleCollDetectTestCase
{
    jcircle c1;
    jvec v;
    jcircle c2;
    bool ret_expected;
    jfloat t_expected;
} circleWithCircleCollDetectTestCase;

bool test_circleWithCircleCollDetect()
{
    uint32_t i;
    circleWithCircleCollDetectTestCase tcs[] = {
        {{{0,0}, 1}, {5, 0}, {{3,0}, 1}, true, 0.2},
        {{{0,0}, 2}, {6, 0}, {{4,0}, 1}, true, 1/6.},
        {{{0,0}, 1}, {6, 0}, {{4,0}, 2}, true, 1/6.},
    };

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        circleWithCircleCollDetectTestCase * tc = &tcs[i];
        jfloat coll_time = 999;
        bool ret;

        ret = circleWithCircleCollDetect(tc->c1, tc->v, tc->c2, &coll_time);

        if (ret != tc->ret_expected || (ret && ((coll_time - tc->t_expected) * (coll_time - tc->t_expected) > DELTA * DELTA)))
        {
            printf("TEST FAILED: coll detected: %s coll time returned: %f coll time expected: %f\n\n", ret ? "T" : "F", coll_time, tc->t_expected);
            return false;
        }
    }
    return true;
}

typedef struct circleWithRectCollDetectTestCase
{
    jcircle c;
    jvec v;
    jrect r;
    bool ret_expected;
    jfloat t_expected;
} circleWithRectCollDetectTestCase;

bool test_circleWithRectCollDetect()
{
    uint32_t i;
    circleWithRectCollDetectTestCase tcs[] = {
        {{{0,0}, 1}, {5, 0}, {{2, -1}, {4, 1}}, true, 0.2},
        {{{0,0}, 1}, {0, -5}, {{-1, -4}, {1, -2}}, true, 0.2},
        {{{0,0}, 1}, {-5, 0}, {{-4, -1}, {-2, 1}}, true, 0.2},
        {{{0,0}, 1}, {0, 5}, {{-1, 2}, {1, 4}}, true, 0.2},
        {{{0,0}, 1}, {4, -4}, {{1, -3}, {3, -1}}, true, 0.073223},
        {{{3,3}, 1}, {5, 0}, {{5, 2}, {7, 4}}, true, 0.2},
        {{{0,0}, 2}, {3, 0}, {{2, -3}, {4, -1}}, true, 0.089316},
    };

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        circleWithRectCollDetectTestCase * tc = &tcs[i];
        jfloat coll_time = 999;
        bool ret;

        ret = circleWithRectCollDetect(tc->c, tc->v, tc->r, &coll_time);

        if (ret != tc->ret_expected || (ret && ((coll_time - tc->t_expected) * (coll_time - tc->t_expected) > DELTA * DELTA)))
        {
            printf("TEST FAILED: coll detected: %s coll time returned: %f coll time expected: %f\n\n", ret ? "T" : "F", coll_time, tc->t_expected);
            return false;
        }
    }
    return true;
}

typedef struct circleWithAxisParallelSegCollDetectTestCase
{
    jcircle c;
    jvec v;
    jvec b;
    jfloat h;
    AXIS ax;
    bool   ret_expected;
    jfloat tc_expected;
} circleWithAxisParallelSegCollDetectTestCase;

bool test_circleWithAxisParallelSegCollDetect()
{
    uint32_t i;
    circleWithAxisParallelSegCollDetectTestCase tcs[] = {
        {{{0, 0}, 1}, {4, 0}, {2, -2}, 4, AXIS_Y, true, 0.25},
        {{{0, 0}, 1}, {-4, 0}, {2, -2}, 4, AXIS_Y, false, 0.0},
        {{{0, 0}, 1}, {-4, 0}, {-2, -2}, 4, AXIS_Y, true, 0.25},
        {{{0, 0}, 1}, {0, 4}, {-2, 2}, 4, AXIS_X, true, 0.25},
        {{{0, 0}, 2}, {4, 0}, {3, -5}, 4, AXIS_Y, true, 0.3169872981077807},
        {{{6, 7}, 2}, {4, 0}, {9, 2}, 4, AXIS_Y, true, 0.3169872981077807},
        {{{0, 0}, 2}, {0, 4}, {-5, 3}, 4, AXIS_X, true, 0.3169872981077807},
        {{{0, 0}, 4}, {8, 0}, {6, -10}, 8, AXIS_Y, true, 0.3169872981077807},
        {{{0, 0}, 4}, {8, 0}, {6, -3}, 1, AXIS_Y, true, 0.3169872981077807},
        {{{0, 0}, 1}, {4, 0}, {2, 2}, -4, AXIS_Y, true, 0.25},
        {{{0, 0}, 2}, {4, 0}, {3, -1}, -4, AXIS_Y, true, 0.3169872981077807},
    };

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        circleWithAxisParallelSegCollDetectTestCase * tc = &tcs[i];
        jfloat coll_time = 999;
        bool ret;

        ret = circleWithAxisParallelSegCollDetect(tc->c, tc->v, tc->b, tc->h, tc->ax, &coll_time);

        if (ret != tc->ret_expected || (ret && coll_time != tc->tc_expected))
        {
            printf("%s %f %f\n\n", ret ? "T" : "F", coll_time, tc->tc_expected);
            return false;
        }
    }
    return true;

}


typedef struct getPosTypeTestCase
{
    uint32_t a;
    int32_t b1;
    int32_t b2;
    POS_TYPE expected;
} getPosTypeTestCase;


bool test_getPosType()
{
    uint32_t i;
    getPosTypeTestCase tcs[] = {
        {5, -10, -9, B},
        {5, -10,  2, C},
        {5, -10,  6, F},
        {5,   1,  2, A},
        {5,   6,  7, E},
        {5,   3,  6, D},
        {0,  -2,  2, F},
        {0,   0,  2, D},
        {0,  -2,  0, C},
        {0,   0,  0, A}
    };

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        getPosTypeTestCase * tc = &tcs[i];
        if (getPosType(tc->a, tc->b1, tc->b2) != tc->expected)
        {
            fprintf(stderr, "getPosTypeTestCase(%u): getPosType(%u, %d, %d) != %u\n\n",
                    i, tc->a, tc->b1, tc->b2, tc->expected);
            return false;
        }
    }
    return true;
}

typedef struct oneDCollDetectTestCase
{
    uint32_t a;
    uint32_t b1;
    uint32_t b2;
    int32_t v;
    oneDimCollObj expected;
} oneDCollDetectTestCase;

const char * pp_collType(COLL_TYPE ct)
{
    switch (ct) {
        case COLL_TYPE_NONE:
            return "COLL_TYPE_NONE";
        case COLL_TYPE_SMALL_SIDE:
            return "COLL_TYPE_SMALL_SIDE";
        case COLL_TYPE_LONG_SIDE:
            return "COLL_TYPE_LONG_SIDE";
        case COLL_TYPE_INSIDE:
            return "COLL_TYPE_INSIDE";
    }
}

const char * pp_oneDimCollObj(oneDimCollObj * obj, char * buffer, size_t buflen)
{
    snprintf(buffer, buflen, "{coll_type: %s, t_start: %f, t_end: %f}",
            pp_collType(obj->coll_type),
            obj->t_start,
            obj->t_end);

    return buffer;
}

bool test_oneDCollDetect()
{
    uint32_t i;
    oneDCollDetectTestCase tcs[] = {
        {5, -10, -5,  1, {COLL_TYPE_NONE, -1, -1}},
        {5,  10, 15,  1, {COLL_TYPE_NONE, -1, -1}},
        {0,  10, 15,  1, {COLL_TYPE_NONE, -1, -1}},
        {0,  10, 10,  0, {COLL_TYPE_NONE, -1, -1}},
        {5, -10, -5,  6, {COLL_TYPE_SMALL_SIDE, 5./6., -1}},   // B -> C
        {5, -10, -5,  5, {COLL_TYPE_SMALL_SIDE, 1, -1}},       // B -> C
        {5, -10, -5, 16, {COLL_TYPE_SMALL_SIDE, 5./16., -1}},  // B -> E
        {5, -10, -8, 11, {COLL_TYPE_SMALL_SIDE, 8./11., -1}},  // B -> A
        {5, -10, -8, 14, {COLL_TYPE_SMALL_SIDE, 8./14., -1}},  // B -> D
        {2, -10, -5, 10, {COLL_TYPE_SMALL_SIDE, 5./10., -1}},  // B -> F
    };

    for (i = 0; i < ARRAY_SIZE(tcs); i++)
    {
        oneDCollDetectTestCase * tc = &tcs[i];
        
        oneDimCollObj res = oneDCollDetect(tc->a, tc->b1, tc->b2, tc->v);
        if (memcmp(&res, &tc->expected, sizeof(res))!=0)
        {
            char buff[512];
            char buff2[512];
            fprintf(stderr, "oneDCollDetectTestCase(%u): oneDCollDetect(%u, %d, %d, %d) = %s != %s\n\n",
                    i, tc->a, tc->b1, tc->b2, tc->v,
                    pp_oneDimCollObj(&res, buff, sizeof(buff)),
                    pp_oneDimCollObj(&tc->expected, buff2, sizeof(buff2)));
            return false;
        }
    }
    return true;
}

int main()
{
    test_getPosType();
    test_oneDCollDetect();
    test_circleWithAxisParallelSegCollDetect();
    test_circleWithRectCollDetect();
    test_circleWithCircleCollDetect();
    test_jcEngRegistrationsInit();
    test_jcEngRegistrations();
    test_checkCollision();
    test_sortCollisionList();
    test_removeCollisionsInvolvingObjects();
    test_processCollisions();
}
