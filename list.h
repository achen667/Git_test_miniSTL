#ifndef LIST_H
#define LIST_H

#include "iterator.h"
#include "alloc.h"
#include "construct.h"
namespace miniSTL{

template <class T>
struct _list_node{
    
    _list_node *next;
    _list_node *prev;
    T data;
};

template <class T, class Ref, class Ptr>
struct _list_iterator{
    using iterator = _list_iterator<T, T&, T*>; //【与self有什么不同？】
    using const_iterator = _list_iterator<T, const T&, const T*>;
    using self = _list_iterator<T, Ref, Ptr>;

    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;
    using link_type = _list_node<T>*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    link_type node;

    _list_iterator(link_type x): node(x){}   //【 】
    _list_iterator(){}
    _list_iterator(const iterator& x) : node(x.node) {}

    bool operator==(const self& x) const{ return node == x.node;}
    bool operator!=(const self& x) const { return node != x.node; }
    reference operator*() const { return (*node).data; }

    pointer operator->() const {return &(operator*());}  

    self& operator++(){
        node = (link_type)((*node).next);   //【node->next】
        return *this;
    }
    self operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--() { 
        node = (link_type)((*node).prev);
        return *this;
    }
    self operator--(int) { 
        self tmp = *this;
        --*this;
        return tmp;
    }    
};
template <class T, class Alloc = alloc>
class list{
private:
    using void_pointer = void*;
    using list_node = _list_node<T>;
    using list_node_allocator = simple_alloc<list_node,Alloc>;  //【存储的是list_node】
public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using link_type = list_node*;  //【private?】
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using iterator = _list_iterator<T, T&, T*>;
    using const_iterator = _list_iterator<T, const T&, const T*>;

private:
    /*node是哨兵节点的指针，构造时总会创造一个node指向的哨兵节点对象，
    node->next是第一个数据节点，node本身又作为end()的返回值，即尾后节点
    导致形成环形(双向)链表
    */
    link_type node; 

    link_type get_node() {return list_node_allocator::allocate();}  
    void put_node(link_type p) { list_node_allocator::deallocate(p); }

    link_type creat_node(const T& x){
        link_type p = get_node();
        //TODO commit or rollback
        construct(&p->data, x);
        return p;  
    }
    void destroy_node(link_type p){
        destroy(&(p->data));   //【分两步  ：1.析构 
                                //          2.回收内存 】
        put_node(p);
    }

    void empty_initialize(){
        node = get_node();
        node->next = node;
        node->prev = node;
    }
    void fill_initialize(size_type n, const T& value){
        empty_initialize();
         //TODO commit or rollback
        insert(begin(), n, value);        
    }
    //TODO  其他形式的initialize
    void range_initialize(const_iterator first, const_iterator last){
        //TODO commit or rollback
        inset(begin(), first, last);
    }

    
public:
    list(){empty_initialize();}

    iterator begin() { return (link_type)((*node).next); }
    const_iterator begin() const { return (link_type)((*node).next); }
    iterator end() { return node; }
    const_iterator end() const { return node; }
    //TODO reverse_iterator

    bool empty() const {return node->next == node;}
    size_type size() const {
        size_type result = 0;
        distance(begin(), end(), result);
        return result;
    }
    reference front() {return *begin();}
    const_reference front() const { return *begin(); }
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }
    //仅仅互换dummy节点 
    void swap(list<T,Alloc>& x){ std::swap(node, x.node);}//TODO std::swap
    //在position前插入
    iterator insert(iterator position, const value_type& x){
        link_type new_node = creat_node(x);
        new_node->prev = position.node->prev;
        new_node->next = position.node;
        (link_type(position.node->prev))->next = new_node;
        position.node->prev = new_node;
        return new_node;
    }
    iterator insert(iterator position) { return insert(position, T()); }
    //TODO 多种insert
    void insert(iterator pos,const_iterator first, const_iterator last);
    void insert(iterator pos, size_type n, const T& x);
    void insert(iterator pos, int n, const T& x) {
        insert(pos, (size_type)n, x);
    }
    void insert(iterator pos, long n, const T& x) {
        insert(pos, (size_type)n, x);
    }

    void push_front(const T& x) { insert(begin(), x); }
    void push_back(const T& x) { insert(end(), x); }
    iterator erase(iterator position) {
        link_type next_node = link_type(position.node->next);
        link_type prev_node = link_type(position.node->prev);
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        return iterator(next_node);
    }
    iterator erase(iterator first, iterator last);
    void resize(size_type new_size, const T& x);
    void resize(size_type new_size) { resize(new_size, T()); }
    void clear();

    void pop_front() { erase(begin()); }
    void pop_back() { 
        iterator tmp = end();
        erase(--tmp);
    }

    list(size_type n, const T& value) { fill_initialize(n, value); }
    list(int n, const T& value) { fill_initialize(n, value); }
    list(long n, const T& value) { fill_initialize(n, value); }
    explicit list(size_type n) { fill_initialize(n, T()); }
    list(const list<T,Alloc>& x){
        range_initialize(x.begin(), x.end());
    }
    //TODO list初始化列表构造

