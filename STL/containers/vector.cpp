// C++ library

// Input/Output
#include <iostream>
// Diagnostics
#include <exception>
// General Utilities
#include <type_traits>
// Concurrency
#include <thread>
#include <future>

// Usage:
// g++ vector.cpp -o test -lpthread

//--- Implementation details
namespace
lib_impl
{
	class
	BadAlloc
		: public std::bad_alloc
	{
		public:
			const char* 
			what() const 
			noexcept
			override
			{
				return "Memory Bad Allocation";
			}
	};
}
//--- NS lib_impl

// Global scope
// The program is ill-formed if a replacement is defined in namespace other than global namespace,
// or if it is defined as a static non-member function at global scope. 

// no inline, required by [replacement.functions]
void* 
operator 
new(std::size_t sz)
{
	std::printf("1) new(size_t), size = %zu\n", sz);
  // avoid std::malloc(0) which may return nullptr on success
  if (sz == 0)
      ++sz; 

  if (void *ptr = std::malloc(sz))
      return ptr;
	// required by [new.delete.single]
  throw lib_impl::BadAlloc{}; 
}
 
// no inline, required by [replacement.functions]
void* 
operator 
new[](std::size_t sz)
{
	std::printf("2) new[](size_t), size = %zu\n", sz);
  if (sz == 0)
      ++sz; 

  if (void *ptr = std::malloc(sz))
      return ptr;

  throw lib_impl::BadAlloc{}; 
}
 
void 
operator 
delete(void* ptr) 
noexcept
{
    std::puts("3) delete(void*)");
    std::free(ptr);
}
 
void 
operator 
delete(void* ptr, 
			std::size_t size) 
noexcept
{
    std::printf("4) delete(void*, size_t), size = %zu\n", size);
    std::free(ptr);
}
 
void 
operator 
delete[](void* ptr) 
noexcept
{
    std::puts("5) delete[](void* ptr)");
    std::free(ptr);
}
 
void 
operator 
delete[](void* ptr, 
				std::size_t size) 
