#ifndef SHAREDQUEUE_H
#define SHAREDQUEUE_H

#include <mutex>
#include <queue>
#include <atomic>
#include <vector>


template<typename T>
class SharedQueue
{
public:
    SharedQueue() {
        m_chunkSize.store(5);
    }

    ~SharedQueue() {
    }

    void Push(T &t);

    T Pop();

    T TakeLast();

    void PushChunk(std::vector<T> &&);
    std::vector<T> PopChunk();

    void PopChunk(std::vector<T> &);

    void SetChunkSize(int size) {
        m_chunkSize.store(size);
    }

    void Clear() {
      std::lock_guard<std::mutex> guard(m_mutex);
      std::queue<T> queue;
      m_queue.swap(queue);
    }

    int GetChunkSize() {
        return m_chunkSize.load();
    }

    bool TryPop(T &t) {
      bool ret = true;
      {
        std::lock_guard<std::mutex> guard(m_mutex);
        if(m_queue.empty())
          ret = false;
        else {
          t = m_queue.front();
          m_queue.pop();
          ret = true;
        }
      }
      return ret;
    }

    bool IsEmpty() {
      bool isEmpty = true;
      {
        std::lock_guard<std::mutex> guard(m_mutex);
        isEmpty = m_queue.empty();
      }
      return isEmpty;
    }

private:
    std::mutex m_mutex;
    std::queue<T> m_queue;
    std::atomic_int m_chunkSize;
};

template<typename T>
void SharedQueue<T>::Push(T &t) {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.push(t);
}

template<typename T>
T SharedQueue<T>::Pop() {
    T t;
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      t = m_queue.front();
      m_queue.pop();
    }

    return t;
}

template<typename T>
T SharedQueue<T>::TakeLast() {
  T t;
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    t = m_queue.back();
  }
    return t;
}

template<typename T>
void SharedQueue<T>::PushChunk(std::vector<T> && chunk)
{
    std::vector<T> temp = std::move(chunk);
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      for(int i = 0; i < temp.size(); i++) {
          m_queue.push(temp[i]);
      }
    }
    temp.clear();
}

template<typename T>
std::vector<T> SharedQueue<T>::PopChunk()
{
    std::vector<T> chunk;
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        int vecSize = 0;
        vecSize = (GetChunkSize() < (int)m_queue.size() ?
                       GetChunkSize() : (int)m_queue.size());
        if(vecSize > 0) {
            chunk.reserve(vecSize);
            for(int i = 0; i < GetChunkSize() && !m_queue.empty(); i++) {
             chunk.emplace_back( m_queue.front());
                m_queue.pop();
            }
        }
    }
    return chunk;
}

template<typename T>
void SharedQueue<T>::PopChunk(std::vector<T> &chunk)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    int vecSize = 0;
    vecSize = (GetChunkSize() < (int)m_queue.size() ?
                   GetChunkSize() : (int)m_queue.size());
    if(vecSize > 0) {
        for(int i = 0; i < vecSize; i++) {
            chunk.push_back(m_queue.front());
            m_queue.pop();
        }
    }
}

#endif // SHAREDQUEUE_H
