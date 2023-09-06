#ifndef SET_H
#define SET_H

#include "iterator.h"
#include "construct.h"
#include "alloc.h"
#include "uninitialized.h"
#include "function.h"
#include <functional> //【std】
#include "tree.h"

namespace miniSTL{

template <class Key, class Compare = std::less<Key>, class Alloc = alloc>
class set{
public:
    using key_type = Key;
    using value_type = Key;
    using key_compare = Compare;
    using value_compare = Compare;
private:
    using rep_type = rb_tree<key_type, value_type, identity<value_type>, key_compare, Alloc>;
    rep_type t;

public:
    using pointer = typename rep_type::const_pointer;
    using const_pointer = typename rep_type::const_pointer;
    using reference = typename rep_type::const_reference;
    using const_reference =typename rep_type::const_reference ;
    using iterator = typename rep_type::const_iterator ;
    using const_iterator = typename rep_type::const_iterator ;
    //TODO reverse
    // using reverse_iterator = typename rep_type::const_reverse_iterator ;
    // using const_reverse_iterator = typename rep_type::const_reverse_iterator ;
    using size_type = typename rep_type::size_type ;
    using difference_type = typename rep_type::difference_type ;

    set() : t(Compare()){}
    explicit set(const Compare& comp) : t(comp){}

    template <class InputIterator>
    set(InputIterator first, InputIterator last)
        :t(Compare()){ t.insert_unique(first,last);}
    template <class InputIterator>
    set(InputIterator first, InputIterator last, const Compare& comp)
        :t(comp){ t.insert_unique(first,last);}
    set(const set<Key, Compare, Alloc>& x) : t(x.t){}
    
    set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc> x){
        t = x.t;
        return *this;
    }
        
    key_compare key_comp() const { return t.key_comp();}
    value_compare value_comp() const { return t.key_comp();}
    iterator begin() const { return t.begin();}
    iterator end() const { return t.end(); }
    //TODO reverse iterator
    // reverse_iterator rbegin() const { return t.rbegin(); } 
    // reverse_iterator rend() const { return t.rend(); }
    bool empty() const { return t.empty(); }
    size_type size() const { return t.size(); }
    size_type max_size() const { return t.max_size(); }
    void swap(set<Key, Compare, Alloc>& x) { t.swap(x.t); }    

    using pair_iterator_bool = std::pair<iterator, bool> ; //TODO pair
    std::pair<iterator,bool> insert(const value_type& x){
        return t.insert_unique(x);
    }

    iterator insert(iterator position, const value_type& x){
        //rebalance可能需要改变迭代器所指节点颜色，需要转型至非const
        using rep_iterator = typename rep_type::iterator ; 
        return t.insert_unique((rep_iterator&)position, x);  //TODO reinterpret_cast<rep_iterator &>  以及其他的容器里的转型
    }
    void insert(const_iterator first, const_iterator last) {
        t.insert_unique(first, last);
    }
    void insert(const value_type* first, const value_type* last) {
        t.insert_unique(first, last);
    }  

    void erase(iterator position) { 
        using rep_iterator = typename rep_type::iterator ;
        t.erase((rep_iterator&)position); //TODO 
    }
    size_type erase(const key_type& x) { 
        return t.erase(x); 
    }
    void erase(iterator first, iterator last) { 
        using rep_iterator = typename rep_type::iterator ;
        t.erase((rep_iterator&)first, (rep_iterator&)last); //TODO 
    }
    void clear() { t.clear(); } 

    iterator find(const key_type& x) const { return t.find(x); }
    size_type count(const key_type& x) const { return t.count(x); }
    iterator lower_bound(const key_type& x) const {
        return t.lower_bound(x);
    }
    iterator upper_bound(const key_type& x) const {
        return t.upper_bound(x); 
    }
    pair<iterator,iterator> equal_range(const key_type& x) const {
        return t.equal_range(x);
    }
};

template <class Key, class Compare, class Alloc>
inline bool operator==(const set<Key, Compare, Alloc>& x, 
                       const set<Key, Compare, Alloc>& y) {
    return x.t == y.t;
}

template <class Key, class Compare, class Alloc>
inline bool operator<(const set<Key, Compare, Alloc>& x, 
                      const set<Key, Compare, Alloc>& y) {
    return x.t < y.t;
}



}


#endif