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

    c->InjectMessageTCP("Hello ", 6);
    c->InjectMessageTCP("world ", 6);
    c->SetCheckHeaders(false);

    char nc[13];
    memset(&nc, 0, 13);

    c->ReceiveTCP(nc, 12);
    EXPECT_EQ(0, strcmp(nc, "Hello world ")) << "Message push fail: '" << nc << "' not equals 'Hello world '";


    c->InjectMessageTCP("fags ", 5);
    memset(&nc, 0, 13);

    c->ReceiveTCP(nc, 5);
    EXPECT_EQ(0, strcmp(nc, "fags ")) << "Message not flushed";
}

TEST_F(ClientTest, TestMessagePushComplex) {

    struct in_addr a;
    Client* c = new Client(0, a);

    c->InjectMessageTCP("Hello ", 6);
    c->InjectMessageTCP("world ", 6);
    c->SetCheckHeaders(false);
    
    char nc[13];
    memset(&nc, 0, 13);

    c->ReceiveTCP(nc, 6);
    EXPECT_EQ(0, strcmp(nc, "Hello ")) << "Message push fail: '" << nc << "' not equals 'Hello '";

    c->InjectMessageTCP("fags ", 5);
    memset(&nc, 0, 13);

    c->ReceiveTCP(nc, 5);
    EXPECT_EQ(0, strcmp(nc, "world")) << "Message not flushed '"<< nc << "' != 'world'";

    memset(&nc, 0, 13);
    c->ReceiveTCP(nc, 99);
    EXPECT_EQ(0, strcmp(nc, " fags ")) << "Message not flushed '"<< nc << "' != ' fags '";

}

TEST_F(ClientTest, TestIfReceiveReturnFalseNoMessage) {
    struct in_addr a = {};
    Client* c = new Client(0, a);

    c->InjectMessageTCP("Hello ", 6);
    c->SetCheckHeaders(false);
    
    char nc[13];
    memset(&nc, 0, 13);
    EXPECT_TRUE(c->ReceiveTCP(nc, 6));
    EXPECT_FALSE(c->ReceiveTCP(nc, 6));

}

TEST_F(ClientTest, TestIfReceiveTokenCorrect) {
    struct in_addr a = {};
    Client* c = new Client(0, a);
    c->SetCheckHeaders(false);
    
    c->InjectMessageTCP("[ I AM TOKEN 1][I AM TOKEN 2]", 29);

    char nc[26];
    c->ReceiveTCP(nc, 20);
    EXPECT_EQ(0, strcmp(nc, "[ I AM TOKEN 1]")) << "Message not flushed '" << nc << "' != '[ I AM TOKEN 1]";

    memset(&nc, 0, 20);
    c->ReceiveTCP(nc, 20);
    EXPECT_EQ(0, strcmp(nc, "[I AM TOKEN 2]")) << "Message not flushed '" << nc << "' != '[I AM TOKEN 2]";

}
