#include <iostream>

template <typename T>
class StackheapPtr
/*
  Class that is just as unsafe as a normal pointer, 
  meaning that you must not forget to call delete (if the class was default-initialized [with nullptr]).
  
  But the pointer can refer to memory on the heap (allocated with new) -- this is if it was default-initialized [with nullptr]
  .      -> in this case delete1() will delete
  Or  the pointer can refer to memory passed in from the outside (typically on the stack)
  .      -> in this case delete1() will not delete (since the memory has to be handled from the outside)
 */
{
public:
  using type = T;

  StackheapPtr(T *p = nullptr)
    : ptr{p}, is_heap{false}
  {
    if (ptr == nullptr) {
      ptr = new T{};
      is_heap = true;
    }
  }

  StackheapPtr(const StackheapPtr<T> &rhs)
    : ptr{rhs.ptr}, is_heap{rhs.is_heap}
  {
  }
  
  StackheapPtr<T> &operator=(const StackheapPtr<T> &rhs)
  {
    ptr     = rhs.ptr;
    is_heap = rhs.is_heap;
    return *this;
  }

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




class RefCount {
public:
  using cnt_t = size_t;
  RefCount(const std::string &name_, cnt_t *cnt = nullptr) :
    name{name_}, cnt_p{cnt}
  {
    *cnt_p = 1U;
  }
  
  RefCount(const RefCount &rhs)
    : name{rhs.name}, cnt_p{rhs.cnt_p}
  {
    ++*cnt_p;
  }

  RefCount &operator=(const RefCount &rhs)
  {
    if (get_shared_cnt_ptr() == rhs.get_shared_cnt_ptr())
      return *this;

    name = rhs.name;
    decrease_cnt_check_del();
    cnt_p = rhs.cnt_p;
    ++*cnt_p;

    return *this;
  }
  
  virtual ~RefCount()
  {
    decrease_cnt_check_del();
  }

  const cnt_t      *get_shared_cnt_ptr()  const { return &*cnt_p;}
  std::string       get_name()            const { return name; }

protected:
  StackheapPtr<cnt_t> cnt_p;
  std::string      name;  /* name could also be held on heap 
                             (in which case objects with same value could point to a single name instance;
                             instead of each holding their own name, as is currently the case */
  
private:
  void decrease_cnt_check_del() {
    if (--*cnt_p == 0) {
      cnt_p.delete1();
    }
  }
};


class BaseWrapper : public RefCount {
  // Class used for monitoring constructor and destructor behaviour.
  // Other classes just need to derive from this class!
public:
  BaseWrapper(const std::string &name_, RefCount::cnt_t *cnt = new RefCount::cnt_t)
    : RefCount{name_, cnt}
  {
    std::cout << "#constructor      \t cnt_p " << get_shared_cnt_ptr() << "  " << name << " (" << *get_shared_cnt_ptr() << ')' << std::endl;    
  }
  
  BaseWrapper(const BaseWrapper &rhs)
    : RefCount{rhs}
  {
    std::cout << "#copy-constructor \t cnt_p " << get_shared_cnt_ptr() << "  " << name << " (" << *get_shared_cnt_ptr() << ')' << std::endl;
  }
  
  BaseWrapper &operator=(const BaseWrapper &rhs)
  {
    std::cout << "#operator=        \t cnt_p " << get_shared_cnt_ptr() << "  " << name << " (" << *get_shared_cnt_ptr() << ')';
    if (get_shared_cnt_ptr() == rhs.get_shared_cnt_ptr()) {
      std::cout << "\t already_holding_same_value" << std::endl;
        return *this;
    }

    RefCount::operator=(rhs);
    std::cout << "\t ==>  cnt_p " << get_shared_cnt_ptr() << "  " << name << " (" << *get_shared_cnt_ptr() << ')' << std::endl;
    return *this;
  }
  
  ~BaseWrapper()
  {
    std::cout << "#destructor       \t cnt_p " << get_shared_cnt_ptr() << "  " << name << " (" << *get_shared_cnt_ptr() << ')' << std::endl;
  }

};


int main()
{
  StackheapPtr<int> s1;

  int i;
  StackheapPtr<int> s2{&i};

  BaseWrapper br{"br"};
  return 0;
}
