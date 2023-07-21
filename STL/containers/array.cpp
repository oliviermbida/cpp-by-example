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
// g++ array.cpp -o test -lpthread

// MISRA C++ 
// Rule 7–3–1
// The global namespace shall only contain 
// main, 
// namespace declarations 
// and extern "C" declarations.
// Rule 16–0–2
// Macros shall only be #define’d or #undef’d in the global namespace.

#define _IsUnused  __attribute__ ((__unused__))

// Type checking
namespace
type
{			
	// Iterator
	struct input_iterator_tag { };
	struct output_iterator_tag { };
	struct forward_iterator_tag : public input_iterator_tag { };
	struct bidirectional_iterator_tag : public forward_iterator_tag { };
	struct random_access_iterator_tag : public bidirectional_iterator_tag { };
		
	template <typename Iterator>
	struct 
	Iterator_traits	
	{
		typedef typename Iterator::iterator_category iterator_category;
		typedef typename Iterator::value_type        value_type;
		typedef typename Iterator::difference_type   difference_type;
		typedef typename Iterator::pointer           pointer;
		typedef typename Iterator::reference         reference;
	};
	template <typename T>
	struct 
	Iterator_traits<T*>	
	{
		typedef random_access_iterator_tag					 iterator_category;
		typedef T															       value_type;
		typedef std::ptrdiff_t										   difference_type;
		typedef T*												           pointer;
		typedef T&													         reference;
	};
	template <typename T>
	struct 
	Iterator_traits<const T*>	
	{
		typedef random_access_iterator_tag					 iterator_category;
		typedef T															       value_type;
		typedef std::ptrdiff_t										   difference_type;
		typedef const T*									           pointer;
		typedef const T&										         reference;
	};				
  //---
	template <class _Concept>
	inline 
	void 
	__function_requires()
	{
		void (_Concept::*__x)() _IsUnused = &_Concept::__constraints;
	}	
	struct 
	DefaultConcept
	{
    // MISRA C++ 
    // Compliant Rule 2–10–2
    // Identifiers declared in an inner scope shall not hide 
    // an identifier declared in an outer scope.

    // Every type will implement its own function within the scope of the type definition
    static
		void 
		__constraints() 
		{
		}
	};  
  typedef decltype(DefaultConcept::__constraints) _func_type_concept;
  template <_func_type_concept _Concept> 
  struct 
  _class_requires 
  { 
  }; 	
	
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
  // This is equivalent to SGI STL's LessThanComparable.
	template <class T>
	struct 
	ComparableConcept
	{
		void 
		__constraints() 
		{
			__aux_require_boolean_expr(__a < __b);
			__aux_require_boolean_expr(__a > __b);
			__aux_require_boolean_expr(__a <= __b);
			__aux_require_boolean_expr(__a >= __b);
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
																							typename type::Iterator_traits<T>::iterator_category,
																							type::input_iterator_tag> >();
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
																							typename type::Iterator_traits<T>::iterator_category,
																							type::forward_iterator_tag> >();
			typedef typename std::iterator_traits<T>::reference _Ref;
			_Ref __r _IsUnused = *__i;
		}
		T __i;
	};	
 	template <class T>
	struct 
	BidirectionalIteratorConcept
	{
		void 
		__constraints() 
		{
				__function_requires< ForwardIteratorConcept<T> >();
				__function_requires< ConvertibleConcept<
																					      typename type::Iterator_traits<T>::iterator_category,
																					      type::bidirectional_iterator_tag> >();
				// requires pre-decrement operator
				--__i; 
				// requires post-decrement operator                           
				__i--;                            
			}
			T __i;
	}; 
	template <class T>
	struct 
	RandomAccessIteratorConcept
	{
		void __constraints() 
		{
			__function_requires< BidirectionalIteratorConcept<T> >();
			__function_requires< ComparableConcept<T> >();
			__function_requires< ConvertibleConcept<
																			        typename type::Iterator_traits<T>::iterator_category,
																			        type::random_access_iterator_tag> >();
			typedef typename type::Iterator_traits<T>::reference _Ref;

			// requires assignment addition operator
			__i += __n;
			// requires addition with difference type                       
			__i = __i + __n; __i = __n + __i; 
			// requires assignment subtraction op
			__i -= __n;
			// requires subtraction with difference type                    
			__i = __i - __n;                  
			// requires difference operator			                         
			__n = __i - __j; 
			// requires element access operator                 
			static_cast<void>(__i[__n]);                   
		}
		T __a;
		T __b;
		T __i;
		T __j;
		typename type::Iterator_traits<T>::difference_type __n;
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
//---Implementation details
namespace
lib_impl
{
	template <class C>
	constexpr 
	auto 
	begin(C& c) -> decltype(c.begin()) 
	{
		return c.begin();
	}
	template <class C>
	constexpr 
	auto 
	end(C& c) -> decltype(c.end()) 
	{
		return c.end();	
	}	
	template <class T, 
							std::size_t n>
	constexpr
	T*
	begin(T(&arr)[n])
	{
		return arr;
	}
	template <class T, 
							std::size_t n>
	constexpr
	T*
	end(T(&arr)[n])
	{
		return (arr+n);
	}	
	template<class InputIt, 
					class NoThrowForwardIt>
	NoThrowForwardIt 
	uninitialized_copy(InputIt first, 
										InputIt last, 
										NoThrowForwardIt d_first)
	noexcept(false)
	{
	  using T = typename type::Iterator_traits<NoThrowForwardIt>::value_type;
	  NoThrowForwardIt current = d_first;
	  try 
	  {
      for (; first != last; ++first, (void) ++current) 
      {
      	// non-allocating placement [new] allocation function
      	// pointer to a memory area to initialize the object at 
        ::new( static_cast<void*>(std::addressof(*current)) ) T(*first);
      }
      return current;
	  } 
	  catch (...) 
	  {
      for (; d_first != current; ++d_first) 
      {
        d_first->~T();
      }
      throw;
	  }
	}
	template<typename OutputIt, 
						typename Size, 
						typename T>		
	OutputIt 
	fill_n(OutputIt result, 
					Size count, 
					const T& val)
	{
		for (Size i = 0; i < count; i++)
			*result++ = val;
		return result;
	}
	// Iterator swap
	template <bool BType>
	struct
	iter_s
	{
		template<typename Iter1, 
							typename Iter2>
		static
		void
		swap(Iter1 a, 
								Iter2 b)
		{
			typedef typename type::Iterator_traits<Iter1>::value_type
			ValueType1;
			ValueType1 tmp = std::move(*a);
			*a = std::move(*b);
			*b = std::move(tmp);
		}
	};
	template <>
	struct 
	iter_s<true>
	{
		template<typename Iter1, 
							typename Iter2>
		static
		void
		swap(Iter1 a, 
								Iter2 b)
		{
			std::swap(*a, *b);
		}	
	};
	template <typename Iter1, 
							typename Iter2>
	inline 
	void
	iter_swap(Iter1 a, Iter2 b)
	{
		typedef typename type::Iterator_traits<Iter1>::value_type
		ValueType1;
		typedef typename type::Iterator_traits<Iter2>::value_type
		ValueType2;

		typedef typename type::Iterator_traits<Iter1>::reference
		ReferenceType1;
		typedef typename type::Iterator_traits<Iter2>::reference
		ReferenceType2;
		
		iter_s <type::is_same<ValueType1, ValueType2>::value
							&& type::is_same<ValueType1&, ReferenceType1>::value
							&& type::is_same<ValueType2&, ReferenceType2>::value>::
		swap(a, b);
	}
	template <typename Iter1,
							typename Iter2>
	Iter2
	swap_ranges(Iter1 first1, 
								Iter1 last1,
	 							Iter2 first2)
	{ 
		for (; first1 != last1; ++first1, ++first2)
		{
			lib_impl::iter_swap(first1, first2);
		}
		return first2;
	}			
}
//---NS lib_impl
//-- User library
namespace
lib
{
	// type checking
	
	template<class T,
					std::size_t n>
	class
	Array
	{
      // MISRA C++
      // Rule 16–0–4
      // Function-like macros shall not be defined.
      // used inline functions instead for safer type checking of parameters.

			// Concept requirements 
			typedef T* iter;
      typedef Array<T,n> Array_t; 
        // Member functions of a local class are implicitly inline functions.
      static
      void 
      M_constraints() 
      {
        // Requirements for element type T used by Array
        type::__function_requires< type::SGIAssignableConcept<T> >();
        type::__function_requires< type::RandomAccessIteratorConcept<iter> >();
        // Requirements for the container Array
        type::__function_requires< type::ContainerConcept<Array_t> >();   
      }
      typedef type::_class_requires< &M_constraints > _concept_requirements;

		public:
    	using size_type                              = std::size_t;
    	using value_type                             = T;	
    	using difference_type                        = std::ptrdiff_t;
    	using reference															 = T&;
    	using const_reference												 = const T&;    	
    	using pointer																 = T*;
    	using const_pointer													 = const T*;
    	using iterator 															 = T*;
    	using const_iterator												 = const T*;	
    	using reverse_iterator											 = std::reverse_iterator<iterator>;
    	using const_reverse_iterator								 = std::reverse_iterator<const_iterator>;

			Array()
      {
      }
      // AUTOSAR C++
      // Compliant Rule A12-0-1
      // “the rule of zero”
      // five special member functions
      // - copy and move constructor
      // - copy and move assignment
      // - destructor

      Array(std::initializer_list<T> lst)
				// get memory for size elements	
			{		
				// copy initializer_list elements
				M_copy(lst);
			}    	
    	// Iterators.
    	iterator
    	begin()
    	{
    		return iterator(std::addressof(M_instance[0])); 
    	}
    	iterator
    	end()
    	{
    		return iterator(std::addressof(M_instance[n])); 
    	}
    	const_iterator
    	begin() const
    	{
    		return const_iterator(std::addressof(M_instance[0])); 
    	}
    	const_iterator
    	end() const
    	{
    		return const_iterator(std::addressof(M_instance[n])); 
    	}    	 
    	reverse_iterator
    	rbegin()
    	{
    		return reverse_iterator(end()); 
    	} 
    	reverse_iterator
    	rend()
    	{
    		return reverse_iterator(begin()); 
    	} 
    	// Capacity.
    	constexpr 
    	size_type
    	size() const
    	{
    		return n;
    	}
    	constexpr
    	size_type
    	max_size() const 
    	{ 
    		return n; 
    	}
    	constexpr 
    	bool
    	empty()	const
    	{
    		return (size() == 0);
    	}
    	// Element access.
    	reference
    	operator[](size_type sz) 
    	{
    		return M_instance[sz];
    	}
    	const_reference
    	operator[](size_type sz) const
    	{
    		return M_instance[sz];
    	}
    	reference
    	at(size_type p)
    	{
    		if (p >= n)
    			throw;
    		return M_instance[p];
    	} 
    	const_reference
    	at(size_type p) const
    	{
    		if (p >= n)
    			throw;
    		return M_instance[p];
    	} 
    	reference
    	front()
    	{
    		return *begin();
    	} 
    	const_reference
    	front() const
    	{
    		return *begin();
    	} 
    	reference
    	back()
    	{
    		return n ? *(end() - 1) : *end(); 
    	} 
    	const_reference
    	back() const
    	{
    		return n ? *(end() - 1) : *end(); 
    	} 
    	pointer
    	data()
    	{
    		return std::addressof(M_instance[0]);
    	} 
    	const_pointer
    	data() const
    	{
    		return std::addressof(M_instance[0]);
    	}
    	void
    	swap(Array& other)
    	{
    		M_swap_array(other.begin()); 
    	} 
    	void
    	fill(const value_type& val)
    	{
    		M_fill_array(val);
    	}    	   	     	      	    	 	   	   			
		private:
			// Representation 

      // AUTOSAR 
      // Compliant Rule A10-0-2
			// Membership or non-public inheritance shall be used to implement “has-a” relationship.

			// uses built-in data array type
			// Stack memory
			// Support for zero-sized arrays mandatory.
			value_type M_instance[n ? n : 1]{};	
			
			// Implementation details	
			void
			M_swap_array(Array& other)
			{
    		lib_impl::swap_ranges(begin(), end(), other.begin()); 			
			}
			void
			M_fill_array(const T& val)
			{
    		lib_impl::fill_n(begin(), size(), val);			
			}
			void
			M_copy(std::initializer_list<T> l)
			{
				lib_impl::uninitialized_copy(l.begin(),l.end(),begin());			
			}				
	};
	//---
	// Non member functions	
	// Comparisons.
	template<typename T, 
					std::size_t n>
	inline 
	bool
	operator==(const Array<T, n>& lhs, 
						const Array<T, n>& rhs)
	{
		return std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}
	template<typename T, 
					std::size_t n>
	inline 
	bool
	operator!=(const Array<T, n>& lhs, 
						const Array<T, n>& rhs)
	{
		return !(lhs == rhs); 
	}	
	template<typename T, 
					std::size_t n>
	inline 
	bool
	operator<(const Array<T, n>& lhs, 
						const Array<T, n>& rhs)
	{
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()); 
	}
	template<typename T, 
					std::size_t n>		
	inline 
	bool
	operator>(const Array<T, n>& lhs, 
						const Array<T, n>& rhs)
	{
		return (rhs < lhs); 
	}
	template<typename T, 
					std::size_t n>		
	inline 
	bool
	operator<=(const Array<T, n>& lhs, 
						const Array<T, n>& rhs)
	{
		return !(lhs > rhs);
	}
	template<typename T, 
					std::size_t n>		
	inline 
	bool
	operator>=(const Array<T, n>& lhs, 
						const Array<T, n>& rhs)
	{
		return !(lhs < rhs);
	}
	template<typename T, 
					std::size_t n>		
	inline 
	void
	swap(const Array<T, n>& arr1, 
						const Array<T, n>& arr2)
	{
		return arr1.swap(arr2);
	}				
}
//--- NS lib
//--- User code
namespace
helper
{
  // MISRA C++ Rule 7–3–4
  // using-directives shall not be used
  // exception for _CLASS_REQUIRES expansion
	//using namespace type;
  // compliant using-declaration
	using Array_t 	= lib::Array<int,5>;
 
  // MISRA C++ Rule 0–1–10
  // Every defined function shall be called at least once.
	template <class Itor>
	void
	print(Itor first, Itor last)
	{
    for (auto it = first; it != last; ++it)
    {
        std::cout << *it << '_';
    }
    std::cout << '\n';	
	}
	void
	use()
	{
		Array_t arr1{2,3,5,7,9};
		Array_t arr2{1,3,5,10,15};
    // MISRA C++ Rule 2–10–2
    // Identifiers declaration 
		helper::print(arr1.begin(),arr1.end());
		helper::print(arr2.begin(),arr2.end());
		if (arr1 <= arr2)
		{
			std::cout << "arr1 <= arr2\n";
		}
		else if (arr1 > arr2)
		{
			std::cout << "arr1 > arr2\n";		
		}
		else
		{
			std::cout << "arr1 not comparable to arr2\n";		
		}
    // "rule of zero"
    // implicitly generated copy assignment function
    arr2 = arr1;
 		helper::print(arr2.begin(),arr2.end());	   
		arr1.fill(5);
		helper::print(arr1.begin(),arr1.end());	
    // "rule of zero"
    // implicitly generated copy constructor function
    Array_t arr3(arr2);
 		helper::print(arr3.begin(),arr3.end());	
    		
		class
		A
		{
			public:
				A(){};
        // MISRAC C++
        // Non compliant Rule 2–7–3
        // Sections of code should not be “commented out” using C++ comments.

        // Uncomment below to test Array concept requirements

				//A(const A&)=delete;
				//A&
				//operator=(const A&)=delete;
		};

    // MISRA C++ Rule 0–1–11
    // unused variables    
		lib::Array<A,1> test{};	
    static_cast<void>(test);
		throw std::runtime_error("Array::use() error");
	}
}
//--- NS helper
//--- Driver code
int
main(int argc,
		char* argv[])
{
  // MISRA C++ Rule 0–1–11
  // There shall be no unused named parameters 
  static_cast<void>(argc);
  static_cast<void>(argv);
	// Asynchronous task-based concurrency
	// store exception thrown 
  std::promise<int> prom;
  std::future<int> fut = prom.get_future();

	// thread-based concurrency
  std::thread 
  t(
  	[&prom]
		{
		  try 
		  {
		  	helper::use();
		  } 
		  catch(...) 
		  {
		    try 
		    {
		    	// transfer an exception thrown on one thread to a handler on another thread
		    	
		      // store anything thrown in the promise
		      prom.set_exception(std::current_exception());
		      // or throw a custom exception instead
		      // p.set_exception(std::make_exception_ptr(MyException("mine")));
		    } 
		    catch(...) 
		    {
		    	// set_exception() may throw too
		    } 
		  }
		}
  );
  //---

  try 
  {
  	// throws stored exception from thread
    std::cout << fut.get();
  } 
  catch(const std::exception& err) 
  {
    std::cout << "Exception from thread: " << err.what() << '\n';
  } 
	// If an exception is not caught on a thread std::terminate() is called
	catch (...) 
	{	
	}
	t.join();
	return 0;
}