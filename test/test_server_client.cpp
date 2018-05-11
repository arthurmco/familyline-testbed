#include "test_server_client.hpp"

using namespace Familyline::Server;

void ClientTest::SetUp()
{
 
}

void ClientTest::TearDown()
{
}

TEST_F(ClientTest, TestMessagePushSimple) {

    struct in_addr a = {};
    Client* c = new Client(0, a);

    c->GetQueue()->InjectMessageTCP("Hello ", 6);
    c->GetQueue()->InjectMessageTCP("world ", 6);
    c->SetCheckHeaders(false);

    char nc[13];
    memset(&nc, 0, 13);

    c->GetQueue()->ReceiveTCP(nc, 12);
    EXPECT_EQ(0, strcmp(nc, "Hello world ")) << "Message push fail: '" << nc << "' not equals 'Hello world '";


    c->GetQueue()->InjectMessageTCP("fags ", 5);
    memset(&nc, 0, 13);

    c->GetQueue()->ReceiveTCP(nc, 5);
    EXPECT_EQ(0, strcmp(nc, "fags ")) << "Message not flushed";

    delete c;	
}

TEST_F(ClientTest, TestMessagePushComplex) {

    struct in_addr a = {};
    Client* c = new Client(0, a);

    c->GetQueue()->InjectMessageTCP("Hello ", 6);
    c->GetQueue()->InjectMessageTCP("world ", 6);
    c->SetCheckHeaders(false);
    
    char nc[13];
    memset(&nc, 0, 13);

    c->GetQueue()->ReceiveTCP(nc, 6);
    EXPECT_EQ(0, strcmp(nc, "Hello ")) << "Message push fail: '" << nc << "' not equals 'Hello '";

    c->GetQueue()->InjectMessageTCP("fags ", 5);
    memset(&nc, 0, 13);

    c->GetQueue()->ReceiveTCP(nc, 5);
    EXPECT_EQ(0, strcmp(nc, "world")) << "Message not flushed '"<< nc << "' != 'world'";

    memset(&nc, 0, 13);
    c->GetQueue()->ReceiveTCP(nc, 99);
    EXPECT_EQ(0, strcmp(nc, " fags ")) << "Message not flushed '"<< nc << "' != ' fags '";

    delete c;
}

TEST_F(ClientTest, TestIfReceiveReturnFalseNoMessage) {
    struct in_addr a = {};
    Client* c = new Client(0, a);

    c->GetQueue()->InjectMessageTCP("Hello ", 6);
    c->SetCheckHeaders(false);
    
    char nc[13];
    memset(&nc, 0, 13);
    EXPECT_TRUE(c->GetQueue()->ReceiveTCP(nc, 6));
    EXPECT_FALSE(c->GetQueue()->ReceiveTCP(nc, 6));

    delete c;
}

TEST_F(ClientTest, TestIfReceiveTokenCorrect) {
    struct in_addr a = {};
    Client* c = new Client(0, a);
    c->SetCheckHeaders(false);
    
    c->GetQueue()->InjectMessageTCP("[ I AM TOKEN 1][I AM TOKEN 2]", 29);

    char nc[26];
    c->GetQueue()->ReceiveTCP(nc, 20);
    EXPECT_EQ(0, strcmp(nc, "[ I AM TOKEN 1]")) << "Message not flushed '" << nc << "' != '[ I AM TOKEN 1]";

    memset(&nc, 0, 20);
    c->GetQueue()->ReceiveTCP(nc, 20);
    EXPECT_EQ(0, strcmp(nc, "[I AM TOKEN 2]")) << "Message not flushed '" << nc << "' != '[I AM TOKEN 2]";

    delete c;
}

TEST_F(ClientTest, TestHeaderCheckCorrect) {
    struct in_addr a = {};
    Client* c = new Client(0, a);
    c->GetQueue()->SetCheckHeaders(true);
    
    c->GetQueue()->InjectMessageTCP("[TRIBALIA H1]%%%!!!@@@[TRIBALIA H2]", 35);
    

    char nc[26];
    c->GetQueue()->ReceiveTCP(nc, 20);
    EXPECT_EQ(0, strcmp(nc, "[TRIBALIA H1]")) << "Message not flushed '" << nc << "' != '[TRIBALIA H1]";

    memset(&nc, 0, 20);
    c->GetQueue()->ReceiveTCP(nc, 15);
    EXPECT_EQ(0, strcmp(nc, "[TRIBALIA H2]")) << "Message not flushed '" << nc << "' != '[TRIBALIA H2]";

    delete c;
}

