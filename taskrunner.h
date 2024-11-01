#ifndef TASKRUNNER_H
#define TASKRUNNER_H

// implemention for a thread
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>

#include "sharedqueue.h"

using namespace std::chrono_literals;

class UTILS_EXPORTS TaskRunner {
public:
  TaskRunner();
  ~TaskRunner();
  template<typename Executable>
  void PostTask(Executable executable);
  void Terminate();
  void SetThreadName(std::string&& threadName) {
    m_threadName = std::move(threadName);
  }
private:
  static void Run(TaskRunner* pthis);
private:
  std::atomic_bool                   m_isRunning;
  SharedQueue<std::function<void()>> m_executableList;
  std::thread                        m_thread;
  std::string                        m_threadName;
  static int                         m_threadId;
};

template<typename Executable>
void TaskRunner::PostTask(Executable executable)
{
  std::function<void()> f(executable);
  m_executableList.Push(f);
}

#endif // TASKRUNNER_H
