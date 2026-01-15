#include "../header/Channel.hpp"
#include "../header/Client.hpp"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

void test_mode_handling() {
    std::cout << "Testing Mode Handling..." << std::endl;

    Channel c("#test", NULL);
    Client op(1, "ip");
    op.setNickname("op");
    c.addUser(&op);
    c.addOperator(&op);

    Client nonop(2, "ip2");
    nonop.setNickname("nonop");
    c.addUser(&nonop);

    // Test no modes - should send current modes
    // Hard to test reply without mocking, but ensure no crash
    c.mode(&op, "", std::vector<std::string>());

    // Test operator privileges
    // Non-op trying to set mode should fail (but reply is sent)
    c.mode(&nonop, "+i", std::vector<std::string>());

    // Op setting modes
    std::vector<std::string> args;
    c.mode(&op, "+i", args);
    assert(c.isInviteOnly());

    args.push_back("secret");
    c.mode(&op, "+k", args);
    assert(c.hasKey());

    args.clear();
    args.push_back("100");
    c.mode(&op, "+l", args);
    assert(c.hasLimit());

    c.mode(&op, "+t", std::vector<std::string>());
    assert(c.isTopicRestricted());

    // Test +o with user
    Client u3(3, "ip3");
    u3.setNickname("user3");
    c.addUser(&u3);

    args.clear();
    args.push_back("user3");
    c.mode(&op, "+o", args);
    assert(c.isOperator(&u3));

    // Test - modes
    c.mode(&op, "-i", std::vector<std::string>());
    assert(!c.isInviteOnly());

    c.mode(&op, "-k", std::vector<std::string>());
    assert(!c.hasKey());

    c.mode(&op, "-l", std::vector<std::string>());
    assert(!c.hasLimit());

    c.mode(&op, "-t", std::vector<std::string>());
    assert(!c.isTopicRestricted());

    c.mode(&op, "-o", args);
    assert(!c.isOperator(&u3));

    // Test unknown mode
    // Should send error reply
    c.mode(&op, "+z", std::vector<std::string>());

    // Test multiple modes in one command
    args.clear();
    args.push_back("secret");
    args.push_back("50");
    c.mode(&op, "+ktl", args);
    assert(c.hasKey());
    assert(c.isTopicRestricted());
    assert(c.hasLimit());

    // Test mixed + and -
    c.mode(&op, "+i-k", std::vector<std::string>());
    assert(c.isInviteOnly());
    assert(!c.hasKey());

    std::cout << "Mode Handling OK" << std::endl;
}
