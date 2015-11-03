#ifndef REFCOUNTONLY_H
#define REFCOUNTONLY_H

class RefCountOnly {
public:
  using cnt_t = size_t;
  RefCountOnly(cnt_t *cnt = nullptr) :
    cnt_p{cnt}
  {
    *cnt_p = 1U;
  }
  
  RefCountOnly(const RefCountOnly &rhs)
    : cnt_p{rhs.cnt_p}
  {
    ++*cnt_p;
  }

  RefCountOnly &operator=(const RefCountOnly &rhs)
  {
    if (get_shared_cnt_ptr() == rhs.get_shared_cnt_ptr())
      return *this;

    decrease_cnt_check_del();
    cnt_p = rhs.cnt_p;
    ++*cnt_p;

    return *this;
  }
  
  virtual ~RefCountOnly()
  {
    decrease_cnt_check_del();
  }

  const cnt_t *get_shared_cnt_ptr() const { return &*cnt_p;}

protected:
  StackheapPtr<cnt_t> cnt_p;
  
private:
  void decrease_cnt_check_del() {
    if (--*cnt_p == 0) {
      cnt_p.delete1();
    }
  }
};

#endif
