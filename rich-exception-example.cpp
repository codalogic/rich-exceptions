//----------------------------------------------------------------------------
// Copyright (c) 2015, Codalogic Ltd (http://www.codalogic.com)
// All rights reserved.
//
// The license for this file is based on the BSD-3-Clause license
// (http://www.opensource.org/licenses/BSD-3-Clause).
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// - Neither the name Codalogic Ltd nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
// Require error when nonstandard extension used :
//      'token' : conversion from 'type' to 'type'
//      e.g. disallow foo( /*const*/ foo & ) for copy constructor
#pragma warning(error: 4239)
#endif

#include "rich-exception.h"

#include <string>
#include <iostream>
#include <cstring>

#include "annotate-lite.h"

using namespace rich_excep;

void show_multiple_linkage_ok();

void show_single_exception_class()
{
    Suite( "show_single_exception_class()" );

    RichException rich_exception( "com.codalogic.nexp.show1", "First exception show" );

    VerifyCritical( ! rich_exception.empty(), "Is exception non-empty?" );

    VerifyCritical( rich_exception.size() == 1, "Is exception size correct?" );

    Verify( strcmp( rich_exception.main_error_uri(), "com.codalogic.nexp.show1" ) == 0, "Is 'main_error_uri()' OK?" );

    Verify( strcmp( rich_exception.what(), "First exception show" ) == 0, "Is 'what()' description OK?" );

    std::exception & r_std_exception( rich_exception );

    Verify( strcmp( r_std_exception.what(), "First exception show" ) == 0, "Is 'what()' accessible via std::exception base?" );

    Verify( rich_exception.to_string() == "com.codalogic.nexp.show1: First exception show\n", "Is rich_exception.to_string() correct?" );
}

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
        VerifyCritical( ! e.empty(), "Is throw_2 exception non-empty?" );

        VerifyCritical( e.size() == 2, "Is throw_2 exception size correct?" );

        Verify( strcmp( e.main_error_uri(), "com.codalogic.nexp.show_2_second" ) == 0, "Is throw_2 'main_error_uri()' OK?" );

        Verify( strcmp( e.what(), "Second exception of 2 show" ) == 0, "Is throw_2 'what()' description OK?" );

        std::exception & r_std_exception( e );

        Verify( strcmp( e.what(), "Second exception of 2 show" ) == 0, "Is throw_2 'what()' accessible via std::exception base?" );

        Verify( e.to_string() == "com.codalogic.nexp.show_2_second: Second exception of 2 show\n"
                                    "  com.codalogic.nexp.show_2_first: First exception of 2 show\n",
                                "Is throw_2 rich_exception.to_string() correct?" );
    }
}

void show_params_storage()
{
    Suite( "show_params_storage()" );

    RichException rich_exception_1( "com.codalogic.nexp.show_params_storage.1",
                                    RichExceptionParams( "p1_1", "v1_1" ).add( "p1_2", 2 ).add( "p1_3", 3.0 ),
                                    "First show params exception 1 show" );

    RichException rich_exception_2( "com.codalogic.nexp.show_params_storage.2",
                                    RichExceptionParams( "p2_1", "v2_1" ).add( "p2_2", 2 ).add( "p2_3", 3.0 ),
                                    "First show params exception 2 show",
                                    &rich_exception_1 );

    Verify( rich_exception_1.size() == 0, "Has rich_exception_1 contents been passed to rich_exception_2?" );
    VerifyCritical( rich_exception_2.size() == 2, "Is rich_exception_2 exception size correct?" );

    RichException::const_iterator i_rich_exception = rich_exception_2.begin();

    Verify( strcmp( i_rich_exception->error_uri, "com.codalogic.nexp.show_params_storage.2" ) == 0, "Is first error_uri correct?" );
    Verify( strcmp( i_rich_exception->description, "First show params exception 2 show" ) == 0, "Is first description correct?" );
    VerifyCritical( i_rich_exception->error_params.size() == 3, "Is size of first params correct?" );
    Verify( strcmp( i_rich_exception->error_params[0].name, "p2_1" ) == 0, "Is first exception first param name correct?" );
    Verify( i_rich_exception->error_params[0].value == "v2_1", "Is first exception first value name correct?" );
    Verify( strcmp( i_rich_exception->error_params[1].name, "p2_2" ) == 0, "Is first exception 2nd param name correct?" );
    Verify( i_rich_exception->error_params[1].value == "2", "Is first exception 2nd value name correct?" );
    Verify( strcmp( i_rich_exception->error_params[2].name, "p2_3" ) == 0, "Is first exception 3rd param name correct?" );
    Verify( i_rich_exception->error_params[2].value == "3", "Is first exception 3rd value name correct?" );

    ++i_rich_exception;

    Verify( strcmp( i_rich_exception->error_uri, "com.codalogic.nexp.show_params_storage.1" ) == 0, "Is 2nd error_uri correct?" );
    Verify( strcmp( i_rich_exception->description, "First show params exception 1 show" ) == 0, "Is 2nd description correct?" );
    VerifyCritical( i_rich_exception->error_params.size() == 3, "Is size of 2nd params correct?" );
    Verify( strcmp( i_rich_exception->error_params[0].name, "p1_1" ) == 0, "Is 2nd exception first param name correct?" );
    Verify( i_rich_exception->error_params[0].value == "v1_1", "Is 2nd exception first value name correct?" );
    Verify( strcmp( i_rich_exception->error_params[1].name, "p1_2" ) == 0, "Is 2nd exception 2nd param name correct?" );
    Verify( i_rich_exception->error_params[1].value == "2", "Is 2nd exception 2nd value name correct?" );
    Verify( strcmp( i_rich_exception->error_params[2].name, "p1_3" ) == 0, "Is 2nd exception 3rd param name correct?" );
    Verify( i_rich_exception->error_params[2].value == "3", "Is 2nd exception 3rd value name correct?" );

    ++i_rich_exception;

    Verify( i_rich_exception == rich_exception_2.end(), "Has iterator reached end()?" );
}

