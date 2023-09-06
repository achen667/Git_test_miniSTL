#ifndef TREE_H
#define TREE_H

#include "iterator.h"
#include "construct.h"
#include "alloc.h"
#include "uninitialized.h"
#include "function.h"

namespace miniSTL{

typedef bool _rb_tree_color_type;
const _rb_tree_color_type _rb_tree_red = false;
const _rb_tree_color_type _rb_tree_black = true;

//储存树枝、颜色信息
struct _rb_tree_node_base
{
    using color_type = _rb_tree_color_type;
    using base_ptr = _rb_tree_node_base*;

    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;
    static base_ptr minimum(base_ptr x)
    {
        while (x->left != nullptr) x = x->left;
        return x;
    }
    static base_ptr maximum(base_ptr x)
    {
        while (x->right != nullptr) x = x->right;
        return x;
    }
};

//储存节点值  继承base
template <class Value>
struct  _rb_tree_node : public _rb_tree_node_base
{
    using link_type = _rb_tree_node<Value>*;
    Value value_field;
};

struct _rb_tree_base_iterator
{
    using base_ptr = _rb_tree_node_base::base_ptr;  //只操作父子关系和颜色
    using iterator_category = bidirectional_iterator_tag;
    using difference_type = ptrdiff_t;
    //【唯一成员变量】
    base_ptr node;   //_rb_tree_node_base*

    void increment()  //【最大值的节点调用 则返回header】
    {
        if (node->right != nullptr) {
            node = node->right;
            while (node->left != 0)
                node = node->left;
        }
        else { 
            base_ptr p = node->parent;
            while (node == p->right) {
                node = p;
                p = p->parent;
        }
        if (node->right != p)
            node = p;
        }
    }
    void decrement() 
    {
        if (node->left != nullptr)
        {
            node = node->left;
            while (node->right){
                node = node->right;
            }
        }
        else {
            base_ptr p = node->parent;
            while(node == p->left){
                node = p;
                p = p->parent;
            }
            node =  p;
        }
    }
};
template <class Value, class Ref, class Ptr>
struct _rb_tree_iterator : public _rb_tree_base_iterator
{
    using value_type = Value ;
    using reference = Ref ;
    using pointer = Ptr ;
    using iterator = _rb_tree_iterator<Value, Value&, Value*>  ;
    using const_iterator = _rb_tree_iterator<Value, const Value&, const Value*> ;
    using self = _rb_tree_iterator<Value, Ref, Ptr> ;
    using link_type = _rb_tree_node<Value>* ; 
    _rb_tree_iterator() {}
    _rb_tree_iterator(link_type x) { node = x; }  //【一个子类指针赋值给父类指针（正确）】
    _rb_tree_iterator(const iterator& it) { node = it.node; }
    //【将父类指针转换成子类指针（危险），但其原本就是子类指针转换过来的】
    reference operator*() const { return link_type(node)->value_field; }  
    pointer operator->() const { return &(operator*()); }

    self& operator++() { increment(); return *this; }
    self operator++(int) {
        self tmp = *this;
        increment();
        return tmp;
    }
        
