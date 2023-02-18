# Use subset of STL by limiting behaviour.
Do not directly include standard libraries unless full visibility is required and those libraries are compliant to your required standards.

# Example
		// library.h
		
		#ifndef _LIBRARY_H_
		#define _LIBRARY_H_
		
		// Vector container from STL
		#include <vector>
		#include <string>

		// MISRA C++ 2008 Rule 3–3–1
		// Objects or functions with external linkage shall be declared in a header file.
		extern std::vector<std::string> my_words;	

# Use subset of vector container from STL by limiting behaviour using `delete`.

- 1. Assignment to vector container by a list initializer shall not be used.

			template<>
			std::vector<std::string>& 
			std::vector<std::string>::operator=(std::initializer_list<std::string>) = delete;
		
- 2. Assignment to vector container by reference to a constant vector string shall not be used

			template<>
			std::vector<std::string>& 
			std::vector<std::string>::operator=(const std::vector<std::string>&) = delete;
	
- 3. Assignment to vector container by rvalue reference to a vector string shall not be used

			template<>
			std::vector<std::string>& 
			std::vector<std::string>::operator=(std::vector<std::string>&&) = delete;

			// End of _LIBRARY_H_		
			#endif
		
It is good practice to put your program defaults in a separate file.

		//-----------------------------
		// defaults.cpp

		// Default initialization
		std::vector<std::string> my_words{"Hello","Welcome","C++"};

Indirectly include vector container library with the subset	
		//-----------------------------
		// main.cpp
		
		// vector container library with the subset
		#include "library.h"
		
		// Calls 1 (initializer list)
		my_words = {"Goodbye","End"};	// Error: use of deleted function
		
		// Calls 2 (copy assignment)
		std::vector<std::string> two_words = {"Goodbye","End"};
		my_words = two_words;		// Error: use of deleted function
		
		// Calls 3 (move assignment)
		std::vector<std::string> two_words{"Goodbye","End"};
		my_words = static_cast<std::vector<std::string>&&>(two_words);	
						// Error: use of deleted function

# Note: Memory management 
The move assignment (Call 3 above) is more efficient and therefore the function should not be removed. You will then have a subset of the vector container from STL which only allows copying via the move assignment and efficient in memory management.

# Reference
- The C++ Programming Language 4th Edition Bjarne Stroustrup §7.7.2 Rvalue References
