checkCollisionTestCase tcs[] = {
    {obj1: { shape: {circle: {{0,0}, 1}}, v: {5,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     obj2: { shape: {circle: {{3,0}, 1}}, v: {0,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     tRem: 1,
     collExpected: true,
     tExpected: 0.2
    },
    {obj1: { shape: {circle: {{0,0}, 1}}, v: {5,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     obj2: { shape: {circle: {{3,0}, 1}}, v: {0,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     tRem: 0.1,
     collExpected: false,
    },
    {obj1: { shape: {circle: {{0,0}, 1}}, v: {5,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     obj2: { shape: {circle: {{3,0}, 1}}, v: {0,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     tRem: 0.2,
     collExpected: true,
     tExpected: 0.2
    },
    {obj1: { shape: {circle: {{0,0}, 1}}, v: {5,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     obj2: { shape: {circle: {{3,0}, 1}}, v: {0,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     tRem: 0.3,
     collExpected: true,
     tExpected: 0.2
    },
    {obj1: { shape: {circle: {{0,0}, 1}}, v: {2,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     obj2: { shape: {circle: {{3,0}, 1}}, v: {-3,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     tRem: 1,
     collExpected: true,
     tExpected: 0.2
    },
    {obj1: { shape: {rect: {{2,-1}, {4, 1}}}, v: {-3,0}, obj: {shapeType: SHAPE_TYPE_RECT}},
     obj2: { shape: {circle: {{0,0}, 1}}, v: {2,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     tRem: 1,
     collExpected: true,
     tExpected: 0.2
    },
    {obj1: { shape: {circle: {{0,0}, 1}}, v: {2,0}, obj: {shapeType: SHAPE_TYPE_CIRCLE}},
     obj2: { shape: {rect: {{2,-1}, {4, 1}}}, v: {-3,0}, obj: {shapeType: SHAPE_TYPE_RECT}},
     tRem: 1,
     collExpected: true,
     tExpected: 0.2
    },
};