noexcept
{
    std::printf("6) delete[](void*, size_t), size = %zu\n", size);
    std::free(ptr);
}
//---End Global Namespace
//---Type checking
namespace
type
{
	#define _IsUnused __attribute__ ((__unused__))
	
	#define _CLASS_REQUIRES(_type, _concept) \
		typedef void (_concept <_type>::* _func##_type##_concept)(); \
		template <_func##_type##_concept _T1> \
		struct _concept_checking##_type##_concept { }; \
		typedef _concept_checking##_type##_concept< \
			&_concept <_type>::__constraints> \
			_concept_checking_typedef##_type##_concept
				
	template <class _Concept>
	inline 
	void 
	__function_requires()
	{
		void (_Concept::*__x)() _IsUnused = &_Concept::__constraints;
	}	
	
	// Basic Concepts
	template <class T>
	struct 
	DefaultConstructibleConcept
	{
		void 
		__constraints() 
		{
			// requires default constructor
			T __a _IsUnused;                
		}
	};				
	template <class T>
	struct 
	CopyConstructibleConcept
	{
		void 
		__constraints() 
		{
			// requires copy constructor
			 T __a(__b);  
			// requires address of operator                   
			 T* __ptr _IsUnused = &__a;      
			 __const_constraints(__a);
		}
		void 
		__const_constraints(const T& __a) 
		{
			// requires const copy constructor
			 T __c _IsUnused(__a);
			// requires const address of operator           
			 const T* __ptr _IsUnused = &__a; 
		}
		T __b;
	};
	template <class T>
	struct 
	AssignableConcept
	{
		void 
		__constraints() 
		{
			// requires assignment operator
			__a = __a;                        
			__const_constraints(__a);
		}
		void 
		__const_constraints(const T& __b) 
		{
			// const required for argument to assignment
			__a = __b;                   
		}
		T __a;
	};
	
	// http://www.stlport.org/doc/sgi_stl.html
  // The SGI STL version of Assignable requires copy constructor and operator=
	template <class T>
	struct 
	SGIAssignableConcept
	{
		void 
		__constraints() 
		{
			T __b _IsUnused(__a);
			// requires assignment operator
			__a = __a;                        
			__const_constraints(__a);
		}
		void 
		__const_constraints(const T& __b) 
		{
			T __c _IsUnused(__b);
			// const required for argument to assignment
			__a = __b;              
		}
		T __a;
	};
	template <class _From, 
							class _To>
	struct 
	ConvertibleConcept
	{
		void __constraints() 
		{
			_To __y _IsUnused = __x;
		}
		_From __x;
	};
	template <class T>
	void 
	__aux_require_boolean_expr(const T& __t) 
	{
		bool __x _IsUnused = __t;
	}

	template <class T>
	struct EqualityComparableConcept
	{
		void 
		__constraints() 
		{
			__aux_require_boolean_expr(__a == __b);
		}
		T __a;
		T __b;
	};			
	// Iterator Concepts
	template <class T>
	struct 
	TrivialIteratorConcept
	{
		void __constraints() 
		{
			__function_requires< DefaultConstructibleConcept<T> >();
			__function_requires< AssignableConcept<T> >();
			__function_requires< EqualityComparableConcept<T> >();
			// require dereference operator
			(void)*__i;                       
		}
		T __i;
	};	
	template <class T>
	struct 
	InputIteratorConcept
	{
		void __constraints() 
		{
			__function_requires< TrivialIteratorConcept<T> >();
			// require iterator_traits typedef's
			typedef typename std::iterator_traits<T>::difference_type _Diff;
			typedef typename std::iterator_traits<T>::reference _Ref;
			typedef typename std::iterator_traits<T>::pointer _Pt;
			typedef typename std::iterator_traits<T>::iterator_category _Cat;
			__function_requires< ConvertibleConcept<
																							typename std::iterator_traits<T>::iterator_category,
																							std::input_iterator_tag> >();
			// requires pre-increment operator
			++__i; 
			// requires post-increment operator                           
			__i++;                            
		}
		T __i;
	};
	template <class T>
	struct 
	ForwardIteratorConcept
	{
		void 
		__constraints() 
		{
			__function_requires< InputIteratorConcept<T> >();
			__function_requires< DefaultConstructibleConcept<T> >();
			__function_requires< ConvertibleConcept<
																							typename std::iterator_traits<T>::iterator_category,
																							std::forward_iterator_tag> >();
			typedef typename std::iterator_traits<T>::reference _Ref;
			_Ref __r _IsUnused = *__i;
		}
		T __i;
	};	
	// Container Concepts
	template <class _Container>
	struct 
	ContainerConcept
	{
		typedef typename _Container::value_type _Value_type;
		typedef typename _Container::difference_type _Difference_type;
		typedef typename _Container::size_type _Size_type;
		typedef typename _Container::const_reference _Const_reference;
		typedef typename _Container::const_pointer _Const_pointer;
		typedef typename _Container::const_iterator _Const_iterator;

		void 
		__constraints() 
		{
			__function_requires< InputIteratorConcept<_Const_iterator> >();
			__function_requires< AssignableConcept<_Container> >();
			const _Container __c;
			__i = __c.begin();
			__i = __c.end();
			__n = __c.size();
			__n = __c.max_size();
			__b = __c.empty();
		}
		bool __b;
		_Const_iterator __i;
		_Size_type __n;
	};		 
   
  // integral_constant
  template<typename _Tp, _Tp __v>
  class 
  integral_constant
  {
		public:
		  typedef _Tp value_type;
		  typedef integral_constant<_Tp, __v> type;			
		  constexpr 
		  operator value_type() const 
		  noexcept 
		  { 
		  	return value; 
		  }
#if __cplusplus > 201103L

//#define __cpp_lib_integral_constant_callable 201304L

		  constexpr 
		  value_type 
		  operator()() const 
		  noexcept 
		  { 
		  	return value; 
		  }
#endif
		private:
		  static constexpr _Tp value{__v};
	
  };

#if ! __cpp_inline_variables
  template<typename _Tp, _Tp __v>
    constexpr _Tp integral_constant<_Tp, __v>::value;
#endif

  // The type used as a compile-time boolean with true value.
  using true_type =  integral_constant<bool, true>;
  // The type used as a compile-time boolean with false value.
  using false_type = integral_constant<bool, false>;
  
	template<typename...> using __void_t = void;
          
  // bool_constant for C++11
  template<bool __v>
    using __bool_constant = integral_constant<bool, __v>;
	// Type relation
  /// is_same
  template<typename _Tp, typename _Up>
    struct is_same
    : public false_type
    {
    };
  template<typename _Tp>
    struct is_same<_Tp, _Tp>
    : public true_type
    { 
    }; 
}
//---NS type