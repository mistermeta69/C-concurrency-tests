// concurrency/async1.cpp
#include <future>
#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include <exception>
using namespace std;

int doSomething (char c)
{
// random-number generator (use c as seed to get different sequences)
std::default_random_engine dre(c);
std::uniform_int_distribution<int> id(10,1000);
// loop to print character after a random period of time
for (int i=0; i<10; ++i) {
    this_thread::sleep_for(chrono::milliseconds(id(dre)));
    cout.put(c).flush();
}
return c;
}

int func1 ()
{
return doSomething('.');
}

int func2 ()
{
return doSomething('+');
}

int func3 ()
{
return doSomething('*');
}

int main()
{
std::cout << "starting func1/2() in background"
<< " and func3() in foreground:" << std::endl;

// start func1() asynchronously (now or later or never):
std::future<int> result1(std::async(func1));
std::future<int> result2(std::async(func2));
std::future<int> result4(std::async(func2));
// call func3() synchronously (here and now)
int result3 = func3();

// print result (wait for func1/2() to finish and add its result to result2
//int result = result1.get() + result2;
int result = result1.get() + result2.get() + result3 + result4.get();
std::cout << "\nresult of func1()+func2()+func3()+func4(): " << result
<< std::endl;
return 0;
}