    self& operator--() { decrement(); return *this; }
    self operator--(int) {
        self tmp = *this;
        decrement();
        return tmp;
    }


};

inline bool operator==(const _rb_tree_base_iterator& x,
                       const _rb_tree_base_iterator& y) {
  return x.node == y.node;
}

inline bool operator!=(const _rb_tree_base_iterator& x,
                       const _rb_tree_base_iterator& y) {
  return x.node != y.node;
}

inline void 
_rb_tree_rotate_left(_rb_tree_node_base* x, _rb_tree_node_base*& root){
    //右子树要上提，右子树的左子要接到旋转点上,右子树的右子不用动
    _rb_tree_node_base* y = x->right;
    x->right = y->left;
    if(y->left != nullptr)
        y->left->parent = x;
    //处理右子树
    if(x == root)
        root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}
inline void 
_rb_tree_rotate_right(_rb_tree_node_base* x, _rb_tree_node_base*& root){
    //镜像操作
    _rb_tree_node_base* y = x->left;
    x->left = y->right;
    if (y->right != nullptr)
        y->right->parent = x;
    y->parent = x->parent;

    if (x == root)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;
    y->right = x;
    x->parent = y;    
}
inline void 
_rb_tree_rebalance(_rb_tree_node_base* x, _rb_tree_node_base*& root){
    x->color = _rb_tree_red;  //不插入黑节点是为了不打破规则四（黑节点数相同）
    //重新平衡的任务是遵守规则三（红接黑），即调整节点颜色。
    while(x != root && x->parent->color == _rb_tree_red){ //父为红
        if(x->parent == x->parent->parent->left){ //外侧插入
            _rb_tree_node_base* y = x->parent->parent->right;  
            if(y && y->color == _rb_tree_red){ //伯父为红(父也为红)
                x->parent->color = _rb_tree_black; //改父、伯父为黑
                y->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red; //改祖父为红
                x = x->parent->parent; //祖父颜色被修改，为了遵守规则三，所以从祖父开始 继续上溯 while循环
            }
            else{
                if(x == x->parent->right){ //【试一下】
                    x = x->parent;
                    _rb_tree_rotate_left(x, root);
                }
                x->parent->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                _rb_tree_rotate_right(x->parent->parent, root);
            }
        }
        else{//镜像操作  内侧插入
            _rb_tree_node_base* y = x->parent->parent->left;
            if (y && y->color == _rb_tree_red) {
                x->parent->color = _rb_tree_black;
                y->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                x = x->parent->parent;
            }
            else {
                if (x == x->parent->left) {
                x = x->parent;
                _rb_tree_rotate_right(x, root);
                }
                x->parent->color = _rb_tree_black;
                x->parent->parent->color = _rb_tree_red;
                _rb_tree_rotate_left(x->parent->parent, root);
            }
        }
        
    }
    root->color = _rb_tree_black;
}

inline _rb_tree_node_base*
_rb_tree_rebalance_for_erase( _rb_tree_node_base* z,
                              _rb_tree_node_base*& root,
                              _rb_tree_node_base*& leftmost,
                              _rb_tree_node_base*& rightmost)
{
    _rb_tree_node_base* y = z;
    _rb_tree_node_base* x = nullptr;
    _rb_tree_node_base* x_parent = nullptr;
    if (y->left == nullptr)             // z has at most one non-null child. y == z.
        x = y->right;                   // x might be null.
    else
        if (y->right == nullptr)        // z has exactly one non-null child.  y == z.
            x = y->left;                // x is not null.
        else {                          // z has two non-null children.  Set y to
            y = y->right;               //   z's successor.  x might be null.
            while (y->left != nullptr)
                y = y->left;
            x = y->right;
        }
    if (y != z) {                 //z有左右子  relink y in place of z.  y is z's successor
        z->left->parent = y;      
        y->left = z->left;
        //安置y的子树  (如果z->right 还有左子)
        if (y != z->right) {        
            x_parent = y->parent;
            if (x) x->parent = y->parent;
            y->parent->left = x;      // y must be a left child
            //安置y（y与其子）
            y->right = z->right;        
            z->right->parent = y;
        }
        else
            x_parent = y;
        //安置y（y与其父）
        if (root == z)
            root = y;
        else if (z->parent->left == z) //z是左子
            z->parent->left = y;
        else                           //z是右子
            z->parent->right = y;
        y->parent = z->parent;
        std::swap(y->color, z->color);
        y = z;
        // y now points to node to be actually deleted
    }
    else {                        //z最多有一个左子 y == z
        x_parent = y->parent;
        if (x) x->parent = y->parent;   
        if (root == z)
            root = x;
        else 
            if (z->parent->left == z)
                z->parent->left = x;
            else
                z->parent->right = x;
        if (leftmost == z) 
            if (z->right == 0)        // z->left must be null also
                leftmost = z->parent;
            // makes leftmost == header if z == root
            else
                leftmost = _rb_tree_node_base::minimum(x);
        if (rightmost == z)  
            if (z->left == 0)         // z->right must be null also
                rightmost = z->parent;  
            // makes rightmost == header if z == root
            else                      // x == z->left
                rightmost = _rb_tree_node_base::maximum(x);
    }
    if (y->color != _rb_tree_red) { 
        while (x != root && (x == nullptr || x->color == _rb_tree_black))
            if (x == x_parent->left) {
                _rb_tree_node_base* w = x_parent->right;
                if (w->color == _rb_tree_red) {
                    w->color = _rb_tree_black;
                    x_parent->color = _rb_tree_red;
                    _rb_tree_rotate_left(x_parent, root);
                    w = x_parent->right;
                }
                if ((w->left == nullptr || w->left->color == _rb_tree_black) &&
                    (w->right == nullptr || w->right->color == _rb_tree_black)) {
                    w->color = _rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->right == nullptr || w->right->color == _rb_tree_black) {
                        if (w->left) w->left->color = _rb_tree_black;
                            w->color = _rb_tree_red;
                        _rb_tree_rotate_right(w, root);
                        w = x_parent->right;
                    }
                    w->color = x_parent->color;
                    x_parent->color = _rb_tree_black;
                    if (w->right) w->right->color = _rb_tree_black;
                        _rb_tree_rotate_left(x_parent, root);
                    break;
                }
            } else {                  // same as above, with right <-> left.
                _rb_tree_node_base* w = x_parent->left;
                if (w->color == _rb_tree_red) {
                    w->color = _rb_tree_black;
                    x_parent->color = _rb_tree_red;
                    _rb_tree_rotate_right(x_parent, root);
                    w = x_parent->left;
                }
                if ((w->right == nullptr || w->right->color == _rb_tree_black) &&
                    (w->left == nullptr || w->left->color == _rb_tree_black)) {
                    w->color = _rb_tree_red;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w->left == nullptr || w->left->color == _rb_tree_black) {
                        if (w->right) w->right->color = _rb_tree_black;
                            w->color = _rb_tree_red;
                        _rb_tree_rotate_left(w, root);
                        w = x_parent->left;
                    }
                    w->color = x_parent->color;
                    x_parent->color = _rb_tree_black;
                    if (w->left) w->left->color = _rb_tree_black;
                        _rb_tree_rotate_right(x_parent, root);
                    break;
                }
            }
        if (x) x->color = _rb_tree_black;
    }
    return y;   
}                        


