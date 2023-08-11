#ifndef VECTOR_H
#define VECTOR_H

#include "alloc.h"
#include "construct.h"

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

    void insert_aux(iterator position, const T& x);  //扩容
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

    vector(const vector<T, Alloc>& x){    //【直接改成vector& x 也行的吧  剩下的也一样 如果改的话】
        start = allocate_and_copy(x.end() - x.begin(), x.begin(), x.end());
        finish = start + (x.end() - x.begin());
        end_of_storage = finish;
    }
    //TODO && vector移动初始化

    ~vector(){
        destroy(start, finish);
        deallocate();
    }

    vector& operator=(const vector& x); 
    void reserve(size_type n );//TODO vector::reserve
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference back() const { return *(end() - 1); }
    void push_back(const value_type & x){
        if(finish != end_of_storage){
            construct(finish, x);
            ++ finish;
        }
        else    
            insert_aux(end(), x);
    }
    iterator insert(iterator position, const value_type& x){
        size_type n = position - begin();
        //如果无需扩容且将在尾部插入
        if(finist != end_of_storage && position == end()){
            construct(finish, x);
            ++finish;
        }else
            insert_aux(finish,x);
        return begin() + n;
    }
    void pop_back(){
        --finish;
        destroy(finish);
    }


    //插入n个元素
    void insert(iterator pos, size_type n, const value_type& x); //不适合inline 所以只在类内声明
    void insert(iterator pos, int n, const value_type& x){
        insert(pos, (size_type) n, x);
    }
    void insert(iterator pos, long n, const value_type& x){
        insert(pos, (size_type) n, x);
    }    

    iterator erase(iterator position){
        // if(position + 1 != end())
        //     std::copy(position + 1, finish, position);
        // --finish;
        // destroy(finish);
        // return position;

        erase(position,position + 1);  // :)
    }
    iterator erase(iterator first, iterator last){
        iterator i = std::copy(last, finish, first);
        destroy(i, finish);
        finish = finish - (last - first);
        return first;
    }
    void resize(size_type new_size, const value_type& x){
        if(new_size < size())
            erase(begin() + new_size, end());
        else    
            insert(end(), new_size - size(), x);  //【】
    }
    void resize(size_type new_size) { resize(new_size, value_type()); }
    void clear() { erase(begin(), end()); }

protected:
    iterator allocate_and_fill(size_type n, const T& x) {
        iterator result = data_allocator::allocate(n);
        //TODO 异常处理  commit or rollback
        uninitialized_fill_n(result, n, x);
        return result;
    }
    iterator allocate_and_copy(size_type n, const_iterator first, const_iterator last){
        //TODO【这里allocate n个字节（bytes），需要n个元素的内存，但vector的一个元素不一定是一个字节啊？ 啥情况】 
        iterator result = data_allocator::allocate(n);
        //TODO 异常处理
        uninitialized_copy(first, last, result);
        return result;
    }
};


//重载运算符模板   
template<class T, class Alloc>
inline bool operator==(const vector<T, Alloc>& x, const vector<T,Alloc>& y){
    return x.size == y.size() && std::equal(x.begin(), x.end(), y.begin());
} //TODO  std::equal

template<class T, class Alloc>
inline bool operator<(const vector<T, Alloc>& x, const vector<T,Alloc>& y){
    return std::lexicographical_compare(x.begin, x.end(), y.begin(), y.end());
}   //TODO  std::lexicographical_compare    ....


template<class T, class Alloc>
vector<T,Alloc>& vector<T,Alloc>::operator=(const vector<T, Alloc>& x){
      if (&x != this) {
    if (x.size() > capacity()) {
      iterator tmp = allocate_and_copy(x.end() - x.begin(),
                                       x.begin(), x.end());
      destroy(start, finish);
      deallocate();
      start = tmp;
      end_of_storage = start + (x.end() - x.begin());
    }
    else if (size() >= x.size()) {
      iterator i = copy(x.begin(), x.end(), begin());
      destroy(i, finish);
    }
    else {
      copy(x.begin(), x.begin() + size(), start);
      uninitialized_copy(x.begin() + size(), x.end(), finish);
    }
    finish = start + x.size();
  }
  return *this;
}
//TODO【 移动赋值】
template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {

}








}

#endif