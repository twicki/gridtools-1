/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <common/boollist.hpp>
#include <communication/GCL.hpp>
#include <communication/high_level/descriptors.hpp>
#include <communication/high_level/descriptors_dt.hpp>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <sstream>
#include <string>

#define DIM 10

struct triple_t {
    int x, y, z;
    triple_t(int a, int b, int c) : x(a), y(b), z(c) {}
    triple_t() : x(-1), y(-1), z(-1) {}
};

std::ostream &operator<<(std::ostream &s, triple_t const &t) {
    return s << " (" << t.x << ", " << t.y << ", " << t.z << ") ";
}

bool operator==(triple_t const &a, triple_t const &b) { return (a.x == b.x && a.y == b.y && a.z == b.z); }

bool operator!=(triple_t const &a, triple_t const &b) { return !(a == b); }

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    gridtools::GCL_Init(argc, argv);

    triple_t *a = new triple_t[DIM * DIM * DIM];
    triple_t *b = new triple_t[DIM * DIM * DIM];
    triple_t *c = new triple_t[DIM * DIM * DIM];

    int pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    std::cout << pid << " " << nprocs << "\n";

    std::stringstream ss;
    ss << pid;

    std::string filename = "out" + ss.str() + ".txt";
    // filename[3] = '0'+pid;
    std::cout << filename << std::endl;
    std::ofstream file(filename.c_str());

    file << pid << "  " << nprocs << "\n";

    MPI_Comm CartComm;
    int dims[3] = {0, 0, 0};
    MPI_Dims_create(nprocs, 3, dims);
    int period[3] = {0, 0, 0};

    file << "@" << pid << "@ MPI GRID SIZE " << dims[0] << " - " << dims[1] << " - " << dims[2] << "\n";

    MPI_Cart_create(MPI_COMM_WORLD, 3, dims, period, false, &CartComm);

    typedef gridtools::MPI_3D_process_grid_t<3> grid_type;
    gridtools::array<int, 3> dimensions;
    dimensions[0] = dims[0];
    dimensions[1] = dims[1];
    dimensions[2] = dims[2];

    gridtools::hndlr_dynamic_ut<triple_t, grid_type, gridtools::Halo_Exchange_3D<grid_type>> hd(
        gridtools::boollist<3>(false, false, false), CartComm, &dimensions);

    hd.halo.add_halo(2, 2, 1, 3, 6, DIM);
    hd.halo.add_halo(1, 2, 1, 3, 6, DIM);
    hd.halo.add_halo(0, 3, 2, 4, 6, DIM);

    hd.allocate_buffers(3);

    int pi, pj, pk;
    hd.pattern().proc_grid().coords(pk, pj, pi);
    int PI, PJ, PK;
    hd.pattern().proc_grid().dims(PK, PJ, PI);

    file << "COORDINATES " << pi << ", " << pj << ", " << pk << std::endl;

    for (int ii = 3; ii <= 6; ++ii)
        for (int jj = 3; jj <= 6; ++jj)
            for (int kk = 4; kk <= 6; ++kk) {
                a[gridtools::access(kk, jj, ii, DIM, DIM, DIM)] =
                    triple_t(ii - 3 + 4 * pi, jj - 3 + 4 * pj, kk - 4 + 3 * pk);
            }

    for (int ii = 3; ii <= 6; ++ii)
        for (int jj = 3; jj <= 6; ++jj)
            for (int kk = 4; kk <= 6; ++kk) {
                b[gridtools::access(kk, jj, ii, DIM, DIM, DIM)] =
                    triple_t(ii - 3 + 4 * pi, jj - 3 + 4 * pj, kk - 4 + 3 * pk);
            }

    for (int ii = 3; ii <= 6; ++ii)
        for (int jj = 3; jj <= 6; ++jj)
            for (int kk = 4; kk <= 6; ++kk) {
                c[gridtools::access(kk, jj, ii, DIM, DIM, DIM)] =
                    triple_t(ii - 3 + 4 * pi, jj - 3 + 4 * pj, kk - 4 + 3 * pk);
            }

    hd.pack(a, b, c);

    hd.exchange();

    hd.unpack(a, b, c);

    // CHECK!
    bool err = false;
    for (int ii = 3 - ((pi > 0) ? 2 : 0); ii <= 6 + ((pi < PI - 1) ? 1 : 0); ++ii)
        for (int jj = 3 - ((pj > 0) ? 2 : 0); jj <= 6 + ((pj < PJ - 1) ? 1 : 0); ++jj)
            for (int kk = 4 - ((pk > 0) ? 3 : 0); kk <= 6 + ((pk < PK - 1) ? 2 : 0); ++kk) {
                if (a[gridtools::access(kk, jj, ii, DIM, DIM, DIM)] !=
                    triple_t(ii - 3 + 4 * pi, jj - 3 + 4 * pj, kk - 4 + 3 * pk)) {
                    err = true;
                    file << " A " << ii << ", " << jj << ", " << kk << ", "
                         << a[gridtools::access(kk, jj, ii, DIM, DIM, DIM)]
                         << " != " << triple_t(ii - 3 + 4 * pi, jj - 3 + 4 * pj, kk - 3 + 4 * pk) << "\n";
                }
            }

    for (int ii = 3 - ((pi > 0) ? 2 : 0); ii <= 6 + ((pi < PI - 1) ? 1 : 0); ++ii)
        for (int jj = 3 - ((pj > 0) ? 2 : 0); jj <= 6 + ((pj < PJ - 1) ? 1 : 0); ++jj)
            for (int kk = 4 - ((pk > 0) ? 3 : 0); kk <= 6 + ((pk < PK - 1) ? 2 : 0); ++kk) {
                if (b[gridtools::access(kk, jj, ii, DIM, DIM, DIM)] !=
                    triple_t(ii - 3 + 4 * pi, jj - 3 + 4 * pj, kk - 4 + 3 * pk)) {
                    err = true;
                    file << " B " << ii << ", " << jj << ", " << kk << ", "
                         << b[gridtools::access(kk, jj, ii, DIM, DIM, DIM)]
                         << " != " << triple_t(ii - 3 + 4 * pi, jj - 3 + 4 * pj, kk - 4 + 3 * pk) << "\n";
                }
            }

    for (int ii = 3 - ((pi > 0) ? 2 : 0); ii <= 6 + ((pi < PI - 1) ? 1 : 0); ++ii)
        for (int jj = 3 - ((pj > 0) ? 2 : 0); jj <= 6 + ((pj < PJ - 1) ? 1 : 0); ++jj)
            for (int kk = 4 - ((pk > 0) ? 3 : 0); kk <= 6 + ((pk < PK - 1) ? 2 : 0); ++kk) {
                if (c[gridtools::access(kk, jj, ii, DIM, DIM, DIM)] !=
                    triple_t(ii - 3 + 4 * pi, jj - 3 + 4 * pj, kk - 4 + 3 * pk)) {
                    err = true;
                    file << " C " << ii << ", " << jj << ", " << kk << ", "
                         << c[gridtools::access(kk, jj, ii, DIM, DIM, DIM)]
                         << " != " << triple_t(ii + 7 * pi, jj - 3 + 4 * pj, kk + DIM * pk) << "\n";
                }
            }

    std::cout << std::boolalpha << err << " (False is good)" << std::endl;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
