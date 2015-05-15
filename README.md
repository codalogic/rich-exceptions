rich-exceptions
===============

I have been experimenting with having calling functions specifying the
exceptions they throw under error conditions.  This appraoch produces
very attractive code in terms of separating out error handling from
the 'clean' path, but the re-writing of exceptions means that the
details of why the errors happened lower down can be lost.

This experiment introduces the idea of rich exceptions, that can include
more information about why errors occurred than typical exceptions do.
They also include details of the exception that caused the exception
re-throwing.


Warnings
========
RichException allocates memory as part of recording an exception.  This
can be problematic if memory is exhausted while handling an exception.
Under these conditions, std::terminate would be called, which may mean
a less elegant response to memory exhaustion than handling std::bad_alloc.

See Also
========
C++11 defines std::nested_exception.  This allows you to preserve a stack
of exceptions and see what led to what.  I have not used this mechanism,
because I'm still not using C++11 [:-(] and also, the method of nesting
into the earlier exceptions is essentially a destructive operation that
you can't readily unwind.  Preserving the more recent exceptions while
looking at the earlier exceptions seems like it might be useful!
