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
#define PEDANTIC_DISABLED

#include "gtest/gtest.h"
#include <stencil-composition/stencil-composition.hpp>

using namespace gridtools;
using namespace enumtype;

typedef interval<level<0,-1>, level<1,-1> > x_interval;
typedef interval<level<0,-2>, level<1,1> > axis;
#ifdef __CUDACC__
typedef backend< Cuda, structured, Block > backend_t;
#ifdef CXX11_ENABLED
typedef backend_t::storage_info< 0, layout_map< 0, 1, 2 > > meta_t;
#else
typedef meta_storage< meta_storage_aligned< meta_storage_base< 0U, layout_map< 0, 1, 2 >, false, int, int >,
    aligned< 32 >,
    halo< 0, 0, 0 > > > meta_t;
#endif
#else
typedef backend< Host, structured, Naive > backend_t;
typedef backend_t::storage_info< 0, layout_map< 0, 1, 2 > > meta_t;
#endif
typedef backend_t::storage_type< float_type, meta_t >::type storage_type;

struct boundary {

    int int_value;

    boundary(int ival) : int_value(ival) {}

    GT_FUNCTION
    double value() const {return 10.;}
};

struct functor{
    typedef accessor<0, enumtype::inout, extent<0,0,0,0> > sol;
    typedef global_accessor< 1, enumtype::inout > bd;

    typedef boost::mpl::vector< sol, bd > arg_list;

    template <typename Evaluation>
    GT_FUNCTION
    static void Do(Evaluation const & eval, x_interval) {
        eval(sol()) += eval(bd()).value() + eval(bd()).int_value;
    }
};

TEST(test_global_accessor, boundary_conditions) {
    meta_t meta_(10,10,10);
    storage_type sol_(meta_, (float_type)0.);

    sol_.initialize(2.);

    boundary bd(20);
#ifdef CXX11_ENABLED
    auto bd_ = make_global_parameter(bd);
    typedef arg< 1, decltype(bd_) > p_bd;
    GRIDTOOLS_STATIC_ASSERT(gridtools::is_global_parameter< decltype(bd_) >::value, "is_global_parameter check failed");
#else
    global_parameter< boundary > bd_(bd);
    typedef arg< 1, global_parameter< boundary > > p_bd;
    GRIDTOOLS_STATIC_ASSERT(
        gridtools::is_global_parameter< global_parameter< boundary > >::value, "is_global_parameter check failed");
#endif
    GRIDTOOLS_STATIC_ASSERT(!gridtools::is_global_parameter< storage_type >::value, "is_global_parameter check failed");

    halo_descriptor di=halo_descriptor(0,1,1,9,10);
    halo_descriptor dj=halo_descriptor(0,1,1,1,2);
    grid<axis> coords_bc(di, dj);
    coords_bc.value_list[0] = 0;
    coords_bc.value_list[1] = 1;

    typedef arg<0, storage_type> p_sol;

    aggregator_type<boost::mpl::vector<p_sol, p_bd> > domain ( boost::fusion::make_vector( &sol_, &bd_));

/*****RUN 1 WITH bd int_value set to 20****/
#ifdef CXX11_ENABLED
    auto
#else
#ifdef __CUDACC__
    stencil*
#else
        boost::shared_ptr<stencil>
#endif
#endif
        bc_eval = make_computation< backend_t >
        (
            domain, coords_bc
            , make_multistage
            (
                execute<forward>(),
                make_stage<functor>(p_sol(), p_bd()))
            );

    bc_eval->ready();
    bc_eval->steady();
    bc_eval->run();
    // fetch data and check
    sol_.d2h_update();
    bool result = true;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            for (int k = 0; k < 10; ++k) {
                double value = 2.;
                if (i > 0 && j == 1 && k < 2) {
                    value += 10.;
                    value += 20;
                }
                if (sol_(i, j, k) != value) {
                    result = false;
                }
            }

// get the configuration object from the gpu
// modify configuration object (boundary)
#ifdef __CUDACC__
    bd_.d2h_update();
#endif
    bd.int_value = 30;
#ifdef __CUDACC__
    bd_.h2d_update();
#else
    bd_.update_data();
#endif

    // get the storage object from the gpu
    // modify storage object
    sol_.initialize(2.);
#ifdef __CUDACC__
    sol_.h2d_update();
#endif

    // run again and finalize
    bc_eval->run();
    bc_eval->finalize();

    // check result of second run
    for (int i=0; i<10; ++i)
        for (int j=0; j<10; ++j)
            for (int k=0; k<10; ++k)
            {
                double value=2.;
                if (i > 0 && j == 1 && k < 2) {
                    value += 10.;
                    value += 30;
                }
                if(sol_(i,j,k) != value)
                {
                    result=false;
                }
            }

    EXPECT_TRUE(result);
}