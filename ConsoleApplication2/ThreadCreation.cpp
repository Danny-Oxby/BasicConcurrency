#include <iostream>
#include <thread>
using namespace std;

void basic_function_no_return(string message) {
    cout << message;
}

void Creating_threads() {
    //thread name(function-pointer, arguments)
    //create threads 1 and 2
    thread thread1(basic_function_no_return, "I am a thread\n");
    thread thread2(basic_function_no_return, "I am another thread\n");
    //close threads 1 and 2
    thread1.join(); thread2.join();
}