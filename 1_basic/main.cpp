#include <iostream>
#include <string>
#include <map>


class RefCount {
public:
  using cnt_t = size_t;
  RefCount(const std::string &name_, cnt_t *cnt = new cnt_t) :
    name{name_}, cnt_p{&(*cnt = 1U)}
  { }
  
  RefCount(const RefCount &rhs)
    : name{rhs.name}, cnt_p{&(++(*rhs.cnt_p))}
  {}

  RefCount &operator=(const RefCount &rhs)
  {
    if (get_shared_cnt_ptr() == rhs.get_shared_cnt_ptr())
      return *this;

    name = rhs.name;
    decrease_cnt_check_del();
    cnt_p = &(++(*rhs.cnt_p));
    return *this;
  }
  
  virtual ~RefCount()
  {
    decrease_cnt_check_del();
  }

  const cnt_t      *get_shared_cnt_ptr() const { return cnt_p; }
  std::string       get_name()            const { return name; }

protected:
  cnt_t     *cnt_p;
  std::string name;  /* name could also be held on heap 
                        (in which case objects with same value could point to a single name instance;
                        instead of each holding their own name, as is currently the case */

private:
  void decrease_cnt_check_del() {
    if (--*cnt_p == 0) {
      delete cnt_p;
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



class MyClass : BaseWrapper {
public:
  MyClass(const std::string &name) : BaseWrapper{name}
  {}
};

#define CMD(C) std::cout << "   " #C << std::endl; C

int main()
{
  CMD( MyClass c1{"asdf"});
  CMD( MyClass c2{c1});

  CMD( MyClass c3{"qwer"});
  CMD( c3 = c2);
  CMD( c1 = c3);
  
  CMD(return 0);
}
