
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
    CLASSA(int n ):i(n){};

private:
    int i ;
};

int main(){
    int abcdef;
    
    miniSTL::_true_type a;
    bool value = true;
    miniSTL::vector<int> v(2,value);
    miniSTL::vector<CLASSA> v1(3,10);
    bool res = v[1];

    std::vector<bool> stdv(2,value);


    return 1;
}