template <class Key, class Value, class KeyOfValue, class Compare,
          class Alloc = alloc>
class rb_tree {
private:    
    using void_pointer = void*;
    using base_ptr = _rb_tree_node_base*;  //节点基类
    using rb_tree_node = _rb_tree_node<Value>;
    using rb_tree_node_allocator = simple_alloc<rb_tree_node, Alloc>;
    using color_type = _rb_tree_color_type;
public:
    using key_type = Key;
    using value_type = Value;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type& ;
    using link_type = rb_tree_node* ;  //节点派生类
    using size_type = size_t ;
    using difference_type = ptrdiff_t ;

private:
    link_type get_node() { return rb_tree_node_allocator::allocate();}
    void put_node(link_type p) { rb_tree_node_allocator::deallocate(p);}

    link_type create_node(const value_type& x){
        link_type tmp = get_node();
        //TODO 异常处理
        construct(&tmp->value_field, x);
        return tmp;
    }

    link_type clone_node(link_type x) {
        link_type tmp = create_node(x->value_field);
        tmp->color = x->color;
        tmp->left = nullptr;
        tmp->right = nullptr;
        return tmp;
    }

    void destroy_node(link_type p){
        destroy(&p->value_field);
        put_node(p);
    }

private:
    size_type node_count;
    link_type header;   //min: header->left;  max: header->right; 
                        //parent: root    child: root
    Compare key_compare;

