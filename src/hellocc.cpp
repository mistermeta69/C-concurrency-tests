// async example
#include <future>   // std::async, std::futureecho $PATH
#include <iostream> // std::cout
#include <thread>
#include <chrono>

// a non-optimized way of checking for prime numbers:
bool is_prime(int x)
{
  std::cout << "Calculating. Please, wait...\n";
  for (int i = 2; i < x; ++i)
    if (x % i == 0)
      return false;
  return true;
}

int main()
{
  // Start measuring time
  auto begin = std::chrono::high_resolution_clock::now();

  std::cout << "Call is_prime(313222313) asynchronously:\n";
  std::future<bool> fut = std::async(is_prime, 313222313);

  std::cout << "Checking whether 313222313 is prime.\n";
  // ...

  bool ret = fut.get(); // waits for is_prime to return

  if (ret)
    std::cout << "It is prime!\n";
  else
    std::cout << "It is not prime.\n";

  // Stop measuring time and calculate the elapsed time
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
  printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
  return 0;
}
