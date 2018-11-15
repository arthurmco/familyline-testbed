/***
    Test object for Familyline

    Copyright (C) 2016, 2017 Arthur M
***/

#include "logic/Building.hpp"
#include "objects/ObjectMacros.h"

#ifndef TEST_OBJECT_H
#define TEST_OBJECT_H

/*  A tent is a basic people storage building */
class TestObject : public familyline::logic::Building {
private:
    
public:
	
    TestObject();

/*  250 HP, 3 baseAtk, 1.0 baseArmor, 1.0 building material, 0.95 bulding
    strength, 2 units of garrison capacity */
	
    TestObject(int oid, float x, float y, float z);

    /* Called on each engine loop, when an action is performed */
    virtual void iterate();

    //TestObject(const TestObject&);
    
    //ADD_CLONE_MACRO(TestObject, NULL)

    virtual ~TestObject() {}
};



#endif /* end of include guard: TEST_OBJECT_H */
