#include "gtest/gtest.h"
#include "Options.hpp"
#include "advection_pdbott_prepare_tracers.hpp"

int main(int argc, char **argv) {

    // Pass command line arguments to googltest
    ::testing::InitGoogleTest(&argc, argv);

    if (argc < 4) {
        printf("Usage: advection_pdbott_prepare_tracers_<whatever> dimx dimy dimz\n where args are integer sizes of "
               "the data fields\n");
        return 1;
    }

    for (int i = 0; i != 3; ++i) {
        Options::getInstance().m_size[i] = atoi(argv[i + 1]);
    }

    return RUN_ALL_TESTS();
}

TEST(vertical_diffusion, test) {
    gridtools::uint_t x = Options::getInstance().m_size[0];
    gridtools::uint_t y = Options::getInstance().m_size[1];
    gridtools::uint_t z = Options::getInstance().m_size[2];

    ASSERT_TRUE(adv_prepare_tracers::test(x, y, z));
}
