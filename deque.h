#ifndef DEQUE_H
#define DEQUE_H

#include "iterator.h"
#include "construct.h"
#include "alloc.h"
#include "uninitialized.h"

//TODO deque::swap
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
        const size_type len = size();
        if(&x != this){
            if(len > x.size())
                erase(std::copy(x.begin(), x.end(), start), finish);  //TODO std::copy
            else{
                const_iterator mid = x.begin() + difference_type(len);
                std::copy(x.begin(), mid, start);
                insert(finish, mid, x.end());
            }
        }
    }

    void push_back(const value_type& t){
        if (finish.cur != finish.last - 1) {
            construct(finish.cur, t);
            ++finish.cur;
        }
        else
            push_back_aux(t);
    }
    void push_front(const value_type& t) {
        if (start.cur != start.first) {
            construct(start.cur - 1, t);
            --start.cur;
        }
        else
            push_front_aux(t);
    }   

    void pop_back(){
        if(finish.cur != finish.first){   //finish缓冲区至少还剩一个
            --finish.cur;
            destroy(finish.cur);  //析构 
                                  //但不回收内存
        }
        else  //pop 上一个缓冲区中的最后一个
            pop_back_aux();
    }
    void pop_front() {
        if (start.cur != start.last - 1) {    //start缓冲区至少还剩一个
            destroy(start.cur);
            ++start.cur;
        }
        else 
            pop_front_aux();
    }

    iterator insert(iterator position, const value_type& x){
        if (position.cur == start.cur) {
            push_front(x);
            return start;
        }
        else if (position.cur == finish.cur) {
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        }
        else {
            return insert_aux(position, x);
        }        
    }
    iterator insert(iterator position){ return insert(position, value_type());  }
    void insert(iterator position, const_iterator first, const_iterator last);
    void insert(iterator position, size_type n, const value_type& x);


    iterator erase(iterator pos){
        iterator next = pos;
        next++;
        difference_type index = pos - start;
        if(index < (size() >> 1)){
            //TODO std::copy_backward
            std::copy_backward(start, pos, next);  //向右（接近中心）拷贝
            pop_front();
        }
        else{
            std::copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }
    iterator erase(iterator first, iterator last);
    void clear();
private:
    void create_map_and_nodes(size_type num_elements);
    void destroy_map_and_nodes();
    void fill_initialize(size_type n, const value_type& value);


    iterator insert_aux(iterator pos, const value_type& x);
    void insert_aux(iterator pos, size_type n, const value_type& x);
    void insert_aux(iterator pos, const_iterator first, const_iterator last,
                     size_type n); // 【?】
    
    void push_back_aux(const value_type& t);
    void push_front_aux(const value_type& t);
    void pop_back_aux();
    void pop_front_aux();

private:

    iterator reserve_elements_at_front(size_type n){
        size_type vacancies = start.cur - start.first;
        if(n > vacancies)
            new_elements_at_front(n - vacancies);
        return start - difference_type(n);
    }
    iterator reserve_elements_at_back(size_type n) {
        size_type vacancies = (finish.last - finish.cur) - 1;
        if (n > vacancies)
            new_elements_at_back(n - vacancies);
        return finish + difference_type(n);
    }

    void reserve_map_at_back (size_type nodes_to_add = 1) {
        if (nodes_to_add + 1 > map_size - (finish.node - map))
            reallocate_map(nodes_to_add, false);
    }

    void reserve_map_at_front (size_type nodes_to_add = 1) {
        if (nodes_to_add > start.node - map)
            reallocate_map(nodes_to_add, true);
    }
    void new_elements_at_front(size_type new_elements);
    void new_elements_at_back(size_type new_elements);

    //分配一个缓冲区
    pointer allocate_node() { return data_allocator::allocate(buffer_size());}
    void deallocate_node(pointer n){ //n: node
        data_allocator::deallocate(n, buffer_size());
    }
    void reallocate_map(size_type nodes_to_add, bool add_at_front);

 
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


template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator 
deque<T, Alloc, BufSize>::erase(iterator first, iterator last){
    iterator next = last;
    next++;
    if(first == start && last == finish){
        clear();
        return finish;
    }
    else{
        difference_type n = last - first;
        difference_type elems_before = first - start;
        if(elems_before < (size() - n) / 2){
            std::copy_backward(start, first, last);
            iterator new_start = start + n;
            destroy(start, new_start);
            for(map_pointer cur = start.node; cur < new_start.node; ++cur)
                data_allocator::deallocate(*cur, buffer_size());
            start = new_start;
        }
        else {
            std::copy(last, finish, first);
            iterator new_finish = finish - n;
            destroy(new_finish, finish);
            for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur)
                data_allocator::deallocate(*cur, buffer_size());
            finish = new_finish;
        }
            return start + elems_before;
    }
     
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::clear() {
    for (map_pointer node = start.node + 1; node < finish.node; ++node) {
        destroy(*node, *node + buffer_size());
        data_allocator::deallocate(*node, buffer_size());
    }

    if (start.node != finish.node) {
        destroy(start.cur, start.last);
        destroy(finish.first, finish.cur);
        data_allocator::deallocate(finish.first, buffer_size());
    }
    else
        destroy(start.cur, finish.cur);
        
    finish = start;
}

//finish只剩一个元素备用空间
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_back_aux(const value_type& t) {
    value_type t_copy = t;
    reserve_map_at_back();
    *(finish.node + 1) = allocate_node();  
    //TODO commit or rollback
    construct(finish.cur, t_copy);  //【finish.cur 指向尾后元素】
    finish.set_node(finish.node + 1);
    finish.cur = finish.first;
}
//start已满
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::push_front_aux(const value_type& t) {
    value_type t_copy = t;
    reserve_map_at_front();
    *(start.node - 1) = allocate_node();
    //TODO
    start.set_node(start.node - 1);
    start.cur = start.last - 1;
    construct(start.cur, t_copy);
} 

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>:: pop_back_aux(){
    deallocate_node(finish.first);   
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    destroy(finish.cur);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::pop_front_aux() {
    destroy(start.cur);
    deallocate_node(start.first);
    start.set_node(start.node + 1);
    start.cur = start.first;
}      

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      size_type n, const value_type& x) {
    if (pos.cur == start.cur) {
        iterator new_start = reserve_elements_at_front(n);
        uninitialized_fill(new_start, start, x);
        start = new_start;
    }
    else if (pos.cur == finish.cur) {
        iterator new_finish = reserve_elements_at_back(n);
        uninitialized_fill(finish, new_finish, x);
        finish = new_finish;
    }
    else 
        insert_aux(pos, n, x);
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert(iterator pos,
                                      const_iterator first,
                                      const_iterator last)
{
    size_type n = last - first;
    if (pos.cur == start.cur) {
        iterator new_start = reserve_elements_at_front(n);
        //TODO commit or rollback
        uninitialized_copy(first, last, new_start);
        start = new_start;
        
    }
    else if (pos.cur == finish.cur) {
        iterator new_finish = reserve_elements_at_back(n);
        
        uninitialized_copy(first, last, finish);
        finish = new_finish;
        
    }
    else
        insert_aux(pos, first, last, n);
}


template <class T, class Alloc, size_t BufSize>
typename deque<T, Alloc, BufSize>::iterator
deque<T, Alloc, BufSize>::insert_aux(iterator pos, const value_type& x){
    difference_type index = pos - start;
    value_type x_copy = x;
    if(index < size() / 2){
        push_front(front());
        iterator front1 = start;  
        ++front1;    //copy目的地 front()的下一个
        iterator front2 = front1; 
        ++front2;    //copy起点   front()的下下一个

        pos = start + index;  // pos最后赋值用
        iterator pos1 = pos;  
        ++pos1;               //copy终点
        std::copy(front2, pos1, front1);
    }
    else{
        push_back(back());
        iterator back1 = finish;
        --back1;
        iterator back2 = back1;
        --back2;
        //pos = start + index;
        std::copy_backward(pos, back2, back1);
    }
    *pos = x_copy;
    return pos;
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos, size_type n, const value_type& x){
    const difference_type elems_before = pos - start;
    size_type length = size();
    value_type x_copy = x;
    //TODO commit or rollback
    if(elems_before < length / 2){
        iterator new_start = reserve_elements_at_front(n);
        iterator old_start = start;
        if(elems_before >= difference_type(n)){ 
            iterator start_n = start + difference_type(n);
            uninitialized_copy(start, start_n, new_start); //针对未初始化的内存，可能执行对内置类型的优化
            start = new_start;
            std::copy(start_n, pos, old_start);            //针对已初始化的内存
            std::fill(pos - difference_type(n), pos, x_copy);  //TODO: STD:filllllll
        }
        else{
            _uninitialized_copy_fill(start, pos, new_start, start, x_copy);
            start = new_start;
            std::fill(old_start, pos, x_copy);
        }
    }
    else{
        iterator new_finish = reserve_elements_at_back(n);
        iterator old_finish = finish;
        const difference_type elems_after = difference_type(length) - elems_before;
        pos = finish - elems_after;
        
        if (elems_after > difference_type(n)) {
            iterator finish_n = finish - difference_type(n);
            uninitialized_copy(finish_n, finish, finish);
            finish = new_finish;
            std::copy_backward(pos, finish_n, old_finish);
            std::fill(pos, pos + difference_type(n), x_copy);
        }
        else {
            _uninitialized_fill_copy(finish, pos + difference_type(n),
                                    x_copy,
                                    pos, finish);
            finish = new_finish;
            std::fill(pos, old_finish, x_copy);
        }
       
    }
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::insert_aux(iterator pos,
                                          const_iterator first,
                                          const_iterator last,
                                          size_type n)
{
    const difference_type elems_before = pos - start;
    size_type length = size();
    if (elems_before < length / 2) {
        iterator new_start = reserve_elements_at_front(n);
        iterator old_start = start;
        pos = start + elems_before;
        if (elems_before >= n) {
            iterator start_n = start + n;
            uninitialized_copy(start, start_n, new_start);
            start = new_start;
            std::copy(start_n, pos, old_start);
            std::copy(first, last, pos - difference_type(n));
        }
        else {
            const_iterator mid = first + (n - elems_before);
            _uninitialized_copy_copy(start, pos, first, mid, new_start);
            start = new_start;
            std::copy(mid, last, old_start);
        }
    }
    else {
        iterator new_finish = reserve_elements_at_back(n);
        iterator old_finish = finish;
        const difference_type elems_after = length - elems_before;
        pos = finish - elems_after;
        
        if (elems_after > n) {
            iterator finish_n = finish - difference_type(n);
            uninitialized_copy(finish_n, finish, finish);
            finish = new_finish;
            std::copy_backward(pos, finish_n, old_finish);
            std::copy(first, last, pos);
        }
        else {
            const_iterator mid = first + elems_after;
            _uninitialized_copy_copy(mid, last, pos, finish, finish);
            finish = new_finish;
            std::copy(first, mid, pos);
        }
    }
}

template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_front(size_type new_elements) {
    size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
    reserve_map_at_front(new_nodes);
    size_type i;
    //TODO commit or rollback
    for(i = 1; i <= new_nodes; ++i)
        *(start.node - i) = allocate_node();
}
template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::new_elements_at_back(size_type new_elements) {
    size_type new_nodes = (new_elements + buffer_size() - 1) / buffer_size();
    reserve_map_at_back(new_nodes);
    size_type i;
    //TODO commit or rollback
    for (i = 1; i <= new_nodes; ++i)
        *(finish.node + i) = allocate_node();
    
}


template <class T, class Alloc, size_t BufSize>
void deque<T, Alloc, BufSize>::reallocate_map(size_type nodes_to_add,bool add_at_front) {
    size_type old_num_nodes = finish.node - start.node + 1;
    size_type new_num_nodes = old_num_nodes + nodes_to_add;

    map_pointer new_nstart;
    //若添加元素后容器已用空间达到50%以上 则扩容至至少为当前容量的两倍
    if (map_size > 2 * new_num_nodes) {
        new_nstart = map + (map_size - new_num_nodes) / 2 
                        + (add_at_front ? nodes_to_add : 0);
        if (new_nstart < start.node)
        std::copy(start.node, finish.node + 1, new_nstart);//TODO: std::copy
        else
        std::copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
    }
    else{
        size_type new_map_size = map_size + std::max(map_size, nodes_to_add) + 2;
        map_pointer new_map = map_allocator::allocate(new_map_size);
        new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                         + (add_at_front ? nodes_to_add : 0);
        std::copy(start.node, finish.node + 1, new_nstart);
        map_allocator::deallocate(map, map_size);

        map = new_map;
        map_size = new_map_size;
    }
    
    start.set_node(new_nstart);
    finish.set_node(new_nstart + old_num_nodes - 1);
}


} // namespace miniSTL


#endif











