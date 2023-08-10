#ifndef ALLOC_H
#define ALLOC_H

#include <cstddef>   //size_t   
#include <cstdlib>   //malloc  free
#include <new>  //bad_alloc

namespace miniSTL{

class _malloc_alloc{
public:
    using malloc_handler = void (*)();

private:
    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t);

    static malloc_handler _malloc_alloc_oom_handler;

public:
    static void * allocate(size_t n){
        void *result = malloc(n);   //【malloc是如何实现的？】
        if(0 == result) result = oom_malloc(n);
        return result;
    }

    static void deallocate(void *p, size_t ){
        free(p);
    }

    static void * reallocate(void *p, size_t , size_t new_sz){
        void * result = realloc(p, new_sz); //【直接调用realloc】
        if(0 == result) oom_realloc(p, new_sz);
        return result;
    }

    static malloc_handler set_malloc_hander(malloc_handler f){
        malloc_handler old = _malloc_alloc_oom_handler;
        _malloc_alloc_oom_handler = f;
        return old;
    }

};
//TODO 对oom进行测试 配置oom handler
void (* _malloc_alloc::_malloc_alloc_oom_handler)() = 0;  //【有问题不】

inline void * _malloc_alloc::oom_malloc(size_t n){
    malloc_handler my_alloc_handler;
    void *result;

    for(;;){
        my_alloc_handler = _malloc_alloc_oom_handler;
        // if(!my_alloc_handler)   throw std::bad_alloc();     //TODO   这里要处理 会导致报错
        // (*my_alloc_handler)();
        result = malloc(n);
        if(result) return result;
    }
}

inline void * _malloc_alloc::oom_realloc(void *p, size_t n){
    malloc_handler my_alloc_handler;
    void *result;

    for(;;){
        my_alloc_handler = _malloc_alloc_oom_handler;
        if(!my_alloc_handler) throw std::bad_alloc();
        (*my_alloc_handler)();
        result = realloc(p, n);
        if(result) return result;
    }
}

using malloc_alloc = _malloc_alloc;

/*第二级配置器*/


class _default_alloc{
private:
    enum _freelist_setting {   //【effective c++ 哪条内容？】
        _ALIGN = 8,
        _MAX_BYTES = 128,
        _NFREELISTS = _MAX_BYTES / _ALIGN
    };

    static size_t ROUND_UP(size_t bytes){
        return (((bytes) + static_cast<size_t>(_ALIGN) - 1 ) & ~(static_cast<size_t>(_ALIGN) - 1));
    }

    union obj{
        union obj *free_list_link;   //next
        char client_data[1];
    };

    //隐式空闲链表的 空闲节点 数组，其元素为某一定大小的空闲链表的空闲节点
    static obj * volatile free_list[_NFREELISTS];
    //计算将使用的free-lists编号
    static size_t FREELIST_INDEX(size_t bytes){
        return (((bytes) + _ALIGN - 1) / _ALIGN - 1); //要用static_cast转size_t么
    }

    static void *refill(size_t n);

    static char *chunk_alloc(size_t size, int &nobjs);
    
    static char *start_free;
    static char *end_free;
    static size_t heap_size;

public:
    static void * allocate(size_t n);
    static void deallocate(void *p, size_t n);
    static void * reallocate(void *p, size_t old_sz, size_t new_sz);
    
};

char *_default_alloc::start_free = 0;
char *_default_alloc::end_free = 0;
size_t _default_alloc::heap_size = 0;
       
_default_alloc::obj * volatile _default_alloc::free_list[_default_alloc::_NFREELISTS] = 
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

inline void *_default_alloc::allocate(size_t n ){
    obj * volatile * my_free_list;
    obj * result;
    //调用一级配置器
    if(n > (size_t) _MAX_BYTES){
        return (malloc_alloc::allocate(n));
    }
    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if(result == 0){
        void *r = refill(ROUND_UP(n)); //【同时在refill函数里维护空闲链表】
        return r;   
    }
    *my_free_list = result -> free_list_link;
    return (result);
}

inline void _default_alloc::deallocate(void *p, size_t n){
    if(n > (size_t) _MAX_BYTES){
        malloc_alloc::deallocate(p, n);
        return;
    }
    obj *q = (obj *) p;//【obj *q = reinterpret_cast<obj *>(p);】
    obj * volatile * my_free_list;

    my_free_list = free_list + FREELIST_INDEX(n);
    q -> free_list_link = *my_free_list; // q -> next = previous free
    *my_free_list = q;   //  q is now free

}

void* _default_alloc::refill(size_t n){
    int nobjs = 20;
    char * chunk = chunk_alloc(n, nobjs);
    obj * volatile * my_free_list;
    obj * result;
    obj * current_obj, * next_obj;
    int i;

    if(1 == nobjs) return (chunk);  //【不维护链表吗？直接返回】
    my_free_list = free_list + FREELIST_INDEX(n);

    //chunk所指的内存分给用户
    result = (obj *)chunk; 
    //chunk所指之后的内存，交给空闲链表维护
    //空闲链表节点指向第一个空闲块
    *my_free_list = next_obj = (obj *)(chunk + n);//【chunk + n 是下一个块】
    //把这些节点维护成隐式链表
    for(i = 1; ; i++){
        current_obj = next_obj;
        next_obj = (obj *) ((char*)next_obj + n);
        if(nobjs - 1 == i){
            current_obj -> free_list_link = 0;
            break;
        }else{
            current_obj -> free_list_link = next_obj;
        }
    }
    return (result);
}

char* _default_alloc::chunk_alloc(size_t size, int &nobjs){
    char * result;
    size_t total_req_bytes = size * nobjs;  
    size_t bytes_left = end_free - start_free;

    if(bytes_left >= total_req_bytes){
        result = start_free;
        start_free += total_req_bytes;
        return(result);
    }else if(bytes_left >= size){  //无法提供足够的默认的20个块,但能提供一个块以上的内存
        nobjs = bytes_left/size;  //修改nobjs,指示所分配的块数量
        total_req_bytes = size * nobjs;
        result = start_free;
        start_free += total_req_bytes;
        return(result);
    }else{
        size_t bytes_to_get = 2 * total_req_bytes + ROUND_UP(heap_size >> 4);
        //如果还剩一点儿，分配给较小的块
        if (bytes_left > 0) {
            obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);

            ((obj *)start_free) -> free_list_link = *my_free_list;
            *my_free_list = (obj *)start_free;
        }

        start_free = (char *)malloc(bytes_to_get); //【malloc】
        if(0 == start_free){ //从heap分配失败
            int i ;
            obj * volatile * my_free_list, *p;
            //从空闲链表里收集[一个块]的内存（之前分配太多了还没用掉，且比当前size要大的）
            //【为什么不搜寻更小的呢？ 所说的多进程会带来什么影响？】
            for(i = size; i <= _MAX_BYTES; i += _ALIGN){//【用大块来补小块 会带来碎片吧】
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if(0 != p){
                    *my_free_list = p -> free_list_link; //取出一个，p，更新空闲节点
                    start_free = (char *)p;
                    end_free = start_free + i; //【这里会产生碎片吗？ i并没有对齐】
                    return(chunk_alloc(size, nobjs)); // to else if
                }
            }
            //空闲链表也没剩了 oom
            end_free = 0; //【c++11 nullptr】
            //第一级配置器 
            start_free = (char *)malloc_alloc::allocate(bytes_to_get);

        }
        //第一级配置器取出了一些内存
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        //递归调用 进入前两个if分支 正确重置nobjs
        return (chunk_alloc(size, nobjs));
    }
}









}



#endif