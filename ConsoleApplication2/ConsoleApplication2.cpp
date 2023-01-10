// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <thread>
using namespace std;

void Basic_function_no_return(string message) {
    cout << message;
}

void Creating_threads() {
    //thread name(function-pointer, arguments)
    //create threads 1 and 2
    thread thread1(Basic_function_no_return, "I am a thread\n");
    thread thread2(Basic_function_no_return, "I am another thread\n");
    //close threads 1 and 2
    thread1.join(); thread2.join();
}

void print_nondeterminism_string(std::string message) {
    for (int i = 0; i < 10; i++) {
        cout << i << " " << message;
    }
}

void Nondeterminism() {
    thread t1(print_nondeterminism_string, "A\n");
    thread t2(print_nondeterminism_string, "B\n");
    t1.join(); t2.join();
}

int main() {
    Creating_threads();
}
