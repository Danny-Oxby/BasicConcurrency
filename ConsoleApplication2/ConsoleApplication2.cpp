using namespace std;

#include <iostream>
#include <sstream>
#include <thread>
#include <windows.h>
#include <chrono>
#include <mutex>
#include <future>
#include "ThreadPool.hpp"
#include "barrier.hpp"

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

#pragma region Concurrency
void edit_number(int a) {
    cout << a * a; //do x
}
void edit_string(string a) {
    cout << a; //do y
}
void example_Concurrency() {
    thread t1(edit_number, 1); //create thread
    thread t2(edit_string, "B\n"); //create thread
    t1.join(); t2.join();
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

#pragma region Normal Mutex
int external_data = 0;
mutex mutex_lock;
void data_manipulation_method(int i) {
    mutex_lock.lock(); //lock the criticla section
    external_data =+ i;
    cout << external_data << "\n";
    mutex_lock.unlock(); //unlock the section
}

void base_mutex() {
    thread t1(data_manipulation_method, 1); //create thread
    thread t2(data_manipulation_method, -1); //create thread
    t1.join(); t2.join();
}
#pragma endregion

#pragma region BasicMutex (solving race condition issue)
class mutexcounter
{
private:
    int value;
    mutex lock;

public:
    mutexcounter() { this->value = 0; }
    ~mutexcounter() { };
    void add(int increment) {
        lock.lock();
        this->value += increment;
        lock.unlock();
    }
    int get() { return value; }
};

static mutexcounter racymutex;

void mutexincrementer()
{ //timers make the race more obviouse
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    for (int i = 0; i < 100; i++)
    {
        racymutex.add(1);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

void mutexdecrementer()
{
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    for (int i = 0; i < 100; i++)
    {
        racymutex.add(-1);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

int mutex_race_condition()
{
    std::thread t1(mutexincrementer);
    std::thread t2(mutexdecrementer);

    t1.join(); t2.join();

    std::cout << "Final value: " << racymutex.get() << std::endl;

    return 0;
}
#pragma endregion

#pragma region AccurateTimer plus Amdahls law Example
double PCFreq = 0.0; // convert the requeency to double for the maths
__int64 CounterStart = 0;
LARGE_INTEGER liStart;
LARGE_INTEGER liEnd;

void StartCounter()
{
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
        cout << "QueryPerformanceFrequency failed!\n";

    //find the frequency before starting the timer
    PCFreq = double(li.QuadPart) / 1000.0; // quadpart/ 1000.0 to find milliseconds
    QueryPerformanceCounter(&liStart); //get the start time
}

double GetCounter()
{
    QueryPerformanceCounter(&liEnd); //get the end time
    return double(liEnd.QuadPart - liStart.QuadPart) / PCFreq;
}

int Query_timer()
{
    StartCounter();
    Sleep(1000);
    cout << GetCounter() << "\n";
    return 0;
}

void FindingTValuesUsingCounter() { // for Amdahl?s law
    double localPCFreq = 0.0; // convert the requeency to double for the maths
    __int64 localCounterStart = 0;
    LARGE_INTEGER Start; LARGE_INTEGER EnhanceStart;
    LARGE_INTEGER EnhanceEnd; LARGE_INTEGER End;

    LARGE_INTEGER frequency; //find the frequency before starting the timer
    if (!QueryPerformanceFrequency(&frequency))
        cout << "QueryPerformanceFrequency failed!\n";
    PCFreq = double(frequency.QuadPart) / 1000.0; // quadpart/ 1000.0 to find milliseconds

    QueryPerformanceCounter(&Start); //get the start time for t1
    cout << "I don't change\n";// Do T1 things
    Sleep(10); // simluating code actions
    QueryPerformanceCounter(&EnhanceStart); //get the start time for t2 and t4
    cout << "enhance me\n";//do the enhanced code and unenhacend code timing here
    Sleep(100); // simluating code actions
    QueryPerformanceCounter(&EnhanceEnd); //get the start time for t3
    cout << "I don't change either\n";// Do T3 things
    Sleep(10); // simluating code actions
    QueryPerformanceCounter(&End); //get the end time for t3

    //print values in millisecond format
    cout << "t1 : " << double(EnhanceStart.QuadPart - Start.QuadPart) / PCFreq << "\n";
    cout << "t2/4 : " << double(EnhanceEnd.QuadPart - EnhanceStart.QuadPart) / PCFreq << "\n";
    cout << "t3 : " << double(End.QuadPart - EnhanceEnd.QuadPart) / PCFreq << "\n";
    cout << "total time : " << double(End.QuadPart - Start.QuadPart) / PCFreq << "\n";
}
#pragma endregion

void PrimeBoolOutput(bool ret)
{
    if (ret) std::cout << "313222313 is prime!\n";
    else std::cout << "313222313 is not prime.\n";
}

#pragma region Futures
// a non-optimized way of checking for prime numbers:
bool is_prime(int x)
{
    std::cout << "Calculating. Please, wait...\n";

    for (int i = 2; i < x; ++i)
        if (x % i == 0)
            return false;

    return true;
}

int Future_returns()
{
    // call is_prime(313222313) asynchronously:
    std::future<bool> fut = std::async(is_prime, 313222313);
    bool ret = fut.get();      // waits for is_prime to return

    PrimeBoolOutput(ret);

    return 0;
}


int Asynchronouse_Future_returns() // using the async future policy
{
    // call is_prime(313222313) asynchronously:
    std::future<bool> fut = std::async(std::launch::async, is_prime, 313222313);
    bool ret = fut.get();      // waits for is_prime to return

    PrimeBoolOutput(ret);

    return 0;
}
#pragma endregion

#pragma region Promises
// a non-optimized way of checking for prime numbers:
void prom_is_prime(int x, std::promise<bool>&& prom)
{
    std::cout << "Calculating. Please, wait...\n";

    for (int i = 2; i < x; ++i)
        if (x % i == 0)
        {
            prom.set_value(false);
            return; // exit early
        }

    prom.set_value(true);
}

int Promise_returns()
{
    std::promise<bool> prom;
    std::future<bool> fut = prom.get_future();

    std::thread t(prom_is_prime, 313222313, std::move(prom));
    bool ret = fut.get();

    PrimeBoolOutput(ret);

    t.join();

    return 0;
}
#pragma endregion

#pragma region Condition Variables

#define THREADCOUNT 20
int threadHelloComplete = 0;
mutex threadMutex;
condition_variable cv;

void printHello(int id)
{
    unique_lock<mutex> lock(threadMutex); // unique lock << only one thread at a time

    while (threadHelloComplete < id) // only allow the thread that meets this condition to advance
        cv.wait(lock); // all threads wait here

    cout << "Hello from thread " << id << endl;
    threadHelloComplete++; // update the counter for the condition variable
    cv.notify_all(); // wake up the remaining threads
}

int Condition_varibles()
{
    thread* printThreads[THREADCOUNT]; int i;

    for (i = 0; i < THREADCOUNT; i++)
        printThreads[i] = new thread(printHello, i);
    for (i = 0; i < THREADCOUNT; i++)
        printThreads[i]->join(); // terminat all threads

    return 0;
}

void printHello_nondeterministic_example(unsigned int id)
{
    std::cout << "Hello world from thread " << id << std::endl;
}

int Conditionless_variables()
{
    thread* printThreads[THREADCOUNT];
    unsigned int i;

    for (i = 0; i < THREADCOUNT; i++)
        printThreads[i] = new thread(printHello_nondeterministic_example, i);

    for (i = 0; i < THREADCOUNT; i++)
        printThreads[i]->join();

    return 0;
}

#pragma region mock exam example << Huffman
    #define threads 10
    mutex huffmanMutex;
    condition_variable convar;
    int first_pass = 0; int second_pass = 0;
    bool histogrambusy = false;

    void HuffmanEncoding(string filename) 
    {
        unique_lock<mutex> lock(threadMutex); // unique lock << only one thread at a time
    
        //Do first pass encoding here
        while(histogrambusy) // pnly allow one thread at a time
            cv.wait(lock); // all threads wait here

        histogrambusy = true; //update the histogram
        cout << "add to histogram\n";
        histogrambusy = false;
        cv.notify_all(); // wake up the remaining threads

        first_pass++; cout << "1st pass complete\n"; // update the condition variable
        while (first_pass < threads) // only allow the thread that meets this condition to advance
            cv.wait(lock); // all threads wait here
        cv.notify_all(); // wake up the remaining threads

        //Do second pass encoding
        second_pass++; cout << "2nd pass complete\n";//update the condition variable
        while (second_pass < threads) // only allow the thread that meets this condition to advance
            cv.wait(lock); // all threads wait here
        cv.notify_all(); // wake up the remaining threads
    }
    void historgram_example() {
        thread* huffmanThreads[threads]; //create a list of threads
        for (int i = 0; i < threads; i++)
            huffmanThreads[i] = new thread(HuffmanEncoding, "file");

        for (int i = 0; i < threads; i++)
            huffmanThreads[i]->join();
    }
#pragma endregion

#pragma endregion

#pragma region Thread Pooling
int threadTask(int i)
{
    stringstream ss; string intro; // create variables
    ss << i; ss >> intro; intro = "entered task " + intro + "\n"; // assign variable values
    cout << intro;
    this_thread::sleep_for(std::chrono::seconds(3));
    return i * i;
}

int ThreadPooling()
{
    ThreadPool pool(4); // create a thread pool of size 4
    vector<future<int>> results;

    for (int i = 0; i < 8; ++i) { // add 8 threads to the pool
        results.emplace_back(
            pool.enqueue(threadTask, i));
    }

    for (auto&& result : results)
    {
        stringstream ss; string outro; // create variables
        ss << result.get(); ss >> outro; outro = "result " + outro + "\n"; // assign variable values
        cout << outro;
    }

    return 0;
}

string threadCompressionTask(int file)
{
    //do file compression here
    cout << "file compressed\n"; return "file compressed";
}

void CompressFiles()
{
    ThreadPool pool(4); // create a thread pool of size 4
    vector<future<string>> filetasks;

    for (int i = 0; i < 8; ++i) { // add threads for each file to the pool
        filetasks.emplace_back(
            pool.enqueue(threadCompressionTask, i));
    }

    for (auto&& result : filetasks) //get results from completed files
    {
        result.get(); //use the compressed results
    }
    cout << "final compressed file";
}
#pragma endregion

#pragma region Barriers
vector<int> dataVector(3, 0);
mutex dataMutex;
barrier simpleBarrier(4);

void reader()
{
    unsigned int i;
    std::string threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));

    simpleBarrier.count_down_and_wait(); // wait until 4 thread call this method (number = the value set in barrier constructor)

    for (i = 0; i < dataVector.size(); i++)
    {
        std::cout << threadId + " - Requesting reading lock\n";
        dataMutex.lock();
        int read = dataVector[i];
        dataMutex.unlock();
        std::cout << threadId + " - Outside reader critical section\n";
    }
}

void writer()
{
    unsigned int i;
    std::string threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));

    simpleBarrier.count_down_and_wait(); // wait until 4 thread call this method (number = the value set in barrier constructor)

    for (i = 0; i < dataVector.size(); i++)
    {
        std::cout << threadId + " - Requesting writer lock\n";
        dataMutex.lock();
        dataVector[i] = i;
        dataMutex.unlock();
        cout << threadId + " - Outside writer critical section\n";
    }
}

int Barrier()
{
    vector<thread> readers; vector<thread> writers; //hold the threads

    int i;

    for (i = 0; i < 2; i++) //create readers
        readers.emplace_back(reader);

    for (i = 0; i < 2; i++)
        writers.emplace_back(writer);

    for (auto& reader : readers)
        reader.join();

    for (auto& writer : writers)
        writer.join();

    return 0;
}
#pragma endregion

#pragma region Deadlock
// this example is a slightly editied version of this https://gist.github.com/ivcn/227414b3185840434718f7f6c8cbffb1
void deadlock_example() {
    mutex m1; mutex m2;
    thread t1([&m1, &m2] {
        m1.lock(); cout << "1. Aquired m1.\n";
        this_thread::sleep_for(std::chrono::milliseconds(10));
        m2.lock(); cout << "1. Aquired m2. \n";
        });
    thread t2([&m1, &m2] {
        m2.lock(); cout << "2. Aquired m2. \n";
        this_thread::sleep_for(std::chrono::milliseconds(10));
        m1.lock(); cout << "2. Aquired m1. \n";
        });

    t1.join(); t2.join(); //close the threads
}

void fixed_deadlock_example() {
    mutex m1; mutex m2;
    thread t1([&m1, &m2] {
        bool exitt1 = false;
        do {
            if (m1.try_lock()) {
                cout << "1. Aquired m1.\n";
                this_thread::sleep_for(std::chrono::milliseconds(10));
                if (m2.try_lock()) {
                    cout << "1. Aquired m2. \n"; exitt1 = true;
                    m2.unlock(); }
               m1.unlock(); cout << "1. Release m1.\n";
            }
        } while (!exitt1);
        cout << "t1 complete \n";
        });
    thread t2([&m1, &m2] {
        bool exitt2 = false;
        do {
            if (m2.try_lock()) {
                cout << "2. Aquired m2. \n";
                this_thread::sleep_for(std::chrono::milliseconds(15));
                if (m1.try_lock()) {
                    cout << "2. Aquired m1. \n"; exitt2 = true;
                    m1.unlock(); }
                m2.unlock(); 
                cout << "2. Release m2.\n"; }
        } while (!exitt2);
        cout << "t2 complete \n";
        });

    t1.join(); t2.join(); //close the threads
    cout << "success";
}
#pragma endregion


int main() {
    CompressFiles();
}
