
// #include <stdio.h>
// #include <memory>
// #include<algorithm>



#include "uninitialized.h"
#include "iterator.h"
#include "alloc.h"
#include "vector.h"
#include <vector>
#include<memory>
#include <iostream>
#include <list>
#include "list.h"
#include "deque.h"
#include <deque>
#include "tree.h"
// #include "set.h"
#include <unordered_map>
#include <hashtable.h>
 //using namespace miniSTL;



class largeClass{
private:
    int a = 1;
    int b = 1;
    int c = 1;
};

class Base{
public:    int c;
};
class Derived : public Base{
public:
    int a = 12;
    void print(){
        std::cout << a << std::endl;
    }
};

int get_value_const(const int& n){
    return n;
}
void my_malloc_alloc_oom_handler() {
    std::cerr << "Out of memory! Exiting..." << std::endl;
    std::exit(1); // 退出程序，可以根据需要采取其他措施
}
class ExceptionThrowingClass {
public:
    ExceptionThrowingClass() {
        std::cout << "Constructing ExceptionThrowingClass..." << std::endl;
        throw std::runtime_error("ExceptionThrowingClass constructor threw an exception.");
    }
};

int main(){
    /*
    int abcdef;
    

    miniSTL::_true_type a;
    bool value = true;
    
    std::vector<int> std_v(3,1);
    std::vector<int>::iterator std_it =std_v.begin(); 

    miniSTL::vector<int> v(2,value);
    miniSTL::vector<int> v1(3,1);
    miniSTL::vector<int>::iterator it_v1 = v1.begin();
    
    largeClass lar_C;
    miniSTL::vector<largeClass> v_large(3,lar_C);

    std::list<int> ls;
    ls.push_back(1);ls.push_back(2);
    std::list<int>::iterator i_ls = ls.begin();
    std::list<int>::iterator i_ls2 = ls.end();
    std::cout << *i_ls << std::endl;
    std::cout << *i_ls2 << std::endl;
        
     miniSTL::list<int> my_ls(1,2);  

    bool wrong = my_ls.begin() == my_ls.end();
    bool wrong2 = ls.begin() == ls.end();
    */
    
    /*
    std:: list<int> ls;
    ls.push_back(1);
    ls.push_back(2);
    ls.push_back(3);
    std::list<int>::iterator it = ls.begin();  //1
    while(1){
        std::cout << *it << std::endl;
        std::cout << &(*it) << std::endl;
        it++;

    }
    */
    
    // miniSTL::list<int> ls(5,2);
    // miniSTL::list<int>::iterator it = ls.begin();
    // miniSTL::list<int>::iterator it2 = ls.begin();
    // it2++;  it2++;
    // miniSTL::list<int> ls2;
    // ls2.insert(ls2.begin(),it,it2);
   
    
    // largeClass classA;
    // miniSTL::deque<largeClass> que(41)  ;
    // que.push_back(classA);
    // que.push_back(classA);
    
    // miniSTL::deque<largeClass>::iterator it = que.end();
    // que.insert(it,32,classA);
    // it --;
    // //que.pop_back();
    // largeClass classB(que[50]);
   
    
    // std::deque<int> sque = {1,2,3,4,5,6};
    // auto it = sque.begin();
    // it ++;
    // it ++;
    // it[2] = 99;
    
    // using namespace miniSTL;
    // _rb_tree_node<int> tree_node;
    // tree_node.value_field = 123;
    // _rb_tree_iterator<int, int&,int*>  it(&tree_node);
    // int res = *it;
    // Derived* pd = new Derived;
    // Base* pb = pd;
    // using derived_pointer = Derived*;
    // int b = derived_pointer(pb)->a;
    
    // miniSTL::rb_tree<int,int, miniSTL::identity<int>,std::less<int> > tree;
    // tree.insert_equal(3);
    // tree.insert_equal(4);
    // tree.insert_equal(0);
    // tree.insert_unique(5);
    // auto it1 = tree.end();

    // miniSTL::set<int> set1;
    // set1.insert(1);
    // set1.insert(2);
    // set1.insert(0);
    
    // auto it = set1.begin();
    // auto it2 = set1.end();
    // miniSTL::set<int> set2;
    // set2.insert(it,it2);
    // set1.insert(it,0);

	// miniSTL::vector<int> iv(100,1);
    // iv.push_back(100);
    // iv.push_back(200);
    // iv.push_back(300);
    // iv.push_back(400);
	// auto it = iv.begin();
	// for (; it != iv.end(); it++) {
	// 	if (*it == 300) {
	// 		iv.erase(it);
	// 	}
	// }
    //std::unordered_map

    // //std::set_new_handler(my_malloc_alloc_oom_handler);
    // //miniSTL::malloc_alloc::set_malloc_hander(my_malloc_alloc_oom_handler);
    // try {
    //     // 尝试分配内存，当内存不足时将触发 my_malloc_alloc_oom_handler
    //     miniSTL::vector<int>* vp = new miniSTL::vector<int>(1000000000000,1);
    //     //int* arr = new int[1000000000000]; 
    //     // 使用分配的内存
        
    // } catch (const std::bad_alloc& e) {
    //     std::cerr << "Caught exception: " << e.what() << std::endl;
    // }

    miniSTL::vector<largeClass> vlarge(2);

    miniSTL::vector<int> v1(3,2);
    auto v2 = std::move(v1);

    std::vector<int> numbers;

    // 使用 assign 替换元素
    std::vector<int> newNumbers = {1, 2, 3};
    numbers.assign(newNumbers.begin(), newNumbers.end());

    int *pi = new int[10];
    delete pi;
    
    return 1;
}
