#include "../header/Client.hpp"
#include "../header/Channel.hpp"
#include <iostream>
#include <cassert>
#include <string>

// Client test function declarations
void test_constructors();
void test_setters_getters();
void test_buffer();
void test_edge_cases();
void test_reply();

// Channel test function declarations
void test_channel_constructors();
void test_channel_setters_getters();
void test_channel_users();
void test_channel_modes();
void test_channel_actions();

// Mode test function declarations
void test_mode_handling();

int main() {
	// Client tests
    test_constructors();
    test_setters_getters();
    test_buffer();
    test_edge_cases();
    test_reply();

	// Channel tests
	test_channel_constructors();
	test_channel_setters_getters();
	test_channel_users();
	test_channel_modes();
	test_channel_actions();

	// Mode tests
	test_mode_handling();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