    ~list() {
        clear();
        put_node(node);
    }
private:
    void transfer(iterator position, iterator first, iterator last);

public:
    void splice(iterator position, list& x) {
        if (!x.empty()) 
        transfer(position, x.begin(), x.end());
    }
    void splice(iterator position, list&, iterator i) {
        iterator j = i;
        ++j;
        if (position == i || position == j) return;
        transfer(position, i, j);
    }
    void splice(iterator position, list&, iterator first, iterator last) {
        if (first != last) 
        transfer(position, first, last);
    }
    void remove(const T& value);
    void unique();
    void merge(list& x);
    void reverse();
    void sort();
    //TODO void sort(cmp);


    //friend bool operator== (const list& x,const list& y);   // ?
};



template<class T, class Alloc>
void list<T,Alloc>::insert(iterator pos, size_type n, const T& x){
    for( ; n > 0; --n)
        insert(pos, x);
}
template<class T, class Alloc>
void list<T,Alloc>::insert(iterator pos, const_iterator first, const_iterator last){
    for( ; first != last; ++first)
        insert(pos, *first);
}

//将[first,last)中的元素[移动]到position前面 
template <class T, class Alloc>
void list<T,Alloc>::transfer(iterator position, iterator first, iterator last){
    if(position != last){ //判断输入是否合法
        last.node->prev->next = position.node;
        first.node->prev->next = last.node; 
        position.node->prev->next = first.node;
        list_node *temp = position.node->prev;
        position.node->prev = last.node->prev;
        last.node->prev = first.node->prev;
        first.node->prev = temp;
    }
}

template <class T, class Alloc>
typename list<T,Alloc>::iterator list<T, Alloc>::erase(iterator first, iterator last) {
  while (first != last) erase(first++);
  return last;
}

template <class T, class Alloc>
void list<T, Alloc>::resize(size_type new_size, const T& x)
{
  iterator i = begin();
  size_type len = 0;
  for ( ; i != end() && len < new_size; ++i, ++len)
    ;
  if (len == new_size)
    erase(i, end());
  else                          // i == end()
    insert(end(), new_size - len, x);
}

template <class T,class Alloc>
void list<T,Alloc>::clear(){
    link_type cur = (link_type) node->next;
    while (cur != node) {
        link_type tmp = cur;
        cur = (link_type) cur->next;
        destroy_node(tmp);
    }
    node->next = node;
    node->prev = node;
}

template <class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
        iterator next = first;
        ++next;
        if (*first == value) erase(first);
        first = next;
    }
}

template <class T, class Alloc>
void list<T, Alloc>::unique() {
  iterator first = begin();
  iterator last = end();
  if (first == last) return;
  iterator next = first;
  while (++next != last) {
    if (*first == *next)
      erase(next);
    else
      first = next;
    next = first;
  }
}

template <class T, class Alloc>
void list<T, Alloc>::merge(list<T, Alloc>& x){
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while(first1 != last1 && first2 != last2){
        if(*first2 < *first1){
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
        }
        else
            ++first1;
    }
    if(first2 != last2) transfer(last1, first2, last2);
}

template <class T, class Alloc>
void list<T, Alloc>::reverse() {
    if (node->next == node || link_type(node->next)->next == node) return;
    iterator first = begin();
    ++first;
    while (first != end()) {
        iterator old = first;
        ++first;
        transfer(begin(), old, first);
    }
}    

template <class T, class Alloc>
void list<T, Alloc>::sort() {
    //归并排序 
    if (node->next == node || link_type(node->next)->next == node) return;
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    
    int fill = 0;  //counter中最大索引值 counter[fill]中的元素个数为：2^fill
    while ( !empty()){
        int i = 0;
        carry.splice(carry.begin(), *this, begin()); //取1个元素
        while( i < fill && !counter[i].empty()){
            counter[i].merge(carry); //对于同是升序的序列 merge后仍是升序的  
            carry.swap(counter[i++]);  //准备将排序好的 2^(i+1)个元素与 counter[i+1]中的元素merge排序
        }
        counter[i].swap(carry);  //将最新的排序结果存入counter[fill] （fill等于i)
        if(i == fill)  fill ++;
    }
    for (int i = 1; i < fill; ++i) 
        counter[i].merge(counter[i-1]);
    swap(counter[fill-1]);
    
}

template <class T, class Alloc>
bool operator==(const list<T,Alloc>& x, const list<T,Alloc>& y){
    typename list<T,Alloc>::const_iterator it1 = x.begin(),it2 = y.begin();
    for( ; it1 != x.end() && it2 != y.end(); ++it1, ++it2){
        if(*it1 != *it2)    return false;
    }
    return it1 == x.end() && it2 == y.end();
}
template <class T,class Alloc>
inline bool operator!=(const list<T> &x, const list<T> &y){
    return !(x == y);
}
template <class T,class Alloc>
inline bool operator<(const list<T> &x, const list<T> &y){
    return std::lexicographical_compare(x.begin(),x.end(),y.begin(),y.end()); //TODO std::lexicographical_compare
}

template <class T,class Alloc>
inline bool operator>(const list<T> &x, const list<T> &y){
    return y < x;
}

template <class T,class Alloc>
inline bool operator<=(const list<T> &x, const list<T> &y){
    return !(y < x);
}
template <class T,class Alloc>
inline bool operator>=(const list<T> &x, const list<T> &y){
    return !(x < y);
}




}


#endif