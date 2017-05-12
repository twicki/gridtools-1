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

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include "stencil-composition/mss_metafunctions.hpp"

namespace gridtools {

/*!
   \fn esf_descriptor<ESF, ...> make_esf(plc1, plc2, plc3, ...)
   \brief Function to create a Elementary Stencil Function
   \param plc{i} placeholder which represents the i-th argument to the functor ESF

   Use this function to associate a stencil functor (stage) to
   arguments (actually, placeholders to arguments)
 */

#define _MAKE_ESF(z, n, nil)                                                                                           \
    template < typename ESF, BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename A) >                                       \
        esf_descriptor< ESF,                                                                                           \
            BOOST_PP_CAT(boost::mpl::vector, BOOST_PP_INC(n)) < BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), A) >>            \
        make_esf(BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), A)) {                                                           \
        return esf_descriptor< ESF,                                                                                    \
                   BOOST_PP_CAT(boost::mpl::vector, BOOST_PP_INC(n)) < BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), A) >> (); \
    }

    BOOST_PP_REPEAT(GT_MAX_ARGS, _MAKE_ESF, _)
#undef _MAKE_ESF

} // namespace gridtools