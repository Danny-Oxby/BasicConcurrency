#include <iostream>
#include <thread>
using namespace std;

void print_nondeterminism_string(std::string message) {
    for (int i = 0; i < 10; i++) {
        cout << i << " " << message;
    }
}

void Nondeterminism() {
    //code dosn't change but the output does (message output is random)
    thread t1(print_nondeterminism_string, "A\n");
    thread t2(print_nondeterminism_string, "B\n");
    t1.join(); t2.join();
}