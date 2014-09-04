/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#include "../measure.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <boost/spirit/include/qi.hpp>
#include "double-conversion.h"

namespace
{   
    int const ndigits = 13; 
    std::string numbers[ndigits] =
    {
        "1234",
        "-1.2e3",
        "0.1",
        "-1.2e-3",
        "-.2e3",
        "-2e6",
        "1.2345e5",
        "-5.7222349715140557e+307",
        "2.0332938517515416e-308",

        // constants from clang 3.6 on x86_64
        "1.7976931348623157e+308", // DBL_MAX
        "2.2204460492503131e-16",  // DBL_EPSILON
        "2.2250738585072014e-308", // DBL_MIN
        "4.9406564584124654e-324", // DBL_DENORM_MIN
    };

    char const* first[ndigits];
    char const* last[ndigits];

    ///////////////////////////////////////////////////////////////////////////
    struct atof_test : test::base
    {
        void benchmark()
        {
            for (int i = 0; i < ndigits; ++i) 
            {
                double d = atof(first[i]);
                this->val += *reinterpret_cast<int*>(&d);
            }  
        }
    };
    
    ///////////////////////////////////////////////////////////////////////////
    struct strtod_test : test::base
    {        
        void benchmark()
        {
            for (int i = 0; i < ndigits; ++i) 
            {
                char *e;
                double d = strtod(first[i], &e);
                if (e != last[i]) {
                    std::cerr << "strtod failed on " << first[i] << " at " << e;
                    exit(1);
                }
                this->val += *reinterpret_cast<int*>(&d);
            }
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct double_conv_test : test::base
    {
        static double parse(char const* first, char const* last) {
            using ::double_conversion::StringToDoubleConverter;
            static const StringToDoubleConverter conv(
                StringToDoubleConverter::ALLOW_TRAILING_JUNK |
                  StringToDoubleConverter::ALLOW_HEX |
                  StringToDoubleConverter::ALLOW_OCTALS,
                0, 0,
                "Inf", "NaN");
            int used;
            int len = last - first;
            double d = conv.StringToDouble(first, len, &used);
            if (used != len) {
                std::cerr << "double-conversion failed on " << first << " at " << first + used;
                exit(1);
            }
            return d;
        }

        void benchmark()
        {
            for (int i = 0; i < ndigits; ++i) 
            {
                double d = parse(first[i], last[i]);
                this->val += *reinterpret_cast<int*>(&d);
            }
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct spirit_double_test : test::base
    {
        static double parse(char const* first, char const* last)
        {
            double n;
            namespace qi = boost::spirit::qi;
            using qi::double_;
            qi::parse(first, last, double_, n);
            return n;
        }

        void benchmark()
        {
            for (int i = 0; i < ndigits; ++i) 
            {
                double d = parse(first[i], last[i]);
                this->val += *reinterpret_cast<int*>(&d);
            }
        }
    };
}

int main()
{   
    std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
    std::cout << "Numbers to test:" << std::endl;
    for (int i = 0; i < ndigits; ++i)
    {
        first[i] = numbers[i].c_str();
        last[i] = strchr(first[i], '\0');
        std::cout << numbers[i] << std::endl;
    }
    std::cout.precision(17);
    std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;
    std::cout << "atof/strtod/qi.double results:" << std::endl;
    for (int i = 0; i < ndigits; ++i)
    {
        std::cout 
            << atof(first[i]) << ','
            << strtod(first[i], const_cast<char**>(&last[i])) << ','
            << double_conv_test::parse(first[i], last[i]) << ','
            << spirit_double_test::parse(first[i], last[i]) << ','
            << std::endl;
    }
    std::cout << "///////////////////////////////////////////////////////////////////////////" << std::endl;

    BOOST_SPIRIT_TEST_BENCHMARK(
        10000000,     // This is the maximum repetitions to execute
        (atof_test)
        (strtod_test)
        (double_conv_test)
        (spirit_double_test)
    )
    
    // This is ultimately responsible for preventing all the test code
    // from being optimized away.  Change this to return 0 and you
    // unplug the whole test's life support system.
    return test::live_code != 0;
}

