/*  Unit test fixture for server clients
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "logic/ObjectManager.hpp"
#include "logic/GameObject.hpp"
#include "../server/Client.hpp"
#include "TestObject.hpp"

class ClientTest : public ::testing::Test {
protected:
    
public:
    void SetUp();
    void TearDown();
};
