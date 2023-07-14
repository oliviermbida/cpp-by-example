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

// Type Checking
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
	template <class T>
	struct 
	BidirectionalIteratorConcept
	{
		void 
		__constraints() 
		{
			__function_requires< ForwardIteratorConcept<T> >();
			__function_requires< ConvertibleConcept<
																								typename std::iterator_traits<T>::iterator_category,
																								std::bidirectional_iterator_tag> >();
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
		void 
		__constraints() 
		{
			__function_requires< BidirectionalIteratorConcept<T> >();
			__function_requires< ComparableConcept<T> >();
			__function_requires< ConvertibleConcept<
																								typename std::iterator_traits<T>::iterator_category,
																								std::random_access_iterator_tag> >();
			typedef typename std::iterator_traits<T>::reference _Ref;
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
			(void)__i[__n];                   
		}
		T __a;
		T __b;
		T __i;
		T __j;
		typename std::iterator_traits<T>::difference_type __n;
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

//-- User library
namespace
lib
{
	// type checking
	using namespace type;
	
	template<class T,
					std::size_t n>
	class
	Array
	{
			// Concept requirements 
			typedef T* iter;
			_CLASS_REQUIRES(T,SGIAssignableConcept);
			_CLASS_REQUIRES(iter,RandomAccessIteratorConcept);
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
			// "has-a relationship"
			// uses built-in data array type
			// Stack memory
			// Support for zero-sized arrays mandatory.
			value_type M_instance[n ? n : 1];	
			
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