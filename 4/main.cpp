#include <iostream>

#include "basewrapper.h"

class MyClass : public BaseWrapper {
public:
  MyClass(const std::string &name = "MyClass") : BaseWrapper(name)
  {}
};

#define CMD(cmd) std::cerr << #cmd << std::endl; cmd

int main()
{
  CMD(MyClass a{"a"});
  CMD(MyClass b{a});
  CMD(MyClass c{"c"});
  CMD(b = c);
  CMD(b = a);
  return 0;
}
