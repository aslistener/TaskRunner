#include "taskrunner.h"

#include <Windows.h>

int TaskRunner::m_threadId = 0;

TaskRunner::TaskRunner() :
  m_isRunning(true),
  m_thread(std::bind(TaskRunner::Run, this)) {
  m_threadId++;
  char tn[MAX_PATH + 1];
  ZeroMemory(tn, MAX_PATH + 1);
  sprintf_s(tn, MAX_PATH, "thread %d", m_threadId);
  m_threadName = tn;
  m_thread.detach();
}

TaskRunner::~TaskRunner()
{
  ////  Terminate();
}

void TaskRunner::Terminate()
{
  m_isRunning.store(false);
  m_executableList.Clear();
  WaitForSingleObject((HANDLE)m_thread.native_handle(), 1000);
}

void TaskRunner::Run(TaskRunner* pthis)
{
  while (pthis->m_isRunning.load()) {
    std::function<void()> executable;
    bool hasElement = !pthis->m_executableList.IsEmpty() && 
        pthis->m_executableList.TryPop(executable);
    if (hasElement) {
      executable();
    }
    else {
      std::this_thread::sleep_for(100ms);
    }
  }
}
