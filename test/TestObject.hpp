/***
    Test object for Tribalia

    Copyright (C) 2016 Arthur M
***/

#include "logic/Building.hpp"
#include "objects/ObjectMacros.h"

#ifndef TEST_OBJECT_H
#define TEST_OBJECT_H

/*  A tent is a basic people storage building */
    class TestObject : public Tribalia::Logic::Building {
private:
    
public:
    static const int TID = 3;
	
    TestObject() : Building(0, TID, "TestObject"){};
	
    TestObject(int oid, float x, float y, float z);

    /* Called on object initialization */
    virtual bool Initialize();

    /* Called on each engine loop, when an action is performed */
    virtual bool DoAction(void);

    ADD_CLONE_MACRO(TestObject, NULL)
};



#endif /* end of include guard: TEST_OBJECT_H */
