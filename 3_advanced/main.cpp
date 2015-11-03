#include <iostream>

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
  /*
    Reference Counting (constructor with nullptr, causes allocation on heap; else pass in memory from outside [typically stack]).
    This class tracks instances "of the same value"!   "same value" due to: copy construction, copy assignment.

    Mechanism
    ---------
    Constructor:       *cnt_p = 1
    Copy Constructor:  ++*cnt_p                          // additional instance "of the same value"
    Destructor:        --*cnt_p    (delete if 0)         // one less   instance "of the same value"
    Copy Assignment:   if cnt_p == rhs.cnt_p do nothing; else: 
    .                                                         a) --*cnt_p                 // one less  instance
    .                                                         b) delete if 0
    .                                                         c) cnt_p = rhs.cnt_p
    .                                                         d) ++*cnt_p                 // additional instance "of the same value" as rhs

    There also is a virtual function "hook" (virt_decrease), with which derived classes
    are informed of the same changes, and can perform the same mechanism for their data
   */
public:
  using cnt_t = size_t;

  RefCount(cnt_t *cnt = nullptr) :
    cnt_p{cnt}
  {
    *cnt_p = 1U;
  }
  
  RefCount(const RefCount &rhs)
    : cnt_p{rhs.cnt_p}
  {
    // d) increment
    ++*cnt_p;
  }
  
  RefCount &operator=(const RefCount &rhs)
  {
    if (get_shared_cnt_ptr() == rhs.get_shared_cnt_ptr())
      return *this;

    // a) decrease    and if necessary   b) delete
    decrease_cnt_check_del(&rhs);

    // c) copy_assign
    cnt_p = rhs.cnt_p;

    // d) increment
    ++*cnt_p;

    return *this;
  }
  
  virtual ~RefCount()
  {
    /* Cannot call    decrease_cnt_check_del();    here,
       since it calls virt_decrease, but for virtual functions in the destructor,
       the derived class is already destructed, to the virtual function will not reach the derived class!

       Solution: the derived class calls destructor_pre_cleanup_from_derived(), when it (the derived class is in the destructor)
     */
  }

  void destructor_pre_cleanup_from_derived() // called from derived class's destructor (reason: the callstack will end up calling virt_decrease)
  {
    // a) decrease    and if necessary   b) delete
    decrease_cnt_check_del();
  }

  
  const cnt_t *get_shared_cnt_ptr()  const { return &*cnt_p;}

private:

  StackheapPtr<cnt_t> cnt_p;

  virtual void virt_decrease(const void *copy_assign_rhs, bool del)
  {}
  
  void decrease_cnt_check_del(const void *copy_assign_rhs = nullptr) {
    bool del = false;
    if (--*cnt_p == 0) {
      del = true;
      cnt_p.delete1();
    }
    virt_decrease(copy_assign_rhs, del);
  }
};



template <typename T>
class RefCountWithData : public RefCount {
  /*
    Wrapper derived class, that holds common data (such as a name!) with instances "of the same value"

    Constructor takes a value of the data to hold, and then 2 pointers (one to hold copy of data, one to hold counter)
    If a pointer is nullptr, then constructor allocates on heap; else pass in memory from outside [typically from stack].

    The data is "tracked" and held among instances "of the same value" (achieved through the base class RefCount)
   */
public:
  using datatype = T;
  
  RefCountWithData(const T &data_cpy, T *data_ = nullptr, RefCount::cnt_t *cnt = nullptr)
    : RefCount{cnt}, data{data_}
  {
    *data = data_cpy;
  }
  
  RefCountWithData &operator=(const RefCountWithData &rhs)
  {
    // d) increment
    /* not used here */

    RefCount::operator=(rhs);
  }

  ~RefCountWithData()
  {
    RefCount::destructor_pre_cleanup_from_derived();
  }
  
  virtual void virt_decrease(const void *copy_assign_rhs, bool del)
  {
    // a) decrease
    /* not used here */

    if (del) {
      // b) delete
      data.delete1();
    }

    if (copy_assign_rhs) {
      // c) copy_assign
      data = static_cast<const RefCountWithData *>(copy_assign_rhs)->data;

      // d) increment
      /* not used here */
    }

  }

  const T &get_data()  const { return *data; }
  
private:
  StackheapPtr<T> data;
};


class BaseWrapper : public RefCountWithData<std::string> {
  /* Class used for monitoring constructor and destructor behaviour.
     Also monitor instances "of the same value":      "same value" due to: copy construction, copy assignment.

     Other classes just need to derive from this class and get automatic monitoring.

     Consructor (same as RefCountWithData, to which the arguments are forwarded):
     Constructor takes a value of the name to hold, and then 2 pointers (one to hold copy of name, one to hold counter)
     If a pointer is nullptr, then constructor allocates on heap; else pass in memory from outside [typically from stack].
  */
public:
  BaseWrapper(const std::string &name_, std::string *name_ptr = nullptr, RefCount::cnt_t *cnt_ptr = nullptr)
    : RefCountWithData<std::string>{name_, name_ptr, cnt_ptr}
  {
    std::cerr << "#constructor      ";
    print_info(std::cerr) << std::endl;
  }
  
  BaseWrapper(const BaseWrapper &rhs)
    : RefCountWithData<std::string>{rhs}
  {
    std::cerr << "#copy-constructor ";
    print_info(std::cerr) << std::endl;
  }
  
  BaseWrapper &operator=(const BaseWrapper &rhs)
  {
    std::cerr << "#operator=        ";
    print_info(std::cerr);
    if (get_shared_cnt_ptr() == rhs.get_shared_cnt_ptr()) {
      std::cerr << "\t already_holding_same_value" << std::endl;
        return *this;
    }

    RefCountWithData<std::string>::operator=(rhs);
    std::cerr << "\t ==>  ";
    print_info(std::cerr) << std::endl;
    return *this;
  }
  
  ~BaseWrapper()
  {
    std::cerr << "#destructor       ";
    print_info(std::cerr) << std::endl;
  }

private:
  std::ostream& print_info(std::ostream &os) {
    os << "cnt_p " << get_shared_cnt_ptr() << " \tthis " << this << " \t" << get_data() << " (" << *get_shared_cnt_ptr() << ')';
  }
};



int main()
{
  BaseWrapper::datatype /*std::string*/ name;
  BaseWrapper::cnt_t    /*size_t     */ cnt;

  BaseWrapper br1{"br1", &name, &cnt};
  BaseWrapper br2{br1};
  BaseWrapper br3{"br3"};
  br3 = br1;
  return 0;
}
