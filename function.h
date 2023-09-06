#ifndef FUNCTION_H
#define FUNCTION_H

namespace miniSTL{

template <class T>   //【临时】
struct identity  {
  const T& operator()(const T& x) const { return x; }
};



}
#endif