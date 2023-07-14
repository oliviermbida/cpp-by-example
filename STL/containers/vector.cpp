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

//-- User library
namespace
lib
{
	using namespace type;
	
	// Vector Base
	// Memory management
	// Nothing construct or destroy T elements
	template <typename T, 
							typename A>
	struct
	Vector_base
	{
		typedef typename A::template rebind<T>::other T_alloc_type;
		struct 
		Vector_impl
			: public T_alloc_type
		{
			// Representation
			typename T_alloc_type::pointer M_start{};
			typename T_alloc_type::pointer M_finish{};	
			typename T_alloc_type::pointer M_end{};	
			
			Vector_impl(const T_alloc_type& a)
				: T_alloc_type(a), 
					M_start(0), 
					M_finish(0), 
					M_end(0)
			{
			}
			Vector_impl()
				: Vector_impl(T_alloc_type()) 
			{
			}						
		};
		// Representation		
		Vector_impl M_impl;
		
		typename T_alloc_type::pointer
		M_allocate(std::size_t n)
		{
			return (n != 0 ? M_impl.allocate(n) : 0);
		}
		void
		M_deallocate(typename T_alloc_type::pointer p, 
									std::size_t n)
		{
			if (p)
			{
				M_impl.deallocate(p, n);
			}
		}
		// Allocator access
		typedef A allocator_type;	
		allocator_type&	
		M_get_T_allocator()
		{
			return *static_cast<T_alloc_type*>(&this->M_impl);
		}
		const allocator_type&	
		M_get_T_allocator() const
		{
			return *static_cast<const T_alloc_type*>(&this->M_impl);
		}
		allocator_type
		get_allocator() const	
		{
			return allocator_type(M_get_T_allocator());
		}
		//
		Vector_base(std::size_t n, 
								const allocator_type& a)	
			: M_impl(a)
		{
			// allocate memory
			this->M_impl.M_start = this->M_allocate(n);
			this->M_impl.M_finish = this->M_impl.M_start;
			this->M_impl.M_end = this->M_impl.M_start + n;
		}
		Vector_base(std::size_t n)
			: Vector_base(n, get_allocator())
		{
		}
		Vector_base()
			: Vector_base(T(), get_allocator())
		{
		}
		Vector_base(const allocator_type& a)
			: Vector_base(T(), a)
		{
		}
		// No copy
		Vector_base(const Vector_base&)	= delete;
		Vector_base&
		operator=(const Vector_base&) = delete;
		// Allow move	
		Vector_base(Vector_base&& other)
			: M_impl(other.M_get_T_allocator())
		{
			this->M_impl.M_start = other.M_impl.M_start;
			this->M_impl.M_finish = other.M_impl.M_finish;
			this->M_impl.M_end = other.M_impl.M_end;
			// stable state
			other.M_impl.M_start = 0;
			other.M_impl.M_finish = 0;
			other.M_impl.M_end = 0;
		}
		Vector_base&
		operator=(Vector_base&& other)
		{
			std::swap(M_impl.M_start,other.M_impl.M_start);
			std::swap(M_impl.M_finish,other.M_impl.M_finish);
			std::swap(M_impl.M_end,other.M_impl.M_end);	
			return *this;					
		}
		~Vector_base()
		{
			M_deallocate(this->M_impl.M_start, 
										this->M_impl.M_end - this->M_impl.M_start);										
		}
	};
	//---
	// Vector
	
	template <typename T, 
						typename A>
	class
	Vector
		// AUTOSAR Rule A10-0-2
		// Membership or non-public inheritance shall be used to implement “has-a” relationship.
		: protected Vector_base<T, A>
	{			
		// Private implementation details
		using Alloc_value_type					= typename A::value_type; 
		using Base											= Vector_base<T, A>;
		using T_alloc_type							= typename Base::T_alloc_type;
		template <typename U>
		using Iter											= lib_impl::Normal_iterator<U,Vector>;
		template <typename U>
		using Reverse_iter							= lib_impl::Reverse_iterator<U>;	
		
		// Concept requirements 
		_CLASS_REQUIRES(T,SGIAssignableConcept);	
							
		public:
			using value_type							= T;
			using pointer									= typename T_alloc_type::pointer;
			using const_pointer						= typename T_alloc_type::const_pointer;
			using reference								= typename T_alloc_type::reference;
			using const_reference					= typename T_alloc_type::const_reference;	
			using iterator								= Iter<pointer>;	
			using const_iterator					= Iter<const_pointer>;	
			using reverse_iterator				= Reverse_iter<iterator>;
			using const_reverse_iterator	= Reverse_iter<const_iterator>;
			using size_type								= std::size_t;
			using difference_type					= std::ptrdiff_t;
			using allocator_type					= A;
	
			explicit
			Vector(size_type n, 
							const value_type& val = value_type(),
							const allocator_type& a = allocator_type())
					// alloacate memory
				: Base(n, a)
			{
				// initialize
				M_fill_initialize(n, val);
			}
			Vector()
				: Vector(size_type(0), value_type(0), allocator_type())
			{
			}
			Vector(const allocator_type& a)
				: Vector(size_type(0), value_type(0), a)
			{
			}	
			explicit
			Vector(size_type n)		
				: Vector(n, value_type(0), allocator_type())
			{
			}
			// "rule of five" special functions
			Vector(const Vector& other)	
				: Base(other.size(), other.M_get_T_allocator())	
			{
				this->M_impl.M_finish = M_copy_initialize(other.begin(), 
																										other.end(), 
																										this->M_impl.M_start);
			}
			Vector&
			operator=(const Vector& other);
			Vector(Vector&& other)
				: Base(std::move(other))	
			{
			}	
			Vector&
			operator=(Vector&& other)
			{
				this->clear();
				this->swap(other);
				return *this;
			}					
			~Vector()
			{
				M_destroy(this->M_impl.M_start, this->M_impl.M_finish);
			}
			Vector(lib_impl::initializer_list<value_type> l)
				: Base(l.size())
			{
				this->assign(l.begin(), l.end());
			}			
			Vector&
			operator=(lib_impl::initializer_list<value_type> l)
			{
				this->assign(l.begin(), l.end());
				return *this;
			}
			iterator
			begin()
			{ 
				return iterator(this->M_impl.M_start); 
			}
			const_iterator
			begin() const
			{ 
				return const_iterator(this->M_impl.M_start); 
			}
			iterator
			end()
			{ 
				return iterator(this->M_impl.M_finish); 
			}	
			const_iterator
			end() const
			{ 
				return const_iterator(this->M_impl.M_finish); 
			}
			reverse_iterator
			rbegin()
			{ 
				return reverse_iterator(end()); 
			}
			const_reverse_iterator
			rbegin() const
			{ 
				return const_reverse_iterator(end()); 
			}
			reverse_iterator
			rend()
			{ 
				return reverse_iterator(begin()); 
			}		
			const_reverse_iterator
			rend() const
			{ 
				return const_reverse_iterator(begin()); 
			}	
			size_type
			size() const
			{ 
				return size_type(this->M_impl.M_finish - this->M_impl.M_start); 
			}	
			size_type
			max_size() const
			{ 
				return M_get_T_allocator().max_size(); 
			}	
			size_type
			capacity()
			{ 
				return size_type(this->M_impl.M_end - this->M_impl.M_start);	
			}						
			void
			swap(Vector& other)	
			{
				std::swap(this->M_impl.M_start, other.M_impl.M_start);
				std::swap(this->M_impl.M_finish, other.M_impl.M_finish);
				std::swap(this->M_impl.M_end, other.M_impl.M_end);
				std::__alloc_swap<T_alloc_type>::_S_do_it(M_get_T_allocator(), other.M_get_T_allocator());
			}
			void
			clear()
			{
				M_erase_at_end(this->M_impl.M_start);
			}
			bool
			empty() const
			{
				return bool(begin() == end());
			}
			reference
			operator[](size_type n)
			{
				M_range_check(n);			
				return *(this->M_impl.M_start + n); 
			}
			const_reference
			operator[](size_type n) const
			{
				M_range_check(n);			
				return *(this->M_impl.M_start + n); 
			}
			reference
			at(size_type n)
			{
				M_range_check(n);
				return (*this)[n];
			}	
			const_reference
			at(size_type n) const
			{
				M_range_check(n);
				return (*this)[n];
			}
			reference
			front()
			{
				return (*begin());
			}	
			const_reference
			front() const
			{
				return (*begin());
			}
			reference
			back()
			{
				return (*(end() - 1));
			}	
			const_reference
			back() const
			{
				return (*(end() - 1));
			}	
			pointer
			data()
			{
				return std::addressof(front());
			}	
			const_pointer
			data() const
			{
				return std::addressof(front());
			}
			void
			push_back(const_reference val)	
			{
				if (this->M_impl.M_finish != this->M_impl.M_end)
				{
					this->M_impl.construct(this->M_impl.M_finish, val);
					++this->M_impl.M_finish;
				}
				else
				{
					M_insert_aux(end(), val);
				}
			}
			void
			push_back(value_type&& val)	
			{
				emplace_back(std::move(val));
			}
			
			template <typename... Args>	
			void
			emplace_back(Args&&... args)
			{
				if (this->M_impl.M_finish != this->M_impl.M_end)
				{
					this->M_impl.construct(this->M_impl.M_finish,
																	std::forward<Args>(args)...);
					++this->M_impl.M_finish;
				}
				else
				{
					M_insert_aux(end(), std::forward<Args>(args)...);
				}
			}	
			void
			pop_back()
			{
				--this->M_impl.M_finish;
				this->M_impl.destroy(this->M_impl.M_finish);
			}								
			void
			assign(size_type n, 
							const value_type& val)	
			{
				M_fill_assign(n, val);
			}
			void
			assign(const std::initializer_list<value_type> l)
			{
				this->assign(l.begin(), l.end());
			}
			template <typename InputIterator>	
			void
			assign(InputIterator first,
							InputIterator last)
			{
				pointer curr(this->M_impl.M_start);
				for (; first != last && curr != this->M_impl.M_finish; ++curr, ++first)
					*curr = *first;
				if (first == last)
				{
					M_erase_at_end(curr);
				}
				else
				{
					insert(end(), first, last);
				}
			}
			template <typename InputIterator>	
			void
			insert(iterator pos, 
			 				InputIterator first,	
			 				InputIterator last)
			{
				M_range_insert(pos, first, last); 	
			}
			iterator
			insert(iterator pos, 
							const value_type& val)
			{
				const size_type n = pos - begin();
				if (this->M_impl.M_finish != this->M_impl.M_end
							&& pos == end()) 
				{
					this->M_impl.construct(this->M_impl.M_finish, val);
					++this->M_impl.M_finish;
				}
				else
				{
					if (this->M_impl.M_finish != this->M_impl.M_end)
					{
						value_type val_copy = val;
						M_insert_aux(pos, std::move(val_copy));
					}
					else
					{
						M_insert_aux(pos, val);
					}
				}
				return iterator(this->M_impl.M_start + n);
			} 			
			iterator
			insert(iterator pos, 
							value_type&& val)
			{
				return emplace(pos, std::move(val)); 
			} 
			void
			insert(iterator pos, 
							std::initializer_list<value_type> l)
			{
				this->insert(pos, l.begin(), l.end());
			}
			void
			insert(iterator pos, 
							size_type n, 
							const value_type& val)
			{
				M_fill_insert(pos, n, val);
			}
			template <typename... Args>
			iterator
			emplace(iterator pos, 
								Args&&... args)
			{
				const size_type n = pos - begin();
				if (this->M_impl.M_finish != this->M_impl.M_end && pos == end())
				{
					this->M_impl.construct(this->M_impl.M_finish,
																	std::forward<Args>(args)...);
					++this->M_impl.M_finish;
				}
				else
				{
					M_insert_aux(pos, std::forward<Args>(args)...);
				}
				return iterator(this->M_impl.M_start + n);
			}
			iterator
			erase(iterator pos)
			{
				if (pos + 1 != end())
				{
					std::move(pos + 1, end(), pos);				
				}
				--this->M_impl.M_finish;
				this->M_impl.destroy(this->M_impl.M_finish);
				return pos;
			}
			iterator
			erase(iterator first, 
							iterator last)
			{
				if (first != last)
				{
					if (last != end())
					{
						std::move(last, end(), first);
					}
					M_erase_at_end(first.base() + (end() - last));
				}
				return first;
			}
			
			void
			reserve(size_type n);
			
			void
			resize(size_type new_size, 
							value_type val = value_type())
			{
				if (new_size > size())
				{
					insert(end(), new_size - size(), val);
				}
				else if (new_size < size())
				{
					M_erase_at_end(this->M_impl.M_start + new_size);
				}
			}
		protected:
			// Representation
			using Base::M_impl;
			using Base::M_allocate;
			using Base::M_deallocate;
			using Base::M_get_T_allocator;
			using Base::get_allocator;
			
			// Member functions
			void
			M_fill_initialize(size_type n, 
												const value_type& val)
			{
				lib_impl::uninitialized_fill_n_a(this->M_impl.M_start, n, val, M_get_T_allocator());
				this->M_impl.M_finish = this->M_impl.M_end;
			}
			
			template<typename InputIterator,
								typename ForwardIterator>
			ForwardIterator 
			M_copy_initialize(InputIterator first, 
														InputIterator last,
														ForwardIterator	result)
			{
				return lib_impl::uninitialized_copy_a(first, 
																							last, 
																							result,
																							M_get_T_allocator());			
			}
				
			template<typename InputIterator>
			void 
			M_destroy(InputIterator first, 
									InputIterator last)
			{
				lib_impl::destroy_a(first,last,M_get_T_allocator());
			}
			
			template<typename InputIterator, 
								typename ForwardIterator>
			ForwardIterator
			M_copy(InputIterator first, 
			 			InputIterator last,
			 			ForwardIterator result)
			{	
				return lib_impl::copy(first,last,result);
			}
				
			template<typename ForwardIterator>
			pointer
			M_allocate_and_copy(size_type n,
													ForwardIterator first, 
													ForwardIterator last)	
			{
				pointer result = this->M_allocate(n);
				try
				{
					M_copy_initialize(first, last, result);
					return result;
				}
				catch(...)
				{
					M_deallocate(result, n);
					throw std::runtime_error{"Copy initialization failed"};											
				}
			}
			
			void
			M_erase_at_end(pointer pos)
			{
				M_destroy(pos, this->M_impl.M_finish);
				this->M_impl.M_finish = pos;
			}
			
			void
			M_fill_assign(size_type n, const value_type& val);
			
			template <typename InputIterator>
			void
			M_range_insert(iterator pos, 
											InputIterator first,
											InputIterator last)
			{
				for (; first != last; ++first)
				{
					pos = insert(pos, *first);
				}
			}
			
			template <typename... Args>
			void
			M_insert_aux(iterator pos, 
										Args&&... args);
			
			void
			M_fill_insert(iterator pos, 
										size_type n, 
										const value_type& val);
			
			size_type
			M_check_len(size_type n, 
									const char* s) const
			{
				if (max_size() - size() < n)
					throw std::runtime_error{s};
				const size_type len = size() + std::max(size(), n);
				return ((len < size() || len > max_size()) ? max_size() : len);											
			}
			
			void
			M_range_check(size_type n) const
			{
				if (n >= this->size())
				{
					throw std::runtime_error{"Vector::M_range_check"};
				}
			}
	};
	template <typename T,
						typename A>
	Vector<T,A>&
	Vector<T,A>::
	operator=(const Vector<T,A>& other)
	{
		// check for self copy	
		if (&other != this)
		{
			const size_type len = other.size();
			if (len > capacity())
			{
				pointer tmp = M_allocate_and_copy(len, other.begin(),other.end());
				M_destroy(this->M_impl.M_start, this->M_impl.M_finish);
				M_deallocate(this->M_impl.M_start,this->M_impl.M_end - this->M_impl.M_start);
				this->M_impl.M_start = tmp;
				this->M_impl.M_end = this->M_impl.M_start + len;
			}
			else if (size() >= len)
			{
				M_destroy(M_copy(other.begin(), other.end(), begin()),
									end());
			}
			else
			{
				M_copy(other.M_impl.M_start, other.M_impl.M_start + size(), this->M_impl.M_start);
				M_copy_initialize(other.M_impl.M_start + size(),
														other.M_impl.M_finish,
														this->M_impl.M_finish);
			}
			this->M_impl.M_finish = this->M_impl.M_start + len;
		}
		return *this;
	}
	
	template <typename T,
						typename A>
	void
	Vector<T,A>::
	M_fill_assign(size_type n, 
								const value_type& val)	
	{
		if (n > capacity())	
		{
			Vector tmp(n, val, M_get_T_allocator());
			tmp.swap(*this);
		}
		else if (n > size())
		{
			lib_impl::fill(begin(), end(), val);
			lib_impl::uninitialized_fill_n_a(this->M_impl.M_finish,
																				n - size(),
																				val,
																				M_get_T_allocator());
			this->M_impl.M_finish += n - size();
		}
		else
		{
			M_erase_at_end(lib_impl::fill_n(this->M_impl.M_start, n, val));
		}
	}
	
	template <typename T,
						typename A>
	template <typename... Args>
	void
	Vector<T,A>::	
	M_insert_aux(iterator pos, 
								Args&&... args)
	{
		if (this->M_impl.M_finish != this->M_impl.M_end)
		{
			this->M_impl.construct(this->M_impl.M_finish,
															std::move(*(this->M_impl.M_finish - 1)));
			++this->M_impl.M_finish;
			lib_impl::move_backward(pos.base(),
															this->M_impl.M_finish - 2,
															this->M_impl.M_finish - 1);
			*pos = T(std::forward<Args>(args)...);
		}
		else
		{
			const size_type len = M_check_len(size_type(1), "Vector::M_insert_aux length error");
			const size_type elems_before = pos - begin();
			pointer new_start(this->M_allocate(len));
			pointer new_finish(new_start);
			try
			{
				this->M_impl.construct(new_start + elems_before,
																std::forward<Args>(args)...);
				new_finish = 0;
				new_finish = lib_impl::uninitialized_move_a(this->M_impl.M_start,
																											pos.base(), 
																											new_start,
																											M_get_T_allocator());
				++new_finish;
				new_finish = lib_impl::uninitialized_move_a(pos.base(),
																											this->M_impl.M_finish,
																											new_finish,
																											M_get_T_allocator());
			}
			catch(...)
			{
				if (!new_finish)
				{
					this->M_impl.destroy(new_start + elems_before);
				}
				else
				{
					M_destroy(new_start, new_finish);
				}
				M_deallocate(new_start, len);
				throw std::runtime_error{"Move initialization failed"};											
			}
			M_destroy(this->M_impl.M_start, this->M_impl.M_finish);
			M_deallocate(this->M_impl.M_start,
										this->M_impl.M_end - this->M_impl.M_start);
			this->M_impl.M_start = new_start;
			this->M_impl.M_finish = new_finish;
			this->M_impl.M_end = new_start + len;
		}
	}	
	template <typename T,
						typename A>
	void
	Vector<T,A>::
	M_fill_insert(iterator pos, 
								size_type n, 
								const value_type& val)
	{
		if (n != 0)
		{
			if (size_type(this->M_impl.M_end
										- this->M_impl.M_finish) >= n)
			{
				value_type val_copy = val;
				const size_type elems_after = end() - pos;
				pointer old_finish(this->M_impl.M_finish);
				if (elems_after > n)
				{
					lib_impl::uninitialized_move_a(this->M_impl.M_finish - n,
																					this->M_impl.M_finish,
																					this->M_impl.M_finish,
																					M_get_T_allocator());
					this->M_impl.M_finish += n;
					std::move(pos.base(), old_finish - n, old_finish);
					lib_impl::fill(pos.base(), pos.base() + n, val_copy);
				}
				else
				{
					lib_impl::uninitialized_fill_n_a(this->M_impl.M_finish,
																						n - elems_after,
																						val_copy,
																						M_get_T_allocator());
					this->M_impl.M_finish += n - elems_after;
					lib_impl::uninitialized_move_a(pos.base(), 
																					old_finish,
																					this->M_impl.M_finish,
																					M_get_T_allocator());
					this->M_impl.M_finish += elems_after;
					lib_impl::fill(pos.base(), old_finish, val_copy);
				}
			}
			else
			{
				const size_type len = M_check_len(n, "Vector::M_fill_insert length error");
				const size_type elems_before = pos - begin();
				pointer new_start(this->M_allocate(len));
				pointer new_finish(new_start);
				try
				{
					lib_impl::uninitialized_fill_n_a(new_start + elems_before,
																						n, 
																						val,
																						M_get_T_allocator());
					new_finish = 0;
					new_finish = lib_impl::uninitialized_move_a(this->M_impl.M_start,
																												pos.base(),
																												new_start,
																												M_get_T_allocator());
					new_finish += n;
					new_finish = lib_impl::uninitialized_move_a(pos.base(),
																												this->M_impl.M_finish,
																												new_finish,
																												M_get_T_allocator());
				}
				catch(...)
				{
					if (!new_finish)
					{
						M_destroy(new_start + elems_before,
											new_start + elems_before + n);
					}
					else
					{
						M_destroy(new_start, new_finish);						
					}
					M_deallocate(new_start, len);
					throw std::runtime_error{"Vector::M_fill_insert error"};						
				}
				M_destroy(this->M_impl.M_start, this->M_impl.M_finish);
				M_deallocate(this->M_impl.M_start, this->M_impl.M_end - this->M_impl.M_start);
				this->M_impl.M_start = new_start;
				this->M_impl.M_finish = new_finish;
				this->M_impl.M_end = new_start + len;
			}
		}
	}	
	template <typename T,
						typename A>
	void
	Vector<T,A>::
	reserve(size_type n)	
	{
		if (n > this->max_size())
		{
			throw std::runtime_error{"Vector::reserve length error"};	
		}
		if (this->capacity() < n)
		{
			const size_type old_size = size();
			pointer tmp = M_allocate_and_copy(n,
																				std::move(this->M_impl.M_start),
																				std::move(this->M_impl.M_finish));
			M_destroy(this->M_impl.M_start, this->M_impl.M_finish);
			M_deallocate(this->M_impl.M_start,
										this->M_impl.M_end - this->M_impl.M_start);
			this->M_impl.M_start = tmp;
			this->M_impl.M_finish = tmp + old_size;
			this->M_impl.M_end = this->M_impl.M_start + n;
		}
	}
	
	template<typename T, 
						typename A>
	inline
	bool
	operator==(const Vector<T, A>& lhs, 
							const Vector<T, A>& rhs)
	{
		return (lhs.size() == rhs.size()
						&& std::equal(lhs.begin(), lhs.end(), rhs.begin()));
	}
	template<typename T, 
						typename A>
	inline
	bool
	operator!=(const Vector<T, A>& lhs, 
							const Vector<T, A>& rhs)
	{
		return (!(lhs == rhs));
	}	
	template<typename T, 
						typename A>
	inline
	bool
	operator<(const Vector<T, A>& lhs, 
							const Vector<T, A>& rhs)
	{
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),rhs.end());
	}
	template<typename T, 
						typename A>
	inline
	bool
	operator>(const Vector<T, A>& lhs, 
							const Vector<T, A>& rhs)
	{
		return (rhs < lhs);
	}	
	template<typename T, 
						typename A>
	inline
	bool
	operator<=(const Vector<T, A>& lhs, 
							const Vector<T, A>& rhs)
	{
		return (!(rhs < lhs));
	}	
	template<typename T, 
						typename A>
	inline
	bool
	operator>=(const Vector<T, A>& lhs, 
							const Vector<T, A>& rhs)
	{
		return (!(lhs < rhs));
	}	
	template<typename T, 
						typename A>
	inline
	void
	swap(Vector<T, A>& one, Vector<T, A>& two)
	{
		one.swap(two);
	}					
		// change implementation details
		// change reverse iterator
	template <typename T,
						typename A=std::allocator<T>>
	class
	VectorB
		: public Vector<T,A>
	{
		using Base											= Vector<T,A>;
		public: 
			using value_type							= typename Base::value_type;
			using pointer									= typename Base::pointer;
			using const_pointer						= typename Base::const_pointer;
			using reference								= typename Base::reference;
			using const_reference					= typename Base::const_reference;	
			using iterator								= typename Base::iterator;	
			using const_iterator					= typename Base::const_iterator;	
			using reverse_iterator				= std::reverse_iterator<iterator>;	
			using const_reverse_iterator	= std::reverse_iterator<const_iterator>;				
	};
	
}
//---End lib