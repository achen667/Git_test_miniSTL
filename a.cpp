
// #include <stdio.h>
// #include <memory>
// #include<algorithm>



#include "uninitialized.h"
#include "iterator.h"
#include "alloc.h"
#include "vector.h"
#include <vector>

 //using namespace miniSTL;
class CLASSA{
public:
    CLASSA(int n ){i = n;j = n;k = n;};

private:
    int i ;
    int j ; 
    int k ;
};

int *p ;
int& change_value(){
   
    return *p;
}

int main(){
    int abcdef;
    
    miniSTL::_true_type a;
    bool value = true;
    miniSTL::vector<int> v(2,value);
    miniSTL::vector<CLASSA> v1(3,10);

    miniSTL::vector<CLASSA> v2(v1);

    int aa =1;
    p = &aa;
    int *p2 = &change_value();
    int &r2 = change_value();
    r2 = 5;
    int b = change_value();
    return 1;
}