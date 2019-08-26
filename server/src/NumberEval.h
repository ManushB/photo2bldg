/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A calculator example demonstrating the grammar and semantic actions
//  using phoenix to do the actual expression evaluation. The parser is
//  essentially an "interpreter" that evaluates expressions on the fly.
//
//  [ JDG June 29, 2002 ]   spirit1
//  [ JDG March 5, 2007 ]   spirit2
//
//  [ Gen Nishida September 19, 2015 ]   support variables
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include <string>

namespace myeval {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

	static qi::symbols<char, float> variables;

    ///////////////////////////////////////////////////////////////////////////
    //  Our calculator grammar
    ///////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct calculator : qi::grammar<Iterator, float(), ascii::space_type>
    {
        calculator() : calculator::base_type(expression)
        {
            using qi::_val;
            using qi::_1;
            using qi::uint_;

            expression =
                term                            [_val = _1]
                >> *(   ('+' >> term            [_val += _1])
                    |   ('-' >> term            [_val -= _1])
                    )
                ;

            term =
                factor                          [_val = _1]
                >> *(   ('*' >> factor          [_val *= _1])
                    |   ('/' >> factor          [_val /= _1])
                    )
                ;

            factor =
				( qi::float_ | variables )      [_val = _1]
                |   '(' >> expression           [_val = _1] >> ')'
                |   ('-' >> factor              [_val = -_1])
                |   ('+' >> factor              [_val = _1])
                ;
        }

        qi::rule<Iterator, float(), ascii::space_type> expression, term, factor;
    };
};