    link_type& root() const { return (link_type&) header->parent; }
    link_type& leftmost() const { return (link_type&) header->left; }
    link_type& rightmost() const { return (link_type&) header->right; }

    //提取节点信息的接口
    static link_type& left(link_type x) { return (link_type&)(x->left); }
    static link_type& right(link_type x) { return (link_type&)(x->right); }
    static link_type& parent(link_type x) { return (link_type&)(x->parent); }
    static reference value(link_type x) { return x->value_field; }
    static const Key& key(link_type x) { return KeyOfValue()(value(x));}  // KeyOfValue()是functor
    static color_type& color(link_type x) { return (color_type&)(x->color);}

    //【操作base和derived有什么意义上的区别 什么时候操作base？】
    static link_type& left(base_ptr x) { return (link_type&)(x->left); }
    static link_type& right(base_ptr x) { return (link_type&)(x->right); }
    static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
    static reference value(base_ptr x) { return ((link_type)x)->value_field; }
    static const Key& key(base_ptr x) { return KeyOfValue()(value(link_type(x)));} 
    static color_type& color(base_ptr x) { return (color_type&)(link_type(x)->color); }

    static link_type minimum(link_type x) { 
        return (link_type)  _rb_tree_node_base::minimum(x);
    }
    static link_type maximum(link_type x) {
        return (link_type) _rb_tree_node_base::maximum(x);
    }
public:
    using iterator = _rb_tree_iterator<value_type, reference, pointer> ;
    using const_iterator = _rb_tree_iterator<value_type, const_reference, const_pointer> ;

private:
    iterator _insert(base_ptr x, base_ptr y, const value_type& v);
    link_type _copy(link_type x, link_type p);
    void _erase(link_type x);
    void init() {
        header = get_node();
        color(header) = _rb_tree_red; 
        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
    }   
public:
    rb_tree(const Compare& comp = Compare())
        : node_count(0), key_compare(comp){ init();}
    rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) 
        : node_count(0), key_compare(x.key_compare)
    {
        //构造header：1.header本身 2.最大值最小值
        header = get_node();
        color(header) = _rb_tree_red;
        if(x.root() == nullptr){
            root() = nullptr;
            leftmost() = header;
            rightmost() = header;
        }
        else{
            //TODO 异常处理
            root() = _copy(x.root(), header);  //【_copy实现。】
            leftmost() = minimum(root());
            rightmost() = maximum(root());
        }
    }
    ~rb_tree() {
        clear();
        put_node(header);
    } 
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& 
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);           
   
    Compare key_comp() const { return key_compare; }
    iterator begin() { return leftmost(); }
    const_iterator begin() const { return leftmost(); }
    iterator end() { return header; }  //header是尾后节点  最大值节点自增即为header
    const_iterator end() const { return header; }
    bool empty() const { return node_count == 0; }
    size_type size() const { return node_count; }

    void swap(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& t) {
        std::swap(header, t.header);  //TODO std::swap
        std::swap(node_count, t.node_count);
        std::swap(key_compare, t.key_compare);
    }    

    std::pair<iterator,bool> insert_unique(const value_type& x); // TODO std::pair
    iterator insert_equal(const value_type& x);

    iterator insert_unique(iterator position, const value_type& x);
    iterator insert_equal(iterator position, const value_type& x);    

    template <class InputIterator>
    void insert_unique(InputIterator first, InputIterator last);
    template <class InputIterator>
    void insert_equal(InputIterator first, InputIterator last);

    void erase(iterator position);
    size_type erase(const key_type& x);
    void erase(iterator first, iterator last);
    void erase(const key_type* first, const key_type* last);
    void clear() {
        if (node_count != 0) {
            _erase(root());
            leftmost() = header;
            root() = nullptr;
            rightmost() = header;
            node_count = 0;
        }
    } 


    iterator find(const key_type& x);
    const_iterator find(const key_type& x) const;
    size_type count(const key_type& x) const;
    iterator lower_bound(const key_type& x);
    const_iterator lower_bound(const key_type& x) const;
    iterator upper_bound(const key_type& x);
    const_iterator upper_bound(const key_type& x) const;
    std::pair<iterator,iterator> equal_range(const key_type& x); //TODO std::pair
    std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const;  

};

