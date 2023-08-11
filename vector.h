#ifndef VECTOR_H
#define VECTOR_H

#include "alloc.h"

namespace miniSTL{

template <class T, class Alloc = alloc>  //【如果把allocator用simple_alloc封装的话，这里改一下】
class vector{
public:
    using value_type = T;
    using pointer = value_type *;
    using iterator = value_type *;// iterator is raw pointer
    using const_iterator = const value_type *;
    //TODO  [reverse_iterator ]
    // using reverse_iterator = __reverse_iterator<iterator>;
    // using const_reverse_iterator = __reverse_iterator<const_iterator>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

protected:
    using data_allocator = simple_alloc<value_type, Alloc>;
    iterator start;
    iterator finish;
    iterator end_of_storage;

    void insert_aux(iterator position, const T& x);  //【】
    void deallocate() {
        if (start) data_allocator::deallocate(start, end_of_storage - start);
    }    

    void fill_initialize(size_type n, const T& value) {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }
public:
    iterator begin() { return start; }
    const_iterator begin() const { return start; }
    iterator end() { return finish; }
    const_iterator end() const { return finish; }
    //TODO  reverse_iterator
    // reverse_iterator rbegin() { return reverse_iterator(end()); }
    // const_reverse_iterator rbegin() const { 
    //     return const_reverse_iterator(end()); 
    // }
    // reverse_iterator rend() { return reverse_iterator(begin()); }
    // const_reverse_iterator rend() const { 
    //     return const_reverse_iterator(begin()); 
    // }
    size_type size() const { return size_type(end() - begin()); }
    //【好像用不着】 size_type max_size() const { return size_type(-1) / sizeof(T); }
    size_type capacity() const { return size_type(end_of_storage - begin()); }
    bool empty() const { return begin() == end(); }
    reference operator[](size_type n) { return *(begin() + n); }
    const_reference operator[](size_type n) const { return *(begin() + n); }

    
    vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr){}     //默认构造 空vector
    explicit vector(size_type n){fill_initialize(n, value_type());}           //构造n个默认对象
    vector(size_type n, const value_type& value){fill_initialize(n,value);}

    vector(const vector<T, Alloc>& x){
        start = allocate_and_copy
    }

protected:
    iterator allocate_and_fill(size_type n, const T& x) {
        iterator result = data_allocator::allocate(n);
        //TODO 异常处理  commit or rollback
        uninitialized_fill_n(result, n, x);
        return result;
    }
    iterator allocate_and_copy(size_type n, const_iterator first, const_iterator last){
        iterator result = data_allocator::allocate(n);
        //TODO 异常处理
        uninitialized_copy(first, last, result);
        retrun result;
    }
};











}

#endif