#ifndef BASEWRAPPER_H
#define BASEWRAPPER_H

#include "refcount.h"

class BaseWrapper : public RefCount<std::string> {
  using ref_t = RefCount<std::string>;
public:
  BaseWrapper(const std::string &name = "") : ref_t(name)
  {
    std::cerr << "#constructor      ";
    print_info(std::cerr) << std::endl;
  }
  BaseWrapper(const BaseWrapper &rhs) : ref_t(rhs)
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

    ref_t::operator=(rhs);
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

#endif
