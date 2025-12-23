#include "../header/Client.hpp"
#include <iostream>
#include <cassert>
#include <string>

void test_constructors() {
    std::cout << "Testing Constructors..." << std::endl;

    // Default Constructor
    Client c1;
    assert(c1.getFd() == -1);
    assert(c1.getIp() == "");
    assert(c1.getNickname() == "");
    assert(c1.getUsername() == "");
    assert(c1.getBuffer() == "");
    assert(c1.isRegistered() == false);
    assert(c1.isOperator() == false);

    // Parameterized Constructor
    Client c2(42, "127.0.0.1");
    assert(c2.getFd() == 42);
    assert(c2.getIp() == "127.0.0.1");
    assert(c2.getNickname() == "");

    // Copy Constructor
    c2.setNickname("test_nick");
    Client c3(c2);
    assert(c3.getFd() == 42);
    assert(c3.getIp() == "127.0.0.1");
    assert(c3.getNickname() == "test_nick");

    std::cout << "Constructors OK" << std::endl;
}

void test_setters_getters() {
    std::cout << "Testing Setters/Getters..." << std::endl;

    Client c;

    c.setNickname("User1");
    assert(c.getNickname() == "User1");

    c.setUsername("RealName");
    assert(c.getUsername() == "RealName");

    c.setRegistered(true);
    assert(c.isRegistered() == true);

    c.setOperator(true);
    assert(c.isOperator() == true);

    std::cout << "Setters/Getters OK" << std::endl;
}

void test_buffer() {
    std::cout << "Testing Buffer..." << std::endl;

    Client c;

    c.appendBuffer("Hello");
    assert(c.getBuffer() == "Hello");

    c.appendBuffer(" World");
    assert(c.getBuffer() == "Hello World");

    c.clearBuffer();
    assert(c.getBuffer() == "");

    std::cout << "Buffer OK" << std::endl;
}

void test_edge_cases() {
    std::cout << "Testing Edge Cases..." << std::endl;

    Client c;

    // Empty strings
    c.setNickname("");
    assert(c.getNickname() == "");

    // Large buffer
    std::string large(10000, 'A');
    c.appendBuffer(large);
    assert(c.getBuffer().length() == 10000);
    assert(c.getBuffer() == large);

    // Self assignment
    Client *ptr = &c;
    c = *ptr;
    assert(c.getBuffer() == large);

    std::cout << "Edge Cases OK" << std::endl;
}

void test_reply() {
    std::cout << "Testing Reply..." << std::endl;

    Client c; // fd is -1

    // This should print an error to stderr but not crash
    // We expect "Error: Failed to send message to client -1"
    c.reply("Hello");

    std::cout << "Reply OK" << std::endl;
}

int main() {
    test_constructors();
    test_setters_getters();
    test_buffer();
    test_edge_cases();
    test_reply();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
