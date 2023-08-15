
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
 //using namespace miniSTL;

class A
{
private:
    /* data */
    int data ;
public:
    A(int v):data(v){}
    ~A();
};

class B{
    A* node;
    B(A* x):node(x){}
};

class largeClass{
private:
    int a = 1;
    int b = 1;
    int c = 1;
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
   
    
    miniSTL::list<int> ls(5,2) , ls2(5,1);
    auto it = ls.begin(),it2 = ls2.begin();
    bool equal = it == it2;
    bool equal2 = ls == ls2;
    
    ls.splice(++(ls.begin()),ls2);
    
   
    return 1;
}