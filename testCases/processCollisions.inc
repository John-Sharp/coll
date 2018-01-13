processCollisionsTestCase tcs[] = {
    {
        numPairings: 1,
        completeObjectList: {
            {
                shape: {circle: {{0,0}, 1}},
                v: {-5,0},
                obj: {shapeType: SHAPE_TYPE_CIRCLE}
            },
            {
                shape: {circle: {{3,0}, 1}},
                v: {0,0},
                obj: {shapeType: SHAPE_TYPE_CIRCLE}
            },
        },
        pairingList: {{handler: testHandler1}},
        numHandlerCallsExpected: 0,
    },
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
        pairingObjectIndicies: {0, 1},
        pairingList: {{handler: testHandler1}},
        actionList: {{r: {{-5, 0}, {3, 0}}, v: {{0, 0}, {0,0}}},},
        numHandlerCallsExpected: 1,
        handlerIdsExpected: {1},
        handlerArgumentsExpected: {
            {
                objectIndicies: {0, 1},
                t: 0.2
            },
        }
    },
    {
        numPairings: 2,
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
            {
                shape: {circle: {{10, 0}, 2}},
                v: {0,0},
                obj: {shapeType: SHAPE_TYPE_CIRCLE}
            },
        },
        pairingObjectIndicies: {0, 1, 0, 2},
        pairingList: {{handler: testHandler1}, {handler: testHandler1}},
        actionList: {
            {r: {{6, 0}, {0, 0}}, v: {{6, 0}, {0,0}}},
            {r: {{0, 10}, {0, -10}}, v: {{0, 0}, {0,0}}},
        },
        numHandlerCallsExpected: 2,
        handlerIdsExpected: {1, 1},
        handlerArgumentsExpected: {
            {
                objectIndicies: {0, 1},
                t: 0.2
            },
            {
                objectIndicies: {0, 2},
                t: 0.2 + 1/6. 
            },
        }
    },
};
