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

#include "../grid_traits_fwd.hpp"

namespace gridtools {

    /**
     * @struct extract_extents_for_caches
     * metafunction that extracts the extents associated to each cache of the sequence of caches provided by the user.
     * The extent is determined as the enclosing extent of all the extents of esfs that use the cache.
     * It is used in order to allocate enough memory for each cache storage.
     * @tparam IterateDomainArguments iterate domain arguments type containing sequences of caches, esfs and extents
     * @return map<cache,extent>
     */
    template < typename IterateDomainArguments >
    struct extract_extents_for_caches {
        typedef typename IterateDomainArguments::cache_sequence_t cache_sequence_t;
        typedef typename IterateDomainArguments::extent_sizes_t extents_t;
        typedef typename IterateDomainArguments::esf_sequence_t esf_sequence_t;
        typedef typename IterateDomainArguments::backend_ids_t backend_ids_t;

        // insert the extent associated to a Cache into the map of <cache, extent>
        template < typename ExtendsMap, typename Cache >
        struct insert_extent_for_cache {
            GRIDTOOLS_STATIC_ASSERT((is_cache< Cache >::value), "ERROR");

            // update the entry associated to a cache within the map with a new extent.
            // if the key exist we compute and insert the enclosing extent, otherwise we just
            // insert the extent into a new entry of the map of <cache, extent>
            template < typename ExtendsMap_, typename Extend >
            struct update_extent_map {
                GRIDTOOLS_STATIC_ASSERT((is_extent< Extend >::value), "ERROR");

                typedef typename boost::mpl::if_< boost::mpl::has_key< ExtendsMap_, Cache >,
                    typename boost::mpl::at< ExtendsMap_, Cache >::type,
                    typename grid_traits_from_id< backend_ids_t::s_grid_type_id >::null_extent_t >::type
                    default_extent_t;

                typedef typename boost::mpl::insert< typename boost::mpl::erase_key< ExtendsMap_, Cache >::type,
                    boost::mpl::pair< Cache, typename enclosing_extent< default_extent_t, Extend >::type > >::type type;
            };

            // given an Id within the sequence of esf and extents, extract the extent associated an inserted into
            // the map if the cache is used by the esf with that Id.
            template < typename ExtendsMap_, typename EsfIdx >
            struct insert_extent_for_cache_esf {
                GRIDTOOLS_STATIC_ASSERT((boost::mpl::size< extents_t >::value > EsfIdx::value), "ERROR");
                GRIDTOOLS_STATIC_ASSERT((boost::mpl::size< esf_sequence_t >::value > EsfIdx::value), "ERROR");

                typedef typename boost::mpl::at< extents_t, EsfIdx >::type extent_t;
                typedef typename boost::mpl::at< esf_sequence_t, EsfIdx >::type esf_t;

                typedef typename boost::mpl::if_<
                    boost::mpl::contains< typename esf_t::args_t, typename cache_parameter< Cache >::type >,
                    typename update_extent_map< ExtendsMap_, extent_t >::type,
                    ExtendsMap_ >::type type;
            };

            // loop over all esfs and insert the extent associated to the cache into the map
            typedef typename boost::mpl::fold< boost::mpl::range_c< int, 0, boost::mpl::size< esf_sequence_t >::value >,
                ExtendsMap,
                insert_extent_for_cache_esf< boost::mpl::_1, boost::mpl::_2 > >::type type;
        };

        typedef typename boost::mpl::fold< cache_sequence_t,
            boost::mpl::map0<>,
            insert_extent_for_cache< boost::mpl::_1, boost::mpl::_2 > >::type type;
    };

} // namespace gridtools