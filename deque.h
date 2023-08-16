#ifndef DEQUE_H
#define DEQUE_H

#include "iterator.h"
#include "construct.h"
#include "alloc.h"
#include "uninitialized.h"


namespace miniSTL
{
    
//缓冲区可存储的元素个数
inline size_t _deque_buf_size(size_t n, size_t sz)
{
  return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

template <class T, class Ref, class Ptr, size_t BufSiz>
struct _deque_iterator
{
    using iterator = _deque_iterator<T, T&, T*, BufSiz>;
    using const_iterator = _deque_iterator<T, const T&, const T*, BufSiz>;
    static size_t buffer_size(){return _deque_buf_size(BufSiz, sizeof(T));}
    
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using pointer = Ptr;
    using reference = Ref;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using map_pointer = T**;

    using self = _deque_iterator;

    //采用整体模拟的连续存储，借助了部分连续存储的缓冲区，通过map营造整体连续存储的假象
    //以下成员为指向缓冲区的指针，在缓冲区内对原生指针操作

    T* cur;
    T* first;
    T* last;
    map_pointer node; //指向map中的一个节点 节点指向缓冲区
public:
    //【构造】
    _deque_iterator(T* x, map_pointer y) 
        : cur(x), first(*y), last(*y + buffer_size()), node(y) {}
    _deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
    _deque_iterator(const iterator& x)
        : cur(x.cur), first(x.first), last(x.last), node(x.node) {}

    reference operator*() const {return *cur;}
    pointer operator->() const {return &(operator*());}

    //切换迭代器所指node    
    void set_node(map_pointer new_node){
        node = new_node;
        first = *new_node;
        last = first + difference_type(buffer_size());
    }

    //计算迭代器之间距离
    difference_type operator-(const self& x) const{
        return difference_type(buffer_size()) * (node - x.node - 1) +
        (cur - first) + (x.last - x.cur);
    }
    //自增自减1步
    self& operator++(){
        ++cur; //连续存储
        if(cur == last){
            set_node(++node);
            cur = first;
        }
        return *this;
    }
    self operator++(int){
        self tmp = *this;
        ++*this;
        return tmp;
    }
    self& operator--(){
        if(cur == first){
            set_node(--node);
            cur = last;
        }
    }
    self operator--(int) {
        self tmp = *this;   
        --*this;
        return tmp;
    }
    //自增自减n步
    self& operator+=(difference_type n){
        difference_type offset = n + (cur - first);
        if (offset >= 0 && offset < difference_type(buffer_size()))
        cur += n;
        else {
        difference_type node_offset =
            offset > 0 ? offset / difference_type(buffer_size())
                    : -difference_type((-offset - 1) / buffer_size()) - 1;
        set_node(node + node_offset);
        cur = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }

    //迭代器运算
    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }
    self& operator-=(difference_type n) { return *this += -n; }
 
    self operator-(difference_type n) const {
        self tmp = *this;
        return tmp -= n;
    }

    reference operator[] (difference_type n) const {return *(*this + n);}
    bool operator==(const self& x) const { return cur == x.cur; }
    bool operator!=(const self& x) const { return !(*this == x); }
    bool operator<(const self& x) const {
        return (node == x.node) ? (cur < x.cur) : (node < x.node);
    }
};

template <class T, class Alloc = alloc, size_t BufSiz = 0>
class deque{
public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using iterator = _deque_iterator<T, T&, T*, BufSiz>;
    using const_iterator = _deque_iterator<T, const T&, const T*, BufSiz>;

    //TODO reverse_iterator

private:  
    using map_pointer = pointer*;  //指向map, map数组中存储指向缓冲区的指针
    using data_allocator = simple_alloc<value_type, Alloc>;
    using map_allocator = simple_alloc<pointer, Alloc>;

    static size_type buffer_size(){
        return _deque_buf_size(BufSiz, sizeof(value_type));
    }
    static size_type initial_map_size() {return 8;}

    iterator start;
    iterator finish;
    map_pointer map;
    size_type map_size;

public:
    iterator begin() { return start; }
    iterator end() { return finish; }
    const_iterator begin() const { return start; }
    const_iterator end() const { return finish; }
    //TODO reverse_iterator rbegin() 

    reference operator[](size_type n) { return start[difference_type(n)];}
    reference front() { return *start;}
    reference back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    const_reference front() const { return *start; }
    const_reference back() const {
        const_iterator tmp = finish;
        --tmp;
        return *tmp;
    }

    size_type size() const { return finish - start;}
    bool empty() const {return finish = start;}
    
    deque()
        : start(), finish(), map(nullptr), map_size(0)
    {
        create_map_and_nodes(0);
    }

    deque(const deque& x)
        : start(), finish(), map(nullptr), map_size(0)
    {
        create_map_and_nodes(x.size());
        //TODO commit or rollback
        uninitialized_copy(x.begin(), x.end(), start);
    }

    deque(size_type n, const value_type& value)
        : start(), finish(), map(nullptr), map_size(0)
    {
        fill_initialize(n, value);
    }
    explicit deque(size_type n)
        : start(), finish(), map(0), map_size(0)
    {
        fill_initialize(n, value_type());
    }
    deque(const_iterator first, const_iterator last)
        : start(), finish(), map(nullptr), map_size(0)
    {
        create_map_and_nodes(last - first);
        //TODO commit or rollback
        uninitialized_copy(first, last, start);
    }

    ~deque(){
        destroy(start, finish);
        destroy_map_and_nodes();
    }

    deque& operator= (const deque& x){
        
    }

private:
    void create_map_and_nodes(size_type num_elements);
    void destroy_map_and_nodes();
    void fill_initialize(size_type n, const value_type& value);

private:
    //分配一个缓冲区
    pointer allocate_node() { return data_allocator::allocate(buffer_size());}
    void deallocate_node(pointer n){ //n: node
        data_allocator::deallocate(n, buffer_size());
    }
};


template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::create_map_and_nodes(size_type num_elements){
    size_type num_nodes = num_elements / buffer_size() + 1;

    map_size = std::max(initial_map_size(), num_nodes + 2);
    map = map_allocator::allocate(map_size);

    //从中间向两边扩充   |oooooo | xxxx |oooooo|
    //                       nstart  nfinish
    map_pointer nstart = map + (map_size - num_nodes) / 2; 
    map_pointer nfinish = nstart + num_nodes - 1;

    map_pointer cur;
    //TODO commit or rollback
    for(cur = nstart; cur <= nfinish; ++cur)
        *cur = allocate_node();  //data_allocator::allocate()
    
    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;  //【first值根据deque构造方式决定】
    finish.cur = finish.first + num_elements % buffer_size();
}
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::destroy_map_and_nodes(){
    for(map_pointer cur = start.node; cur <= finish.node; ++cur)
        deallocate_node(*cur);
    map_allocator::deallocate(map, map_size);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::fill_initialize(size_type n,
                                            const value_type& value) {
    create_map_and_nodes(n);
    // iterator tmp = start;
    // for(; tmp != finish; ++tmp){
    //     *tmp->cur = value;
    // }
    map_pointer cur;
    //TODO commit or rollback
    for( cur = start.node; cur < finish.node; ++cur){
        miniSTL::uninitialized_fill(*cur, *cur + buffer_size(), value);
    }
    miniSTL::uninitialized_fill(finish.first, finish.cur, value);
}









} // namespace miniSTL


#endif











