#include "test_server_client.hpp"

using namespace Tribalia::Server;

void ClientTest::SetUp()
{
 
}

void ClientTest::TearDown()
{
}

TEST_F(ClientTest, TestMessagePushSimple) {

    struct in_addr a;
    Client* c = new Client(0, a);

    c->InjectMessage("Hello ", 6);
    c->InjectMessage("world ", 6);

    char nc[13];
    memset(&nc, 0, 13);

    c->Receive(nc, 12);
    EXPECT_EQ(0, strcmp(nc, "Hello world ")) << "Message push fail: '" << nc << "' not equals 'Hello world '";


    c->InjectMessage("fags ", 5);
    memset(&nc, 0, 13);

    c->Receive(nc, 5);
    EXPECT_EQ(0, strcmp(nc, "fags ")) << "Message not flushed";
}

TEST_F(ClientTest, TestMessagePushComplex) {

    struct in_addr a;
    Client* c = new Client(0, a);

    c->InjectMessage("Hello ", 6);
    c->InjectMessage("world ", 6);

    char nc[13];
    memset(&nc, 0, 13);

    c->Receive(nc, 6);
    EXPECT_EQ(0, strcmp(nc, "Hello ")) << "Message push fail: '" << nc << "' not equals 'Hello '";

    c->InjectMessage("fags ", 5);
    memset(&nc, 0, 13);

    c->Receive(nc, 5);
    EXPECT_EQ(0, strcmp(nc, "world")) << "Message not flushed '"<< nc << "' != 'world'";

    memset(&nc, 0, 13);
    c->Receive(nc, 99);
    EXPECT_EQ(0, strcmp(nc, " fags ")) << "Message not flushed '"<< nc << "' != ' fags '";

}

TEST_F(ClientTest, TestIfReceiveReturnFalseNoMessage) {
    struct in_addr a;
    Client* c = new Client(0, a);

    c->InjectMessage("Hello ", 6);

    char nc[13];
    memset(&nc, 0, 13);
    EXPECT_TRUE(c->Receive(nc, 6));
    EXPECT_FALSE(c->Receive(nc, 6));

}
