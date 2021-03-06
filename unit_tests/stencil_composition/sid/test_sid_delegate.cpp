/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <gridtools/stencil_composition/sid/delegate.hpp>

#include <gtest/gtest.h>

#include <gridtools/common/defs.hpp>
#include <gridtools/common/integral_constant.hpp>
#include <gridtools/common/tuple.hpp>
#include <gridtools/common/tuple_util.hpp>
#include <gridtools/meta.hpp>
#include <gridtools/stencil_composition/sid/concept.hpp>
#include <gridtools/stencil_composition/sid/simple_ptr_holder.hpp>
#include <gridtools/stencil_composition/sid/synthetic.hpp>

namespace gridtools {
    namespace {
        using namespace literals;

        template <class Sid>
        class i_shifted : public sid::delegate<Sid> {
            friend GT_META_CALL(sid::ptr_holder_type, Sid) sid_get_origin(i_shifted &obj) {
                auto &&impl = obj.impl();
                GT_META_CALL(sid::ptr_diff_type, Sid) offset{};
                sid::shift(offset, sid::get_stride<integral_constant<int, 1>>(sid::get_strides(impl)), 1_c);
                return sid::get_origin(impl) + offset;
            }
            using sid::delegate<Sid>::delegate;
        };

        template <class Sid>
        i_shifted<Sid> i_shift(Sid const &sid) {
            return i_shifted<Sid>{sid};
        }

        using sid::property;
        namespace tu = tuple_util;

        TEST(delegate, smoke) {
            double data[3][5];
            auto src = sid::synthetic()
                           .set<property::origin>(sid::host_device::make_simple_ptr_holder(&data[0][0]))
                           .set<property::strides>(tu::make<tuple>(1_c, 5_c));
            auto testee = i_shift(src);

            static_assert(is_sid<decltype(testee)>(), "");

            EXPECT_EQ(&data[0][0], sid::get_origin(src)());
            EXPECT_EQ(&data[1][0], sid::get_origin(testee)());
        }
    } // namespace
} // namespace gridtools
