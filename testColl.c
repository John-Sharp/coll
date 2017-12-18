#include "coll.c"

#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

jfloat DELTA = 0.000001;

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
}
