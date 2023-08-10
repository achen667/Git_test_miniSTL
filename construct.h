#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include <new.h>
#include "type_traits.h"
namespace miniSTL{

template <class T1, class T2>
inline void construct(T1* p, const T2& value){
    new (p) T1(value);   //placement new   
}

template <class T>
inline void destroy(T* pointer){
    pointer->~T();
}

template <class ForwardIterator>
inline void
_destroy_aux(ForwardIterator first, ForwardIterator last, _false_type){
    for( ; first < last; ++first)
        destory(&*first);
}

//由系统自动回收内存 无须析构
template <class ForwardIterator>
inline void
_destroy_aux(ForwardIterator, ForwardIterator, _true_type){
}  

//判断value type的destructor类型
template <class ForwardIterator, class T>
inline void _destroy(ForwardIterator first, ForwardIterator last, T*){
    using trivial_destructor = typename _type_traits<T>::has_trivial_destructor ;  //TO-DO  编写traits 
    _destroy_aux(first, last, trivial_destructor());
}

//提取value type
template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last){
    _destroy(first, last, value_type(first)); //TO-DO 在iterator里编写 value_type()
}

inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}


}


#endif