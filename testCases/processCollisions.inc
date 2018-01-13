processCollisionsTestCase tcs[] = {
    {
        numPairings: 1,
        completeObjectList: {
            {
                shape: {circle: {{0,0}, 1}},
                v: {5,0},
                obj: {shapeType: SHAPE_TYPE_CIRCLE}
            },
            {
                shape: {circle: {{3,0}, 1}},
                v: {0,0},
                obj: {shapeType: SHAPE_TYPE_CIRCLE}
            },
        },
        pairingList: {{handler: testHandler1}},
        numHandlerCallsExpected: 1,
        handlerIdsExpected: {1}
    },
};