void throw_rich_exception_with_params()
{
    throw RichException( "com.codalogic.nexp.show_throw_with_params", "Throw with params" ).add( "p1", "first" ).add( "p2", 2 );
}

void show_exception_with_params()
{
    Suite( "show_exception_with_params()" );

    try
    {
        throw_rich_exception_with_params();
        Bad( "throw_rich_exception_with_params did not throw" );
    }
    catch( RichException & e )
    {
        Good( "throw_rich_exception_with_params threw" );
        Verify( e.to_string() == "com.codalogic.nexp.show_throw_with_params (p1: first, p2: 2): Throw with params\n",
                                "Is throw_rich_exception_with_params to_string() correct?" );
    }
}

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
    Suite( "show_throw_2_with_derived_exceptions()" );

    try
    {
        throw_2_second_with_derived_exceptions( 1, 2 );
        Bad( "throw_2_second_with_derived_exceptions() did not throw" );
    }
    catch( DatabaseException & e )
    {
        Good( "throw_2_second_with_derived_exceptions() threw" );
        VerifyCritical( e.size() == 2, "Is DatabaseException correct size()?" );

        RichException::const_iterator i_rich_exception = e.begin();

        Verify( strcmp( i_rich_exception->error_uri, "com.codalogic.database.badcell" ) == 0, "Is DatabaseException error_uri correct?" );
        Verify( strcmp( i_rich_exception->description, "Unable to access database cell" ) == 0, "Is DatabaseException description correct?" );

        VerifyCritical( i_rich_exception->error_params.size() == 2, "Is size of DatabaseException params correct?" );

        Verify( strcmp( i_rich_exception->error_params[0].name, "row" ) == 0, "Is DatabaseException exception first param correct?" );
        Verify( i_rich_exception->error_params[0].value == "1", "Is DatabaseException exception first value name correct?" );

        Verify( strcmp( i_rich_exception->error_params[1].name, "column" ) == 0, "Is DatabaseException exception 2nd param name correct?" );
        Verify( i_rich_exception->error_params[1].value == "2", "Is DatabaseException exception 2nd value correct?" );

        ++i_rich_exception;

        Verify( strcmp( i_rich_exception->error_uri, "com.codalogic.file.noopen" ) == 0, "Is FileException error_uri correct?" );
        Verify( strcmp( i_rich_exception->description, "Unable to open file" ) == 0, "Is FileException description correct?" );

        VerifyCritical( i_rich_exception->error_params.size() == 1, "Is size of FileException params correct?" );

        Verify( strcmp( i_rich_exception->error_params[0].name, "name" ) == 0, "Is FileException exception first param name correct?" );
        Verify( i_rich_exception->error_params[0].value == "abc.txt", "Is FileException exception first value correct?" );
    }
}

