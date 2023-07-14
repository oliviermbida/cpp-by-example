# cpp-by-example
When I started learning C++, this was a frequent question: 

What does this piece of code or function do and how was it implemented or defined?

This is common since most libraries hide implementation details from the user interface.
You often come across very elaborate function names and a struggle to locate their definition.
Helpful websites like [stackoverflow](https://stackoverflow.com/) are full of such queries.
Over the years I have stuck to asking such questions in my quest to fully understand.
It is a habbit I have not given up because I still need to use things I fully understand.
This does not in any way mean that it is a hinderance to [object oriented programming(OOP)](https://en.wikipedia.org/wiki/Object-oriented_programming) principles. 
Or the interface segregation principle of the [SOLID](https://en.wikipedia.org/wiki/SOLID) principles:

'Clients should not be forced to depend upon interfaces that they do not use.'

To the contrary those principles are better understood when you know what you are using.
This is the approach used in this series of learning C++ by example.
In the examples provided, implementation details will not be hiden so that the reader can fully understand. 
Once understood, as an excercise the user can implement ways of hiding details not required by the user.
For this reason, the first rule is to restrict the usage of the `#include` directive.
It shall be used only for libraries neccessary for the running of the basic functions of the `Driver code` in the examples such as `<iostream>` library of the STL.
Secondly to improve the readability of examples only code relevant to the user library shall be included in the named namespace `lib` and implementation details shall be in the named namespace `lib_impl`. This is also inline with those principles mentioned earlier.

Finally the examples shall use coding standards for readability. It shall also use software security standards such as MISRA C++, AUTOSAR C++ and others to encourage the habbit of coding using good practices. 
These coding standards are not meant to be used by exception because unsafe code is usually what is excluded in these standards. 
They are a subset of the complete C++ standard which gives more freedom to the programmer at the cost of some unknown behaviour.
This last point is important because a common instruction given in technical assessments is to code as you normally do. If you are applying for a job which requires certain skillsets they are often picked up by a reviewer looking at how you usually code.

# About the author
Olivier Mbida is a senior C++ developper with expertise in developing safety critical software in the UAV industry.