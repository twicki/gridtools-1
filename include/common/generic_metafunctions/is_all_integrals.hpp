/*
  GridTools Libraries

  Copyright (c) 2017, ETH Zurich and MeteoSwiss
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  For information: http://eth-cscs.github.io/gridtools/
*/
#pragma once
#include "is_all.hpp"

namespace gridtools {
    /**
    * @brief SFINAE for the case in which all the components of a parameter pack are of integral type
    */
    template < typename... IntTypes >
    using all_integral =
#if defined(CUDA8) && !defined(_CRAYC)
        all_< boost::is_integral, IntTypes... >;
#else
        typename boost::enable_if_c< accumulate(logical_and(), true, boost::is_integral< IntTypes >::type::value...),
            bool >::type;
#endif

    /**
    * @brief SFINAE for the case in which all the components of a parameter pack are of static integral type
    */
    template < typename... IntTypes >
    using all_static_integral = all_< is_static_integral, IntTypes... >;

    /* check if all given types are integral types */
    template < typename... IntTypes >
    using is_all_integral =
#if defined(CUDA8) && !defined(_CRAYC)
        is_all< boost::is_integral, IntTypes... >;
#else
        boost::mpl::bool_< accumulate(logical_and(), true, boost::is_integral< IntTypes >::type::value...) >;
#endif

    /* check if all given types are integral types */
    template < typename... IntTypes >
    using is_all_static_integral = is_all< is_static_integral, IntTypes... >;

    /* check if all given types are integral types */
    template < typename T >
    struct is_integral_or_enum : boost::mpl::or_< boost::is_integral< T >, boost::is_enum< T > > {};

    template < typename... IntTypes >
    using is_all_integral_or_enum =
#if defined(CUDA8) && !defined(_CRAYC)
        is_all< is_integral_or_enum, IntTypes... >;
#else
        boost::mpl::bool_< accumulate(logical_and(), true, is_integral_or_enum< IntTypes >::type::value...) >;
#endif
}