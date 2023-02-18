# Use `auto` in small scopes, Stroustrup §6.3.6.1
It is more resilient to code changes or refactoring.

# Example
			// library.h
			
			// Use default initialization
			extern std::vector<int16_t> my_numbers;
			extern std::vector<std::string> my_words;
						
			template<class T> void current_list ( T& );
			
			//-----------------------------
			// library.cpp
			
			#include "library.h"

			// 1. Default implementation
			template<class T>
			void 
			current_list (T& list)
			{
				for(const auto& element: list)
				{
					std::cout << element << std::endl;
				};
			}
			
`auto` will deduce type of `element` from initializer of `list` type.

			// Explicit instantiation
			template void current_list (std::vector<std::string>&);

You can also change the behaviour depending on the `list` type.
			// 2. Explicit specialization
			template<>
			void 
			current_list (std::vector<int16_t>& list)
			{
				for(const auto& element: list)
				{
					if (element == 3 || element == 7)
					{
						continue;
					}
					else
					{
						std::cout << element << std::endl;
					}
				};
			}

It is good practice to put your program defaults in a separate file.

			//-----------------------------
			// defaults.cpp

			// Default initialization
			std::vector<int16_t> my_numbers{1,2,3,4,5,6,7,8,9};
			std::vector<std::string> my_words{"Hello","Welcome","C++"};

 You shall not change every instance of `current_list` function in your code because the overload resolution will do the job for you by calling the correct function.
 
			//-----------------------------
			// main.cpp
			#include "library.h"
		
			// Calls 1 (explicit instantiation)
			current_list(my_words);
			
			// Calls 2 (explicit specialization)
			current_list(my_numbers);

			// Override default initialization
			my_words = {"Goodbye","End"};
			current_list(my_words);

			//------------------------------
			g++ main.cpp defaults.cpp library.cpp -o test
			./test
			
			// Program output
			
			Hello
			Welcome
			C++
			1
			2
			4
			5
			6
			8
			9
			Goodbye
			End

# Reference
- The C++ Programming Language 4th Edition Bjarne Stroustrup
