#ifndef SINOS_64KERNEL_QUEUE_H
#define SINOS_64KERNEL_QUEUE_H

#include "types.h"

template <typename T, u32_t N>
class Queue
{
protected:
  DISALLOW_COPY_AND_ASSIGN(Queue);

  T arr_[N];
  u32_t head_;
  u32_t tail_;
  u32_t num_;

  bool isFull() const {return (num_ == N) ? true : false;}

public:
  Queue()
    : head_(0), tail_(0), num_(0)
  {}

  void push(const T& data);
  void pop();
  const T& front() const {return arr_[head_];}
  u32_t size() const {return num_;}
  bool isEmpty() const {return (num_ == 0) ? true : false;}
};

template <typename T, u32_t N>
void Queue<T, N>::push(const T& data)
{
  if (isFull())
    return;

  arr_[tail_] = data;
  
  if (++tail_ >= N)
    tail_ = 0;
  ++num_;
}

template <typename T, u32_t N>
void Queue<T, N>::pop()
{
  if (isEmpty())
    return;

  if (++head_ >= N)
    head_ = 0;
  --num_;
}

#endif // SINOS_64KERNEL_QUEUE_H
