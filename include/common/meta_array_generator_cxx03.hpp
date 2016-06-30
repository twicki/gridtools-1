/*
   Copyright 2016 GridTools Consortium

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#pragma once

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>

namespace gridtools {

#define _META_ARRAY_GENERATOR_(z, n, nil)                                                                             \
    template < typename Vector, typename First, BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename MssType) >             \
    struct BOOST_PP_CAT(meta_array_generator, BOOST_PP_INC(BOOST_PP_INC(n))) {                                        \
        typedef typename BOOST_PP_CAT(                                                                                \
            meta_array_generator, BOOST_PP_INC(n))< typename boost::mpl::push_back< Vector, First >::type,            \
            BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), MssType) >::type type;                                              \
    };                                                                                                                \
                                                                                                                      \
    template < typename Vector,                                                                                       \
        typename Mss1,                                                                                                \
        typename Mss2,                                                                                                \
        typename Cond,                                                                                                \
        BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename MssType) >                                                     \
    struct BOOST_PP_CAT(meta_array_generator,                                                                         \
        BOOST_PP_INC(BOOST_PP_INC(                                                                                    \
            n)))< Vector, condition< Mss1, Mss2, Cond >, BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), MssType) > {           \
        typedef condition< typename BOOST_PP_CAT(meta_array_generator, BOOST_PP_INC(BOOST_PP_INC(n))) < Vector,       \
            Mss1,                                                                                                     \
            BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), MssType) >::type,                                                   \
            typename BOOST_PP_CAT(meta_array_generator,                                                               \
                BOOST_PP_INC(BOOST_PP_INC(n)))< Vector, Mss2, BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), MssType) >::type, \
            Cond > type;                                                                                              \
    };

    template < typename Mss1, typename Mss2, typename Cond >
    struct condition;

    template < typename Vector >
    struct meta_array_generator0 {
        typedef Vector type;
    };

    template < typename Vector, typename Mss1 >
    struct meta_array_generator1 {
        typedef typename boost::mpl::push_back< Vector, Mss1 >::type type;
    };

    template < typename Vector, typename Mss1, typename Mss2, typename Cond >
    struct meta_array_generator1< Vector, condition< Mss1, Mss2, Cond > > {
        typedef condition< typename meta_array_generator1< Vector, Mss1 >::type,
            typename meta_array_generator1< Vector, Mss2 >::type,
            Cond > type;
    };

    BOOST_PP_REPEAT(GT_MAX_MSS, _META_ARRAY_GENERATOR_, _)

} // namespace gridtools