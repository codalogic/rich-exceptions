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

#ifndef NESTED_EXCEPTION
#define NESTED_EXCEPTION

#include <exception>
#include <string>
#include <vector>
#include <list>
#include <ostream>
#include <sstream>

namespace nest_excep {

struct NestedExceptionParameter
{
    const char * name;
    std::string value;

    NestedExceptionParameter(
            const char * const name_in,
            std::string value_in )
        :
        name( name_in ),
        value( value_in )
    {}
    friend std::ostream & operator << ( std::ostream & os, const NestedExceptionParameter & r_params )
    {
        os << r_params.name << ": " << r_params.value;
        return os;
    }
};

class NestedExceptionParams
{
private:
    typedef std::vector< NestedExceptionParameter > params_t;
    params_t params;

public:
    NestedExceptionParams() {}
    NestedExceptionParams(
            const char * const name_in,
            const std::string & value_in )
    {
        add( name_in, value_in );
    }
    template< typename T >
    NestedExceptionParams(
            const char * const name_in,
            const T & value_in )
    {
        add( name_in, value_in );
    }

    void add(
            const char * const name_in,
            const std::string & value_in )
    {
        params.push_back( NestedExceptionParameter( name_in, value_in ) );
    }
    template< typename T >
    void add(
            const char * const name_in,
            const T & value_in )
    {
        std::stringstream ss;
        ss << value_in;
        params.push_back( NestedExceptionParameter( name_in, ss.str() ) );
    }

    bool empty() const { return params.empty(); }
    size_t size() const { return params.size(); }
    const NestedExceptionParameter & operator []( size_t i ) const { return params[i]; }
    NestedExceptionParameter & operator []( size_t i ) { return params[i]; }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    friend std::ostream & operator << ( std::ostream & os, const NestedExceptionParams & r_params )
    {
        bool is_first = true;
        for( params_t::const_iterator i( r_params.params.begin() ),
                    i_end( r_params.params.end() );
                i != i_end;
                ++i )
        {
            if( ! is_first )
                os << ", ";
            os << *i;
        }
        return os;
    }
};

struct NestedExceptionNode
{
    const char * const error_uri;   // of the form "com.codalogic.mymodule.myerror" or ".mymodule.myerror"
    NestedExceptionParams error_params;
    const char * const description; // Human readable description

    NestedExceptionNode(
            const char * const error_uri_in,
            const char * const description_in )
        :
        error_uri( error_uri_in ),
        description( description_in )
    {
    }
    NestedExceptionNode(
            const char * const error_uri_in,
            NestedExceptionParams & error_params_in,
            const char * const description_in )
        :
        error_uri( error_uri_in ),
        error_params( error_params_in ),
        description( description_in )
    {
    }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    friend std::ostream & operator << ( std::ostream & os, const NestedExceptionNode & r_node )
    {
        os << r_node.error_uri;
        if( ! r_node.error_params.empty() )
            os << " (" << r_node.error_params << ")";
        os << ": " << r_node.description;
        return os;
    }
};

class NestedException : public std::exception
{
private:
    typedef std::list< NestedExceptionNode > nodes_t;
    mutable nodes_t nodes;

public:
    typedef nodes_t::const_iterator const_iterator;
    typedef nodes_t::const_reverse_iterator const_reverse_iterator;
    typedef nodes_t::const_reference const_reference;

    NestedException(
            const char * const error_uri_in,
            const char * const description_in )
    {
        nodes.push_front( NestedExceptionNode( error_uri_in, description_in ) );
    }
    NestedException(
            const char * const error_uri_in,
            NestedExceptionParams & error_params_in,
            const char * const description_in )
    {
        nodes.push_front( NestedExceptionNode( error_uri_in, error_params_in, description_in ) );
    }
    NestedException(
            const char * const error_uri_in,
            const char * const description_in,
            NestedException & r_prev_nested_exception )
    {
        nodes.swap( r_prev_nested_exception.nodes );
        nodes.push_front( NestedExceptionNode( error_uri_in, description_in ) );
    }
    NestedException(
            const char * const error_uri_in,
            NestedExceptionParams & error_params_in,
            const char * const description_in,
            NestedException & r_prev_nested_exception )
    {
        nodes.swap( r_prev_nested_exception.nodes );
        nodes.push_front( NestedExceptionNode( error_uri_in, error_params_in, description_in ) );
    }
    virtual ~NestedException() {}

    virtual const char * what() const
    {
        if( ! nodes.empty() )
            return nodes.front().description;
        return "<Undescribed NestedException>";
    }

    bool empty() const { return nodes.empty(); }
    size_t size() const { return nodes.size(); }

    const_reference front() const { return nodes.front(); }
    const_iterator begin() const { return nodes.begin(); }
    const_iterator end() const { return nodes.end(); }
    const_reverse_iterator rbegin() const { return nodes.rbegin(); }
    const_reverse_iterator rend() const { return nodes.rend(); }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    friend std::ostream & operator << ( std::ostream & os, const NestedException & r_exception )
    {
        size_t indent = 0;
        for( NestedException::const_iterator i( r_exception.begin() ), i_end( r_exception.end() );
                i != i_end;
                ++i, indent += 2 )
            os << std::string( indent, ' ' ) << *i << "\n";
        return os;
    }
};

}   // Namespace namespace nest_excep

#endif  // NESTED_EXCEPTION
