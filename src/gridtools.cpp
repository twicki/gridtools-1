#ifdef _OPENMP
#include <omp.h>
#endif

namespace gridtools {
    int n_threads() {
#ifdef _OPENMP
        return omp_get_max_threads();
#else
        return 1;
#endif
    }

};
