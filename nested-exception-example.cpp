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

#include "nested-exception.h"

#include <string>
#include <iostream>

#include "annotate-lite.h"

using namespace nest_excep;

void test_single_exception_class()
{
    NestedException nested_exception( "com.codalogic.nexp.test1", "First exception test" );

    VerifyCritical( ! nested_exception.empty(), "Is exception non-empty?" );

    VerifyCritical( nested_exception.size() == 1, "Is exception size correct?" );

    Verify( strcmp( nested_exception.main_error_uri(), "com.codalogic.nexp.test1" ) == 0, "Is 'main_error_uri()' OK?" );

    Verify( strcmp( nested_exception.what(), "First exception test" ) == 0, "Is 'what()' description OK?" );

    std::exception & r_std_exception( nested_exception );

    Verify( strcmp( r_std_exception.what(), "First exception test" ) == 0, "Is 'what()' accessible via std::exception base?" );

    Verify( nested_exception.to_string() == "com.codalogic.nexp.test1: First exception test\n", "Is nested_exception.to_string() correct?" );
}

void throw_2_first()
{
    throw NestedException( "com.codalogic.nexp.test_2_first", "First exception of 2 test" );
}

void throw_2_second()
{
    try
    {
        throw_2_first();
    }
    catch( NestedException & e )
    {
        throw NestedException( "com.codalogic.nexp.test_2_second", "Second exception of 2 test", e );
    }
}

void test_throw_2()
{
    try
    {
        throw_2_second();
    }
    catch( NestedException & e )
    {
        VerifyCritical( ! e.empty(), "Is throw_2 exception non-empty?" );

        VerifyCritical( e.size() == 2, "Is throw_2 exception size correct?" );

        Verify( strcmp( e.main_error_uri(), "com.codalogic.nexp.test_2_second" ) == 0, "Is throw_2 'main_error_uri()' OK?" );

        Verify( strcmp( e.what(), "Second exception of 2 test" ) == 0, "Is throw_2 'what()' description OK?" );

        std::exception & r_std_exception( e );

        Verify( strcmp( e.what(), "Second exception of 2 test" ) == 0, "Is throw_2 'what()' accessible via std::exception base?" );

        Verify( e.to_string() == "com.codalogic.nexp.test_2_second: Second exception of 2 test\n"
                                    "  com.codalogic.nexp.test_2_first: First exception of 2 test\n",
                                "Is throw_2 nested_exception.to_string() correct?" );
    }
}

int main( int argc, char * argv[] )
{
    test_single_exception_class();

    test_throw_2();

    report();

    return 0;
}