//在指定位置插入一个节点，并调用rebalance调整树
//x_为插入点，y_为插入点父节点
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
_insert(base_ptr x_, base_ptr y_, const Value& v){  //【为啥要用基类指针类型的形参？】
    link_type x = (link_type) x_;
    link_type y = (link_type) y_;
    link_type z;    
    //【x到底是来干嘛的?】
    if (y == header || x != nullptr || key_compare(KeyOfValue()(v), key(y))) {
        z = create_node(v);
        left(y) = z;  //条件3

        if (y == header) {  //条件1
            root() = z;
            rightmost() = z;
        }
        else if (y == leftmost())
            leftmost() = z;           
    }
    else {
        z = create_node(v);
        right(y) = z;
        if (y == rightmost())
            rightmost() = z;          
    }
    parent(z) = y;
    left(z) = nullptr;
    right(z) = nullptr;
    _rb_tree_rebalance(z, header->parent);
    ++node_count;
    return iterator(z);
}


template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value& v){
    link_type y = header;
    link_type x = root();
    while (x != nullptr){
        y = x;
        x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
    }
    //找到了待插入节点的父节点y
    return _insert(x, y, v);
}

//返回值：pair<iterator,bool>
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& v){
    link_type y = header;
    link_type x = root();
    bool comp = true;
    while(x != nullptr){
        y = x;
        comp = key_compare(KeyOfValue()(v), key(x));
        x = comp ? left(x) : right(x);
    }
    iterator j = iterator(y);
    if(comp)
        if(j == begin())
            return std::pair<iterator,bool>(_insert(x, y, v), true);
        else   
            --j;
    // 默认less比较
    // 1.如果v真的比y大，comp为假，那么compare(y,v)就会为真
    // 否则是v与y相等，comp虽然依旧为假，但compare(y,v)也为假，不再插入。  
    // 2.如果v比y小，comp为真，--j与v也不相等，说明真的没重复过
    // 否则--j会正好与v相等，说明重复出现，不再插入。
    
    if (key_compare(key(j.node), KeyOfValue()(v)))
        return std::pair<iterator,bool>(_insert(x, y, v), true);
    return std::pair<iterator,bool>(j, false);    
}

template <class Key, class Val, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator 
rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_unique(iterator position,
                                                             const Val& v) {
    //插入在begin和end
    if (position.node == header->left) // begin()
        if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
            return _insert(position.node, position.node, v);
        //如果在叶节点插入，则调用_insert
        // first argument just needs to be non-null 
        else
            return insert_unique(v).first;
        //否则则无视形参position，其为无效位置。
    else if (position.node == header) // end()
        if (key_compare(key(rightmost()), KeyOfValue()(v)))
            return _insert(nullptr, rightmost(), v);
        else
            return insert_unique(v).first;
    //插入在普通位置
    else {
        iterator before = position;
        --before;
        if (key_compare(key(before.node), KeyOfValue()(v))
            && key_compare(KeyOfValue()(v), key(position.node)))
            if (right(before.node) == nullptr)
                return _insert(nullptr, before.node, v); 
            else
                return _insert(position.node, position.node, v);
            // first argument just needs to be non-null 
        else
            return insert_unique(v).first;
  }
}

