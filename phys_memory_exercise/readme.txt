Implement a virtual-to-physical address translation function for the x86 architecture in C.
To solve this task you need to understand address translation; see the official Intel manuals or articles on the topic.


    Function inputs:

     - virtual address

     - translation type (2 or 3 levels)

     - physical address of the translation tree root

     - callback to read memory at a physical address


    The function must return the physical address and an error code indicating whether
    translation succeeded.


    API to implement:


    // Read the given number of bytes of physical memory at the given address into the buffer.
    // Returns the number of bytes read (less than requested, or 0, means an error — out of bounds).

    typedef unsigned int (*PREAD_FUNC)(void *buf, const unsigned int size, const unsigned int physical_addr);


    // Virtual-to-physical address translation:

    // virt_addr  - virtual address to translate

    // level      - number of translation levels: 2 or 3 (legacy and PAE on x86)

    // root_addr  - physical address of the translation tree root (CR3)

    // read_func  - callback to read physical memory (see above)

    // Returns 0 on success, non-zero on error

    // phys_addr  - output: translated physical address

    int va2pa(const unsigned int virt_addr, const unsigned int level, const unsigned int root_addr,
    const PREAD_FUNC read_func, unsigned int *phys_addr)


    Access-rights checks may be omitted for simplicity; implementing them is a bonus.

    64-bit translation may be omitted; implementing it is also a bonus.

    Which error cases to detect is up to you — the more, the better.


    Submit a file with the required function and any supporting code (helpers, tests, etc.).
    Standard libraries are allowed, though they are largely unnecessary for this task.
