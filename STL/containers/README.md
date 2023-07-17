# Standard Library Containers
A container holds a sequence of objects and the STL containers can be categorized into two groups:

* Sequence containers provide access to (half-open) sequences of elements.

    `<vector>`

    `<list>`
    
    `<deque>`

* Associative containers provide associative lookup based on a key.

    -   Ordered

        `<map>`
        - `multimap`

        `<set>`
        - `multiset`

    -   Unordered

        `<unordered_map>`
        - `unordered_multimap`

        `<unordered_set>`
        - `unordered_multiset`


These are usually implemented as Red-Black balanced trees of nodes pointing to (key,value) pairs type.
    `<stl_tree.h>`

There are also types of objects that hold elements while not offering all
of the facilities of sequence containers or associative containers:

* Container adaptors provide specialized access to underlying containers.

    `<queue>`   - deque is the default container used 

    `<stack>`   - vector is the default container used


* Almost containers are sequences of elements that provide most, but not all, of the facilities
of a container.

    `<array>`

    `<string>`  - `basic_string`
    
    `<bitset>`

For the purpose of this series of learning C++ by example, the focus will be on the following:

    `<array>`

    `<vector>`

    `<list>`

    `<deque>`

    `<stl_tree.h>`


Mastering these five ensures a good foundation for a complete understanding of the implementation details of the remaining containers and adaptors listed earlier.
For the associative containers an understanding of the Red-Black balanced tree implementation details found in the `stl_tree.h` is essential.
To improve the readability of examples only code relevant to the user library shall be included in the named namespace `lib` and implementation details shall be in the named namespace `lib_impl`. Examples of usage of the user library shall be included in the named namespace `helper` with a `use` function to demonstrate various conatiner operations.
Container concept requirements details to ensure principles in the notes below are maintained shall be in the named namespace `type`.

Notes:

* To be an element of a container, an object must be of a type that allows the container implementation to copy or move it, and to swap elements.

* The STL containers (the sequence and associative containers) are all resource handles with copy and move operations.

* All operations on containers provide the basic guarantee to ensure that they interact properly with exception-based error handling.

* Exception-safety basic guarantee ensures the basic invariants of all objects are maintained and no resources such as memory are leaked.