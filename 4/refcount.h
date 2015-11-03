#ifndef REFCOUNT_H
#define REFCOUNT_H

#include "stackheapptr.h"

template<typename T>
class RefCount {
public:
  using cnt_t = size_t;
  RefCount(const T &dat = T{}, T * dat_ptr = nullptr, cnt_t *cnt = nullptr) :
    cnt_p{cnt}, data{dat_ptr}
  {
    *cnt_p = 1U;
    *data  = dat;
  }
  
  RefCount(const RefCount &rhs)
    : data{rhs.data}, cnt_p{rhs.cnt_p}
  {
    ++*cnt_p;
  }

  RefCount<T> &operator=(const RefCount<T> &rhs);
  
  virtual ~RefCount()
  {
    decrease_cnt_check_del();
  }

  const cnt_t *get_shared_cnt_ptr() const { return &*cnt_p;}
  const T &get_data() const { return *data; }
  T       &get_data()       { return *data; }

protected:
  StackheapPtr<cnt_t> cnt_p;
  StackheapPtr<T>     data;

  
private:
   void decrease_cnt_check_del();
};

template <typename T>
RefCount<T> &RefCount<T>::operator=(const RefCount<T> &rhs)
{
   if (get_shared_cnt_ptr() == rhs.get_shared_cnt_ptr())
      return *this;

   data = rhs.data;
   decrease_cnt_check_del();
   cnt_p = rhs.cnt_p;
   ++*cnt_p;

   return *this;
}

template <typename T>
void RefCount<T>::decrease_cnt_check_del() {
   if (--*cnt_p == 0) {
      data.delete1();
      cnt_p.delete1();
   }
}

#endif
