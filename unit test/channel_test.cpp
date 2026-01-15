#include "../header/Channel.hpp"
#include "../header/Client.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

void test_channel_constructors() {
    std::cout << "Testing Channel Constructors..." << std::endl;

    // Default Constructor
    Channel c1;
    assert(c1.getName() == "");
    assert(c1.getTopic() == "");
    assert(c1.getUserCount() == 0);
    assert(!c1.isInviteOnly());
    assert(!c1.isTopicRestricted());
    assert(!c1.hasKey());
    assert(!c1.hasLimit());

    // Parameterized Constructor
    Client creator(1, "127.0.0.1");
    creator.setNickname("creator");
    Channel c2("#test", &creator);
    assert(c2.getName() == "#test");
    assert(c2.getUserCount() == 1);
    assert(c2.isOperator(&creator));

    // Copy Constructor
    Channel c3(c2);
    assert(c3.getName() == "#test");
    assert(c3.getUserCount() == 1);

    std::cout << "Channel Constructors OK" << std::endl;
}

void test_channel_setters_getters() {
    std::cout << "Testing Channel Setters/Getters..." << std::endl;

    Channel c("#test", NULL);

    c.setTopic("Welcome to #test");
    assert(c.getTopic() == "Welcome to #test");

    c.setInviteOnly(true);
    assert(c.isInviteOnly());

    c.setTopicRestricted(true);
    assert(c.isTopicRestricted());

    c.setKey("secret");
    assert(c.hasKey());
    assert(c.getKey() == "secret");

    c.setLimit("50");
    assert(c.hasLimit());
    assert(c.getLimit() == "50");

    std::cout << "Channel Setters/Getters OK" << std::endl;
}

void test_channel_users() {
    std::cout << "Testing Channel User Management..." << std::endl;

    Channel c("#test", NULL);
    Client u1(1, "ip1");
    u1.setNickname("user1");
    Client u2(2, "ip2");
    u2.setNickname("user2");

    // Add users
    c.addUser(&u1);
    assert(c.hasUser(&u1));
    assert(c.getUserCount() == 1);

    c.addUser(&u2);
    assert(c.getUserCount() == 2);

    // User list
    std::string list = c.getUserList();
    assert(list.find("user1") != std::string::npos);
    assert(list.find("user2") != std::string::npos);

    // Operators
    c.addOperator(&u1);
    assert(c.isOperator(&u1));
    assert(!c.isOperator(&u2));

    c.removeOperator(&u1);
    assert(!c.isOperator(&u1));

    // Remove user
    c.removeUser(&u1);
    assert(!c.hasUser(&u1));
    assert(c.getUserCount() == 1);

    std::cout << "Channel User Management OK" << std::endl;
}

void test_channel_modes() {
    std::cout << "Testing Channel Modes..." << std::endl;

    Channel c("#test", NULL);
    Client op(1, "ip");
    op.setNickname("op");
    c.addUser(&op);
    c.addOperator(&op);

    // Test mode changes via mode method
    std::vector<std::string> args;
    args.push_back("secret");
    c.mode(&op, "+k", args);
    assert(c.hasKey());
    assert(c.getKey() == "secret");

    args.clear();
    args.push_back("50");
    c.mode(&op, "+l", args);
    assert(c.hasLimit());
    assert(c.getLimit() == "50");

    c.mode(&op, "+i", std::vector<std::string>());
    assert(c.isInviteOnly());

    c.mode(&op, "+t", std::vector<std::string>());
    assert(c.isTopicRestricted());

    // Remove modes
    c.mode(&op, "-k", std::vector<std::string>());
    assert(!c.hasKey());

    c.mode(&op, "-l", std::vector<std::string>());
    assert(!c.hasLimit());

    c.mode(&op, "-i", std::vector<std::string>());
    assert(!c.isInviteOnly());

    c.mode(&op, "-t", std::vector<std::string>());
    assert(!c.isTopicRestricted());

    // Test operator mode
    Client u2(2, "ip2");
    u2.setNickname("user2");
    c.addUser(&u2);

    args.clear();
    args.push_back("user2");
    c.mode(&op, "+o", args);
    assert(c.isOperator(&u2));

    c.mode(&op, "-o", args);
    assert(!c.isOperator(&u2));

    std::cout << "Channel Modes OK" << std::endl;
}

void test_channel_actions() {
    std::cout << "Testing Channel Actions..." << std::endl;

    Channel c("#test", NULL);
    Client op(1, "ip");
    op.setNickname("op");
    c.addUser(&op);
    c.addOperator(&op);

    Client u2(2, "ip2");
    u2.setNickname("user2");
    c.addUser(&u2);

    // Invite
    Client u3(3, "ip3");
    u3.setNickname("user3");
    c.invite(&u3);
    // Assuming invite adds to invited list, but doesn't add to users yet

    // Kick
    c.kick(&op, &u2);
    assert(!c.hasUser(&u2));

    // Change topic
    c.changeTopic(&op, "New topic");
    assert(c.getTopic() == "New topic");

    // Broadcast (hard to test without mocking, but ensure no crash)
    c.broadcast("Test message");

    std::cout << "Channel Actions OK" << std::endl;
}
