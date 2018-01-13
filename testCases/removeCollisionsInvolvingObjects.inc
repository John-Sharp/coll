removeCollisionsInvolvingObjectsTestCase tcs[] = {
    { numCollisions:3,
      pairings: {{objects: {(jcObject *)0x1, (jcObject *)0x2}, handler: NULL}, {objects: {(jcObject *)0x3, (jcObject *)0x4}, handler: NULL}, {objects: {(jcObject *)0x5, (jcObject *)0x6}, handler: NULL}},
      collisionList: {{t:0.5},{t:0.5},{t:0.5}},
      objectList: {(jcObject *)0x7, (jcObject *)0x8},
      numCollisionsRemovedExpected: 0,
      collisionsRemovedIndicies: {}
  },
    { numCollisions:3,
      pairings: {{objects: {(jcObject *)0x1, (jcObject *)0x2}, handler: NULL}, {objects: {(jcObject *)0x3, (jcObject *)0x4}, handler: NULL}, {objects: {(jcObject *)0x5, (jcObject *)0x6}, handler: NULL}},
      collisionList: {{t:0.5},{t:0.5},{t:0.5}},
      objectList: {(jcObject *)0x3, (jcObject *)0x4},
      numCollisionsRemovedExpected: 1,
      collisionsRemovedIndicies: {1}
  },
    { numCollisions:3,
      pairings: {{objects: {(jcObject *)0x1, (jcObject *)0x2}, handler: NULL}, {objects: {(jcObject *)0x3, (jcObject *)0x4}, handler: NULL}, {objects: {(jcObject *)0x5, (jcObject *)0x6}, handler: NULL}},
      collisionList: {{t:0.5},{t:0.5},{t:0.5}},
      objectList: {(jcObject *)0x4, (jcObject *)0x3},
      numCollisionsRemovedExpected: 1,
      collisionsRemovedIndicies: {1}
  },
    { numCollisions:3,
      pairings: {{objects: {(jcObject *)0x1, (jcObject *)0x2}, handler: NULL}, {objects: {(jcObject *)0x3, (jcObject *)0x4}, handler: NULL}, {objects: {(jcObject *)0x5, (jcObject *)0x6}, handler: NULL}},
      collisionList: {{t:0.5},{t:0.5},{t:0.5}},
      objectList: {(jcObject *)0x5, (jcObject *)0x6},
      numCollisionsRemovedExpected: 1,
      collisionsRemovedIndicies: {2}
  },
    { numCollisions:4,
      pairings: {{objects: {(jcObject *)0x1, (jcObject *)0x2}, handler: NULL}, {objects: {(jcObject *)0x3, (jcObject *)0x4}, handler: NULL}, {objects: {(jcObject *)0x5, (jcObject *)0x6}, handler: NULL}, {objects: {(jcObject *)0x1, (jcObject *)0x2}, handler: NULL}},
      collisionList: {{t:0.5},{t:0.5},{t:0.5}},
      objectList: {(jcObject *)0x1, (jcObject *)0x2},
      numCollisionsRemovedExpected: 2,
      collisionsRemovedIndicies: {0, 3}
  },
};