template <class Key, class Val, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::iterator 
rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_equal(iterator position,
                                                            const Val& v) {
    if (position.node == header->left) // begin()
        if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
            return _insert(position.node, position.node, v);
    // first argument just needs to be non-null 
        else
            return insert_equal(v);
    else if (position.node == header) // end()
        if (!key_compare(KeyOfValue()(v), key(rightmost())))
            return _insert(nullptr, rightmost(), v);
        else
            return insert_equal(v);
    else {
        iterator before = position;
        --before;
        if (!key_compare(KeyOfValue()(v), key(before.node))
            && !key_compare(key(position.node), KeyOfValue()(v)))
            if (right(before.node) == nullptr)
                return _insert(nullptr, before.node, v); 
            else
                return _insert(position.node, position.node, v);
            // first argument just needs to be non-null 
        else
            return insert_equal(v);
    }
}

template <class K, class V, class KoV, class Cmp, class Al> template<class II>
void rb_tree<K, V, KoV, Cmp, Al>::insert_equal(II first, II last) {
    for ( ; first != last; ++first)
        insert_equal(*first);
}

template <class K, class V, class KoV, class Cmp, class Al> template<class II>
void rb_tree<K, V, KoV, Cmp, Al>::insert_unique(II first, II last) {
    for ( ; first != last; ++first)
        insert_unique(*first);
}

//删除单个节点，rebalance
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline void
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
    link_type y = (link_type) _rb_tree_rebalance_for_erase(position.node,
                                                            header->parent,
                                                            header->left,
                                                            header->right);
    destroy_node(y);
    --node_count;
}
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const Key& x) {
    auto p = equal_range(x);  //pair
    size_type n = 0; //【difference_type】
    n = distance(p.first, p.second);
    erase(p.first, p.second);
    return n;
}
//删除该节点对应的子树 clear()专用
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::_erase(link_type x) {
    while (x != nullptr){
        //递归
        _erase(right(x));
        link_type y = left(x);
        destroy_node(x);
        x = y;        
    }
}
//逐个删除节点，并rebalance
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator first, 
                                                            iterator last) {
    if (first == begin() && last == end())
        clear();
    else
        while (first != last) erase(first++);
}
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) {
    link_type y = header;        //不小于当前节点 ，或者是end()
    link_type x = root();        //当前节点

    while (x != nullptr) 
        if (!key_compare(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);

    iterator j = iterator(y);   
    return (j == end() || key_compare(k, key(j.node))) ? end() : j;
}
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::count(const Key& k) const {
    std::pair<const_iterator, const_iterator> p = equal_range(k); //TODO std::pair
    return distance(p.first, p.second);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key& k) {
    link_type y = header;  
    link_type x = root();  

    while (x != 0) 
        if (!key_compare(key(x), k))
        y = x, x = left(x);
        else
        x = right(x);

    return iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const Key& k) const {
    link_type y = header;  
    link_type x = root();  

    while (x != 0) 
        if (!key_compare(key(x), k))
        y = x, x = left(x);
        else
        x = right(x);

    return const_iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key& k) {
    link_type y = header; /* Last node which is greater than k. */
    link_type x = root(); /* Current node. */

    while (x != 0) 
        if (key_compare(k, key(x)))
        y = x, x = left(x);
        else
        x = right(x);

    return iterator(y);
}

template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator 
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const Key& k) const {
    link_type y = header;  
    link_type x = root(); 

    while (x != 0) 
        if (key_compare(k, key(x)))
        y = x, x = left(x);
        else
        x = right(x);

    return const_iterator(y);
}

//TODO std::pair
//返回【it1,it2)  其中 *it1 >= k, *it2 > k;
template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
inline std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,
            typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator>
rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const Key& k) {
    return std::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
}

template <class Key, class Value, class KoV, class Compare, class Alloc>
inline std::pair<typename rb_tree<Key, Value, KoV, Compare, Alloc>::const_iterator,
            typename rb_tree<Key, Value, KoV, Compare, Alloc>::const_iterator>
rb_tree<Key, Value, KoV, Compare, Alloc>::equal_range(const Key& k) const {
    return std::pair<const_iterator,const_iterator>(lower_bound(k), upper_bound(k));
}


}
#endif