void show_has_and_get_parameter_access()
{
    Suite( "show_has_and_get_parameter_access()" );

    DatabaseException database_exception( 1, 2 );

    VerifyCritical( database_exception.size() == 1, "Is DatabaseException correct size()?" );

    RichException::const_iterator i_rich_exception = database_exception.begin();

    Verify( i_rich_exception->error_params.has( "row" ), "Does DatabaseException exception have row member?" );
    Verify( i_rich_exception->error_params.get( "row" ) == "1", "Is DatabaseException exception row value correct?" );

    Verify( i_rich_exception->error_params.has( "column" ), "Does DatabaseException exception have column member?" );
    Verify( i_rich_exception->error_params.get( "column" ) == "2", "Is DatabaseException exception column value correct?" );

    Verify( ! i_rich_exception->error_params.has( "Not there" ), "Is DatabaseException exception unknown param absent?" );
    Verify( i_rich_exception->error_params.get( "Not there" ) == "", "Is DatabaseException exception unknown param safely returned?" );
}

// Reworked from https://github.com/codalogic/safe-divide

template< typename Texception >
double safe_divide( double i, double j )
{
    if( j == 0.0 ) throw Texception();
    return i / j;
}

struct or_k_is_zero : public RichException
{
    or_k_is_zero( RichException * p_prev = 0 )
        :
        RichException( "com.codalogic.f1.safe_divide.k_is_0",
                        "Divide by zero error",
                        p_prev )
    {}
};

template< typename Texception >
double f1( double i, double j, double k )
{
    try
    {
        return i + safe_divide<or_k_is_zero>(j,k);
    }
    catch( or_k_is_zero & e )
    {
        throw Texception( &e );
    }
}

struct or_default_f1_k_is_zero : public RichException
{
    static const char * const default_error_uri()
    {
        static const char * const error_uri = "com.codalogic.f1.default.k_is_0";
        return error_uri;
    }
    static const char * const default_description()
    {
        static const char * const description = "Divide by zero error";
        return description;
    }

    or_default_f1_k_is_zero( RichException * p_prev = 0 )
        :
        RichException( default_error_uri(), default_description(), p_prev )
    {}
    or_default_f1_k_is_zero( const char * error_uri_in, const char * description_in, RichException * p_prev = 0 )
        :
        RichException( error_uri_in, description_in, p_prev )
    {}
};

struct or_k1_is_zero : public or_default_f1_k_is_zero
{
    static const char * const error_uri()
    {
        static const char * const my_error_uri = "com.codalogic.show_rework_of_safe_divide_project.f1.k1_is_0";
        return my_error_uri;
    }
    static const char * const description()
    {
        static const char * const my_description = "Divide by zero error";
        return my_description;
    }

    or_k1_is_zero( RichException * p_prev = 0 )
        :
        or_default_f1_k_is_zero( error_uri(), description(), p_prev )
    {}
};

struct or_k2_is_zero : public or_default_f1_k_is_zero
{
    static const char * const error_uri()
    {
        static const char * const my_error_uri = "com.codalogic.show_rework_of_safe_divide_project.f1.k2_is_0";
        return my_error_uri;
    }
    static const char * const description()
    {
        static const char * const my_description = "Divide by zero error";
        return my_description;
    }

    or_k2_is_zero( RichException * p_prev = 0 )
        :
        or_default_f1_k_is_zero( error_uri(), description(), p_prev )
    {}
};

void show_rework_of_safe_divide_project()
{
    Suite( "show_rework_of_safe_divide_project()" );

    try
    {
        double i=1.0, j=1.0, k1=1.0, k2=0.0;
        double r = f1<or_k1_is_zero>( i, j, k1 ) +
                    f1<or_k2_is_zero>( i, j, 0.0 );
        Bad( "exception should not throw" );
    }
    catch( or_k1_is_zero & )
    {
        Bad( "or_k1_is_zero exception should not throw" );
    }
    catch( or_k2_is_zero & e )
    {
        Good( "or_k2_is_zero exception thrown" );
        Verify( e.to_string() == "com.codalogic.show_rework_of_safe_divide_project.f1.k2_is_0: Divide by zero error\n"
                                 "  com.codalogic.f1.safe_divide.k_is_0: Divide by zero error\n",
                        "Is or_k2_is_zero exception to_string() correct?" );

    }
}

int main( int argc, char * argv[] )
{
    show_single_exception_class();

    show_throw_2();

    show_params_storage();

    show_exception_with_params();

    show_throw_2_with_derived_exceptions();

    show_has_and_get_parameter_access();

    show_rework_of_safe_divide_project();

    report();

    return 0;
}
