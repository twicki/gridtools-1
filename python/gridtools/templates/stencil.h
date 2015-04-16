/**
 * This code was automatically generated by gridtools4py:
 * the Python interface to the Gridtools library
 *
 */
#pragma once

#include <gridtools.h>
#include <stencil-composition/backend.h>
#include <boost/fusion/include/make_vector.hpp>
#include "{{ fun_hdr_file }}"



#ifdef BACKEND_BLOCK
#define BACKEND backend<Host, Block >
#else
#define BACKEND backend<Host, Naive >
#endif



using gridtools::level;
using gridtools::arg_type;
using gridtools::range;
using gridtools::arg;

using namespace gridtools;
using namespace enumtype;


namespace {{ namespace }}
{
//
// definition of the special regions in the vertical (k) direction
//
typedef gridtools::interval<level<0,-1>, level<1,-1> > x_interval;
typedef gridtools::interval<level<0,-2>, level<1,1> > axis;



bool test (uint_t d1, uint_t d2, uint_t d3,
           {%- for arg in params %}
           float_type *{{ arg.name }}_buff
               {%- if not loop.last -%}
               ,
               {%- endif -%}
           {% endfor -%})
{
    //
    // Fortran-like memory layout
    //
    //typedef gridtools::layout_map<2,1,0> layout_t;

    //
    // C-like memory layout
    //
    typedef gridtools::layout_map<0,1,2> layout_t;

    //
    // define the storage unit used by the backend
    //
    typedef gridtools::BACKEND::storage_type<float_type, layout_t >::type storage_type;

    {% if temps %}
    //
    // define a special data type for the temporary, i.e., intermediate buffers
    //
    typedef gridtools::BACKEND::temporary_storage_type<float_type, layout_t >::type tmp_storage_type;
    {% endif -%}

    {% if params %}
    //
    // parameter data fields use the memory buffers received from NumPy arrays
    // 
    {% for p in params -%}
    storage_type {{ p.name }} (d1, d2, d3,
                         (float_type *) {{ p.name }}_buff,
                          "{{ p.name }}");
    {% endfor %}
    {% endif -%}

    //
    // place-holder definition: their order matches the stencil parameters,
    // especially the non-temporary ones, during the construction of the domain
    //
    {% for p in params_temps -%}
    typedef arg<{{ loop.index0 }}, 
        {%- if scope.is_temporary (p.name) -%}
            tmp_storage_type>
        {%- else -%}
            storage_type>
        {%- endif %} p_{{ p.name|replace('.', '_') }};
    {% endfor %}

    //
    // an array of placeholders to be passed to the domain
    //
    typedef boost::mpl::vector<
        {{- params_temps|join_with_prefix ('p_', attribute='name')|join (', ')|replace('.', '_') }}> arg_type_list;

    //
    // construction of the domain.
    // The domain is the physical domain of the problem, with all the physical
    // fields that are used, temporary and not.
    // It must be noted that the only fields to be passed to the constructor
    // are the non-temporary. The order in which they have to be passed is the
    // order in which they appear scanning the placeholders in order.
    // (I don't particularly like this)
    //
    gridtools::domain_type<arg_type_list> domain (boost::fusion::make_vector (
        {{- params|join_with_prefix('&', attribute='name')|join(', ') }}));

    {% for s in stencils %}
    //
    // definition of the physical dimensions of the problem.
    // The constructor takes the horizontal plane dimensions, i.e.:
    //
    // { halo in negative direction,
    //   halo in positive direction,
    //   index of the first interior element,
    //   index of the last interior element,
    //   total number of elements in dimension }
    //
    uint_t di[5] = { {{ s.halo[0] }},
                     {{ s.halo[1] }},
                     {{ s.halo[1] }},
                     d1-{{ s.halo[0] }}-1,
                     d1 };
    uint_t dj[5] = { {{ s.halo[2] }},
                     {{ s.halo[3] }},
                     {{ s.halo[3] }},
                     d2-{{ s.halo[2] }}-1,
                     d2 };

    //
    // the vertical dimension of the problem is a property of this object
    //
    gridtools::coordinates<axis> coords(di, dj);
    coords.value_list[0] = 0;
    coords.value_list[1] = d3-1;

    //
    // Here we do a lot of stuff
    //
    // 1) we pass to the intermediate representation ::run function the
    // description of the stencil, which is a multi-stage stencil (mss);
    // 2) the logical physical domain with the fields to use;
    // 3) the actual domain dimensions
    //
    boost::shared_ptr<gridtools::computation> comp_{{ s.name|lower }} =
      gridtools::make_computation<gridtools::BACKEND, layout_t>
        (
            gridtools::make_mss
            (
                execute<{{ s.k_direction }}>(),
                {% for f in s.inspector.functors -%}
                gridtools::make_esf<{{ f.name }}>(
                   {{- f.scope.get_parameters ( )|join_with_prefix ('p_', attribute='name')|join ('(), ')|replace('.', '_') }}())
                   {%- if not loop.last -%}
                   ,
                   {%- endif %}
                {% endfor -%}
                ),
            domain, coords
            );
    {% endfor %}

    //
    // preparation ...
    //
    {% for s in stencils %}
    comp_{{ s.name|lower }}->ready();
    {% endfor %}

    {% for s in stencils %}
    comp_{{ s.name|lower }}->steady();
    {% endfor %}

    //
    // ... and execution
    //
    {% for s in stencils %}
    comp_{{ s.name|lower }}->run();
    {% endfor %}

    //
    // clean everything up
    //
    {% for s in stencils %}
    comp_{{ s.name|lower }}->finalize();
    {% endfor %}

    return EXIT_SUCCESS;
}

} // namespace {{ namespace }}
