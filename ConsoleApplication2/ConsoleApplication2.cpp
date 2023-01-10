using namespace std;

#include <iostream>
#include <thread>
#include <windows.h>
#include <chrono>

#pragma region ThreadBasics
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
#pragma endregion

#pragma region Nondeterminism
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
#pragma endregion

#pragma region RaceCondition
class counter
{
private:
    int value;

public:
    counter() { this->value = 0; }
    ~counter() { };
    void add(int increment) { this->value += increment; }
    int get() { return value; }
};

static counter racy;

void incrementer()
{ //timers make the race more obviouse
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    for (int i = 0; i < 100; i++)
    {
        racy.add(1);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

void decrementer()
{
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    for (int i = 0; i < 100; i++)
    {
        racy.add(-1);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

int Race_condition()
{
    std::thread t1(incrementer);
    std::thread t2(decrementer);

    t1.join();
    t2.join();

    std::cout << "Final value: " << racy.get() << std::endl;

    return 0;
}
#pragma endregion

int main() {
    Creating_threads();
}
