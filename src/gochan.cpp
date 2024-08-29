#include <list>
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <future>
#include <exception>
#include <chrono>

template <class item>
class channel
{
private:
  std::list<item> queue;
  std::mutex m;
  std::condition_variable cv;
  bool closed;

public:
  channel() : closed(false) {}
  void close()
  {
    std::unique_lock<std::mutex> lock(m);
    closed = true;
    cv.notify_all();
  }
  bool is_closed()
  {
    std::unique_lock<std::mutex> lock(m);
    return closed;
  }
  void put(const item &i)
  {
    std::unique_lock<std::mutex> lock(m);
    if (closed)
      throw std::logic_error("put to closed channel");
    queue.push_back(i);
    cv.notify_one();
  }
  bool get(item &out, bool wait = true)
  {
    std::unique_lock<std::mutex> lock(m);
    if (wait)
      cv.wait(lock, [&]()
              { return closed || !queue.empty(); });
    if (queue.empty())
      return false;
    out = queue.front();
    queue.pop_front();
    return true;
  }
};

enum state
{
  stop1,
  stop2,
  stop3,
  go
};

int producer1(channel<int> *ch_data, channel<state> *ch_sync)
{
  int i = 0;
  state s = go;

  while (!ch_sync->is_closed() && !ch_data->is_closed())
  {
    ch_sync->get(s, false);
    if (s == stop1)
      break;
    else if (s == stop2)
      ch_sync->put(s); // putback - can throw exception!
    ch_data->put(++i);
    std::this_thread::sleep_for(std::chrono::microseconds(500));
  }
  return 1;
}

int producer2(channel<int> *ch_data, channel<state> *ch_sync, int n)
{
  int i = 0;
  auto s = go;
  for (int i = 1; i <= n; i++)
  {
    ch_sync->get(s, false);
    if (s == stop2)
      break;
    else if (s == stop1)
      ch_sync->put(s); // putback
    ch_data->put(-i);
    std::this_thread::sleep_for(std::chrono::microseconds(500));
  }
  return 1;
}

int consumer(channel<int> *ch_data, channel<state> *ch_sync, int n1, int n2)
{
  int i = 0;
  auto s = go;
  static state istate = go;

  while (!ch_sync->is_closed() && !ch_data->is_closed())
  {
    int received = 0;
    ch_data->get(received, false);
    if (received != 0)
    {
      std::cout << received << " ";
    }

    if (received == n1)
    {
      ch_sync->put(stop1);
      if (istate == stop2)
      {
        ch_data->close();
      }
      else
        istate = stop1;
    }
    else if (received == -n2)
    {
      ch_sync->put(stop2);
      if (istate == stop1)
      {
        ch_data->close();
      }
      else
        istate = stop2;
    }
  }
  return 1;
}

int commTest()
{
  std::cout << "Comm test: ";
  int received;
  channel<int> ch_data;
  channel<state> ch_sync;
  int n1 = 100;
  int n2 = 50;

  std::future<int> prod1Result(std::async(producer1, &ch_data, &ch_sync));
  std::future<int> prod2Result(std::async(producer2, &ch_data, &ch_sync, n2));
  std::future<int> consResult(std::async(consumer, &ch_data, &ch_sync, n1, n2));
  // std::future<int> prod3Result(std::async(producer2, &ch_data, &ch_sync, n2));
  // std::future<int> consResult2(std::async(consumer, &ch_data, &ch_sync, 50));

  prod1Result.get();
  prod2Result.get();
  // prod3Result.get();

  consResult.get();

  std::cout << "\n************************\n";
  // consResult2.get();

  return 0;
}

int simpleTest()
{
  int received;
  channel<int> my_channel;
  my_channel.put(1);
  my_channel.put(2);
  my_channel.put(3);

  std::cout << "Simple test:";
  while (my_channel.get(received, false))
  {
    std::cout << received;
    if (my_channel.is_closed())
      break;
  }
  std::cout << std::endl;
  return 0;
}

// Simple go-like channel test
// and producer consumer concurrency test

int main()
{
  simpleTest();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  commTest();
  return 0;
}