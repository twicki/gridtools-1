/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#pragma once

#ifndef __CUDACC__
#error This is CUDA only header
#endif

#include "../meta/dedup.hpp"
#include "../meta/list.hpp"
#include "../meta/macros.hpp"
#include "../meta/st_contains.hpp"
#include "../meta/type_traits.hpp"

namespace gridtools {
    namespace impl_ {
        using texture_types = GT_META_CALL(meta::dedup,
            (meta::list<char,
                short,
                int,
                long long,
                unsigned char,
                unsigned short,
                unsigned int,
                unsigned long long,
                int2,
                int4,
                uint2,
                uint4,
                float,
                float2,
                float4,
                double,
                double2>));
    } // namespace impl_

    template <class T>
    GT_META_DEFINE_ALIAS(is_texture_type, meta::st_contains, (impl_::texture_types, T));

} // namespace gridtools
