#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H
/*【缺少const的类型提取】*/

namespace miniSTL{

class _false_type{
};
class _true_type{
};



template <class type>
struct _type_traits { 
   typedef _true_type     this_dummy_member_must_be_first;
                   /* Do not remove this member. It informs a compiler which
                      automatically specializes _type_traits that this
                      _type_traits template is special. It just makes sure that
                      things work if an implementation is using a template
                      called _type_traits for something unrelated. */

   /* The following restrictions should be observed for the sake of
      compilers which automatically produce type specific specializations 
      of this class:
          - You may reorder the members below if you wish
          - You may remove any of the members below if you wish
          - You must not rename members without making the corresponding
            name change in the compiler
          - Members you add will be treated like regular members unless
            you add the appropriate support in the compiler. */
 

   typedef _false_type    has_trivial_default_constructor;
   typedef _false_type    has_trivial_copy_constructor;
   typedef _false_type    has_trivial_assignment_operator;
   typedef _false_type    has_trivial_destructor;
   typedef _false_type    is_POD_type;
};



// Provide some specializations.  This is harmless for compilers that
//  have built-in _types_traits support, and essential for compilers
//  that don't.

template <> struct _type_traits<char> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<signed char> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<unsigned char> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<short> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<unsigned short> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<int> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<unsigned int> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<long> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<unsigned long> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<float> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<double> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};

template<> struct _type_traits<long double> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};


//【原生指针  】
template <class T>
struct _type_traits<T*> {
   typedef _true_type    has_trivial_default_constructor;
   typedef _true_type    has_trivial_copy_constructor;
   typedef _true_type    has_trivial_assignment_operator;
   typedef _true_type    has_trivial_destructor;
   typedef _true_type    is_POD_type;
};
}


#endif