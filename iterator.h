#ifndef  ITERATOR_H
#define ITERATOR_H

#include <cstddef>

namespace miniSTL{

//5 iterator category
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <class Category, class T,
        class Distance = ptrdiff_t,
        class Pointer = T*,
        class Reference = T&>
struct iterator{
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;   //【ptrdiff_t 两个指针相减的类型。在一个平台内是固定的 】
    using pointer = Pointer;
    using reference = Reference;
};
//萃取迭代器（Iterator）的类型
template <class Iterator>
struct iterator_traits {
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using difference_type = typename Iterator::difference_type;
  using pointer = typename Iterator::pointer ;
  using reference = typename Iterator::reference;
};
//针对原生指针的特化
template <class T>
struct iterator_traits<T*> {
  using iterator_category = random_access_iterator_tag; 
  using value_type = T;
  using difference_type = ptrdiff_t ;
  using pointer = T* ;
  using reference = T&;
};
//针对常量原生指针的特化
template <class T>
struct iterator_traits<const T*> {
  using iterator_category = random_access_iterator_tag; 
  using value_type = T;
  using difference_type = ptrdiff_t ;
  using pointer = const T*;
  using reference = const T&;
};
template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
  using category = typename iterator_traits<Iterator>::iterator_category;
  return category();
}


//2 difference type
template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
  return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}
//1 value type
template <class Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
  return static_cast<typename iterator_traits<Iterator>::value_type*>(0); //【这两组转型 理解一下】
}

//distance
template <class InputIterator, class Distance>
inline typename iterator_traits<InputIterator>::difference_type
_distance(InputIterator first, InputIterator last, input_iterator_tag){
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last){ ++first; ++n;}
    return n;
}

template <class RandomAccessIterator, class Distance>
inline typename iterator_traits<RandomAccessIterator>::difference_type 
_distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag){
    return last - first;   //连续存储
}

template <class InputIterator, class Distance>
inline typename iterator_traits<InputIterator>::difference_type 
distance(InputIterator first, InputIterator last){
    return _distance(first, last, iterator_category(first));
}


}
#endif

