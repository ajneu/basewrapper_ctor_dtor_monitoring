#ifndef STACKHEAPPTR_H
#define STACKHEAPPTR_H

template <typename T>
class StackheapPtr
/*
  Class that is just as unsafe as a normal pointer, 
  meaning that you must not forget to call delete (if the class was default-initialized [with nullptr]).
  
  But!!!
  The pointer can refer to memory on the heap (allocated with new) -- this is if it was default-initialized [with nullptr]
  .      -> in this case delete1() will delete
  Or  the pointer can refer to memory passed in from the outside (typically on the stack)
  .      -> in this case delete1() will not delete (since the memory then has to be handled from the outside)
 */
{
public:
  using type = T;

  StackheapPtr(T *p = nullptr);

  StackheapPtr(const StackheapPtr<T> &rhs)
    : ptr{rhs.ptr}, is_heap{rhs.is_heap}
  {
  }
  
  StackheapPtr<T> &operator=(const StackheapPtr<T> &rhs);

  virtual ~StackheapPtr()
  {
  }

  void delete1()
  {
    if (is_heap)
      delete ptr;
  }
  
  T       &operator*()       { return *ptr; }
  const T &operator*() const { return *ptr; }

  T       *operator->()       { return ptr; }
  const T *operator->() const { return ptr; }

private:
  T   *ptr;
  bool is_heap;
};

template <typename T>
StackheapPtr<T>::StackheapPtr(T *p)
  : ptr{p}, is_heap{false}
{
   if (ptr == nullptr) {
      ptr = new T{};
      is_heap = true;
   }
}

template <typename T>
StackheapPtr<T> &StackheapPtr<T>::operator=(const StackheapPtr<T> &rhs)
{
   ptr     = rhs.ptr;
   is_heap = rhs.is_heap;
   return *this;
}

#endif
