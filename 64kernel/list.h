#ifndef SINOS_64KERNEL_LIST_H
#define SINOS_64KERNEL_LIST_H

#include "types.h"

template <typename T>
class Node
{
protected:
  DISALLOW_COPY_AND_ASSIGN(Node);

  int next_;
  T data_;
  
public:
  Node() {}

  void setData(T data) {data_ = data;}
  const T& getData() const {return data_;}

  void setNext(int itr_) {next_ = itr_;}
  int getNext() const {return next_;}
};

template <typename T, u32_t N>
class List
{
protected:
  DISALLOW_COPY_AND_ASSIGN(List);

  int head_;
  int tail_;
  Node<T> arr_[N];
  bool isAllocated[N];
  u32_t num_;

  int prev_itr_;
  int itr_;

  void insertNode(int empty_node, const T& value);
  
public:
  List()
    : head_(0), tail_(0), num_(0)
  {
    for (int i = 0; i < N; i++)
      isAllocated[i] = false;
  }

  u32_t size() const {return num_;}

  // Modifiers
  void push_front(const T& value);
  void push_back(const T& value);

  void pop_front();
  void pop_back();

  // Element access
  const T& front() const
  {
    if (num_ != 0)
      return arr_[head_].getData();
  }
  const T& back() const
  {
    if (num_ != 0)
      return arr_[tail_].getData();
  }

  // Iterators
  bool begin() {itr_ = head_; prev_itr_ = tail_;}
  List<T, N>& operator++();
  const T& operator*() const;
  void remove();
};

template <typename T, u32_t N>
void List<T, N>::remove()
{
  if (num_ == 0)
    return;

  int next_itr_ = arr_[itr_].getNext();
  if (itr_ == head_) {
    head_ = next_itr_;
  }
  if (itr_ == tail_) {
    tail_ = prev_itr_;
  }
  arr_[prev_itr_].setNext(next_itr_);

  num_--;
  isAllocated[itr_] = false;
  itr_ = next_itr_;
}

template <typename T, u32_t N>
const T& List<T, N>::operator*() const
{
  if (num_ != 0)
    return arr_[itr_].getData();
}

template <typename T, u32_t N>
List<T, N>& List<T, N>::operator++()
{
  prev_itr_ = itr_;
  itr_ = arr_[itr_].getNext();
  return *this;
}

template <typename T, u32_t N>
void List<T, N>::pop_front()
{
  if (num_ == 0)
    return;
  
  num_--;
  isAllocated[head_] = false;
  
  head_ = arr_[head_].getNext();
  arr_[tail_].setNext(head_);
}

template <typename T, u32_t N>
void List<T, N>::pop_back()
{
  if (num_ == 0)
    return;

  num_--;
  isAllocated[tail_] = false;

  int prev_tail = head_;
  while (arr_[prev_tail].getNext() != tail_)
    prev_tail = arr_[prev_tail].getNext();

  tail_ = prev_tail;
  arr_[tail_].setNext(head_);
}

template <typename T, u32_t N>
void List<T, N>::insertNode(int empty_node, const T& value)
{
  arr_[empty_node].setData(value);
  arr_[empty_node].setNext(head_);
  arr_[tail_].setNext(empty_node);
  
  num_++;
  isAllocated[empty_node] = true;
}

template <typename T, u32_t N>
void List<T, N>::push_front(const T& value)
{
  if (num_ >= N)
    return;

  int empty_node = head_;
  while (isAllocated[empty_node] == true) {
    if (--empty_node < 0)
      empty_node = N - 1;
  }

  insertNode(empty_node, value);
  
  head_ = empty_node;  
  if (num_ == 0)
    tail_ = head_;
}

template <typename T, u32_t N>
void List<T, N>::push_back(const T& value)
{
  if (num_ >= N)
    return;

  int empty_node = tail_;
  while (isAllocated[empty_node] == true) {
    if (++empty_node >= N)
      empty_node = 0;
  }

  insertNode(empty_node, value);
  
  tail_ = empty_node;
  if (num_ == 0)
    head_ = tail_;
}

#endif // SINOS_64KERNEL_LIST_H
