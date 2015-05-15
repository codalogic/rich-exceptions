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

Each rich exception has a mandatory `error-uri`, an optional set of name-value
pair parameters and a mandatory `description`.

The `error-uri` is of the form "com.codalogic.mymodule.merror", or similar,
rather than being an enumerated value.  This allows for more global
application of the module throwing the exceptions.

The name-value parameter pairs allow for extra detail about the error.
For example, the file name that could not be read.  Each value within the
name-value pair is stored as a std::string, but template functions allow
creation of the value from non-std::string types via std::stringstream.

The `description` is intended to be a less-technical, user intelligable string
that can serve as a default error message higher up in the exception handling
if necessary.

A basic example of a function throwing, and then another re-throwing might look like:

```cpp
void throw_2_first()
{
    throw RichException( "com.codalogic.nexp.show_2_first", "First exception of 2 show" );
}

void throw_2_second()
{
    try
    {
        throw_2_first();
    }
    catch( RichException & e )
    {
        throw RichException( "com.codalogic.nexp.show_2_second", "Second exception of 2 show", &e );
    }
}

void show_throw_2()
{
    try
    {
        throw_2_second();
    }
    catch( RichException & e )
    {
        Verify( strcmp( e.main_error_uri(), "com.codalogic.nexp.show_2_second" ) == 0,
                "Is throw_2 'main_error_uri()' OK?" );

        Verify( strcmp( e.what(), "Second exception of 2 show" ) == 0,
                "Is throw_2 'what()' description OK?" );
    }
}
```

Name-value pair parameters can be included in the exception by doing:

```cpp
void throw_rich_exception_with_params_1()
{
    throw RichException( "com.codalogic.nexp.show_throw_with_params",
                         RichExceptionParams( "p1", "first" ).add( "p2", 2 ),
                         "Throw with params" );
}
```
Or:

```cpp
void throw_rich_exception_with_params_2()
{
    throw RichException( "com.codalogic.nexp.show_throw_with_params",
                         "Throw with params"
                         ).add( "p1", "first" ).add( "p2", 2 );
}
```

Most likely you wouldn't throw an instance of `RichException` directly, but derive an
exception class from it, for example:

```cpp
struct FileException : public RichException
{
    FileException( const std::string & file_name_in, RichException * p_prev = 0 )
        :
        RichException( "com.codalogic.file.noopen",
                        RichExceptionParams( "name", file_name_in ),
                        "Unable to open file",
                        p_prev )
    {}
};

struct DatabaseException : public RichException
{
    DatabaseException( int row, int column, RichException * p_prev = 0 )
        :
        RichException( "com.codalogic.database.badcell",
                        "Unable to access database cell",
                        p_prev )
    {
        add( "row", row );
        add( "column", column );
    }
};

void throw_2_first_with_derived_exceptions()
{
    throw FileException( "abc.txt" );
}

void throw_2_second_with_derived_exceptions( int row, int column )
{
    try
    {
        throw_2_first_with_derived_exceptions();
    }
    catch( FileException & e )
    {
        throw DatabaseException( row, column, &e );
    }
}

void show_throw_2_with_derived_exceptions()
{
    try
    {
        throw_2_second_with_derived_exceptions( 1, 2 );

        Bad( "throw_2_second_with_derived_exceptions() did not throw" );
    }
    catch( DatabaseException & e )
    {
    ...
```

In fact, it is highly recommended that the changes in code execution sequence
is influenced only by the type of the derived exception, rather than the 
`error_uri` or parameters within the `RichException` base (which are intended
for diagnostics when a fault is found).

The exception hierarchy can be inspected by iterating through them via the
the `RichException::begin()` and `end()` methods.

```cpp
    ...
    catch( DatabaseException & e )
    {
        Good( "throw_2_second_with_derived_exceptions() threw" );

        VerifyCritical( e.size() == 2, "Is DatabaseException correct size()?" );

        RichException::const_iterator i_rich_exception = e.begin();

        Verify( strcmp( i_rich_exception->error_uri, "com.codalogic.database.badcell" ) == 0,
                "Is DatabaseException error_uri correct?" );
        Verify( strcmp( i_rich_exception->description, "Unable to access database cell" ) == 0,
                "Is DatabaseException description correct?" );

        VerifyCritical( i_rich_exception->error_params.size() == 2,
                        "Is size of DatabaseException params correct?" );

        Verify( strcmp( i_rich_exception->error_params[0].name, "row" ) == 0,
                "Is DatabaseException exception first param correct?" );
        Verify( i_rich_exception->error_params[0].value == "1",
                "Is DatabaseException exception first value name correct?" );

        Verify( strcmp( i_rich_exception->error_params[1].name, "column" ) == 0,
                "Is DatabaseException exception 2nd param name correct?" );
        Verify( i_rich_exception->error_params[1].value == "2",
                "Is DatabaseException exception 2nd value correct?" );

        ++i_rich_exception;

        Verify( strcmp( i_rich_exception->error_uri, "com.codalogic.file.noopen" ) == 0,
                "Is FileException error_uri correct?" );
        Verify( strcmp( i_rich_exception->description, "Unable to open file" ) == 0,
                "Is FileException description correct?" );

        VerifyCritical( i_rich_exception->error_params.size() == 1,
                "Is size of FileException params correct?" );

        Verify( strcmp( i_rich_exception->error_params[0].name, "name" ) == 0,
                "Is FileException exception first param name correct?" );
        Verify( i_rich_exception->error_params[0].value == "abc.txt",
                "Is FileException exception first value correct?" );
    }
}
```

Naturally, (and possibly more sensibly when not trying to verify the code),
an iterator-based for-loop can be used.

When iterating through the exceptions, the most-recent (highest-level) exception
is presented first.  `RichException::rbegin()` and `rend()` allow iterating in
the reverse direction.

Within each exception level, the name-value pairs can be accessed by integer
index, or the `RichExceptionParams::has(...)` and `RichExceptionParams::get(...)`
methods, for example:

```cpp
void show_has_and_get_parameter_access()
{
    DatabaseException database_exception( 1, 2 );

    VerifyCritical( database_exception.size() == 1, "Is DatabaseException correct size()?" );

    RichException::const_iterator i_rich_exception = database_exception.begin();

    Verify( i_rich_exception->error_params.has( "row" ),
            "Does DatabaseException exception have row member?" );
    Verify( i_rich_exception->error_params.get( "row" ) == "1",
            "Is DatabaseException exception row value correct?" );

    Verify( i_rich_exception->error_params.has( "column" ),
            "Does DatabaseException exception have column member?" );
    Verify( i_rich_exception->error_params.get( "column" ) == "2",
            "Is DatabaseException exception column value correct?" );

    Verify( ! i_rich_exception->error_params.has( "Not there" ),
            "Is DatabaseException exception unknown param absent?" );
    Verify( i_rich_exception->error_params.get( "Not there" ) == "",
            "Is DatabaseException exception unknown param safely returned?" );
}
```

As can be seen in the above example, trying to `get()` a non-existent
parameter returns an empty string.

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
