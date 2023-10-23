//
//  Copyright (C) 2020 Shrey Aryan
//
//   @@ All Rights Reserved @@
//  This file is part of the RDKit.
//  The contents are covered by the terms of the BSD license
//  which is included in the file license.txt, found at the root
//  of the RDKit source tree.
//
#ifdef RDK_BUILD_THREADSAFE_SSS
#ifndef CONCURRENT_QUEUE
#define CONCURRENT_QUEUE
#include <condition_variable>
#include <thread>
#include <vector>

namespace RDKit {
template <typename E>
class ConcurrentQueue {
 private:
  unsigned int d_capacity;
  bool d_done;
  std::vector<E> d_elements;
  unsigned int d_head, d_tail;
  mutable std::mutex d_lock;
  std::condition_variable d_notEmpty, d_notFull;

 private:
  ConcurrentQueue(const ConcurrentQueue<E>&);
  ConcurrentQueue& operator=(const ConcurrentQueue<E>&);

 public:
  ConcurrentQueue(unsigned int capacity)
      : d_capacity(capacity), d_done(false), d_head(0), d_tail(0) {
    std::vector<E> elements(capacity);
    d_elements = elements;
  }

  //! tries to push an element into the queue if it is not full without
  //! modifying the variable element, if the queue is full then pushing an
  //! element will result in blocking
  bool push(const E& element);

  //! tries to pop an element from the queue if it is not empty and not done
  //! the boolean value indicates the whether popping is successful, if the
  //! queue is empty and not done then popping an element will result in
  //! blocking
  bool pop(E& element);

  //! checks whether the ConcurrentQueue is empty
  bool isEmpty() const;

  //! returns the value of the variable done
  bool getDone() const;

  //! sets the variable d_done = true
  void setDone();

  //! clears the vector
  void clear();
};

template <typename E>
bool ConcurrentQueue<E>::push(const E& element) {
  std::unique_lock<std::mutex> lk(d_lock);
  //! concurrent queue is full so we wait until
  //! it is not full
  while (d_head + d_capacity == d_tail) {
    if (d_done) {
      return false;
    }
    d_notFull.wait(lk);
  }

  d_elements.at(d_tail % d_capacity) = element;
  d_tail++;

  lk.unlock();
  d_notEmpty.notify_one();
  return true;
}

template <typename E>
bool ConcurrentQueue<E>::pop(E& element) {
  std::unique_lock<std::mutex> lk(d_lock);
  //! concurrent queue is empty so we wait until
  //! it is not empty
  while (d_head == d_tail) {
    if (d_done) {
      return false;
    }
    d_notEmpty.wait(lk);
  }

  element = d_elements.at(d_head % d_capacity);
  d_head++;

  lk.unlock();
  d_notFull.notify_one();
  return true;
}

template <typename E>
bool ConcurrentQueue<E>::isEmpty() const {
  std::unique_lock<std::mutex> lk(d_lock);
  return (d_head == d_tail);
}

template <typename E>
bool ConcurrentQueue<E>::getDone() const {
  std::unique_lock<std::mutex> lk(d_lock);
  return d_done;
}

template <typename E>
void ConcurrentQueue<E>::setDone() {
  std::unique_lock<std::mutex> lk(d_lock);
  d_done = true;
  lk.unlock();
  d_notEmpty.notify_all();
}

template <typename E>
void ConcurrentQueue<E>::clear() {
  std::unique_lock<std::mutex> lk(d_lock);
  d_elements.clear();
  d_head = 0;
  d_tail = 0;
  lk.unlock();
  d_notFull.notify_all();
}

}  // namespace RDKit
#endif
#endif
