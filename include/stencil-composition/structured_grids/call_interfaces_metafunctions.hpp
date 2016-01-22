#pragma once

#include <boost/mpl/count_if.hpp>

namespace gridtools {
    namespace _impl {
        /** Metafunction to compute the index of the first accessor in the
            list of accessors to be written.
        */
        template <typename Functor>
        struct _get_index_of_first_non_const {

            template <int I, int L, typename List>
            struct scan_for_index {
                using type = typename boost::mpl::if_
                    <typename is_accessor_readonly<typename boost::mpl::at_c<List, I>::type >::type,
                     typename scan_for_index<I+1, L, List>::type,
                     static_int<I>
                     >::type;
            };

            template <int I, typename List>
            struct scan_for_index<I, I, List> {
                using type = static_int<-1>;
            };

            static const int value = scan_for_index
                <0,
                 boost::mpl::size<typename Functor::arg_list>::value,
                 typename Functor::arg_list>::type::value;
        };

        /** Metafunction to check that there is only one
            written argument in the argument list of a stencil
            operator, so that it is legal to call it as a
            function.

            User protection
        */
        template <typename Functor>
        struct can_be_a_function {

            template <typename CurrentCount, typename CurrentArg>
            struct count_if_written {
                typedef typename boost::mpl::if_
                <typename is_accessor_written<CurrentArg>::type,
                 CurrentCount,
                 static_int<CurrentCount::value+1>
                 >::type type;
            };

            typedef typename boost::mpl::fold
            <typename Functor::arg_list,
             static_int<0>,
             count_if_written<boost::mpl::_1, boost::mpl::_2>
             >::type type;

            static const bool value = type::value==1;
        };


        /** Metafunction to check if a list of types with value traits
            contains a type with a given value.
            This metafunction can search values that are coded in different ways,
            like boost::mpl::int_ and static_int.

            It may belong to common, but its use may be not so easily explained.
         */
        template <typename ListOfIndices, typename Value>
        struct contains_value {
            template <typename TheValue>
            struct has_value {
                template <typename Element>
                struct apply {
                    static const bool value = Value::value == Element::value;
                    using type = boost::mpl::bool_<value>;
                };
            };

            using cnt = typename boost::mpl::count_if<ListOfIndices, typename has_value<Value>::template apply<boost::mpl::_>>::type;
            using type = boost::mpl::bool_<cnt::value >= 1>;
            static const bool value = type::value;
        };


        /** Metafunction to collect all the indices of a sequence
            corresponding to non-accessors. Used in calling interface
            to provide enable_ifs based on accessor indices of the
            called function.
         */
        template <typename PArguments>
        struct insert_index_if_not_accessor {
            template <typename Index, typename CurrentState>
            struct apply {
                typedef typename boost::mpl::at<PArguments, static_uint<Index::value>>::type to_check;
                typedef typename boost::mpl::if_<
                    is_accessor<to_check>,
                    CurrentState,
                    typename boost::mpl::push_back<CurrentState, Index>::type
                    >::type type;
            };
        };


        /** Struct to wrap a reference that is passed to a procedure
            (call_proc). This is used to deduce the type of the arguments
            required by the called function.
         */
        template <typename Type>
        struct wrap_reference {
            using type = Type;

            type * p_value;

            wrap_reference(type const& v)
                : p_value(const_cast<typename std::decay<type>::type*>(&v))
            {}

            type& value() const {return *p_value;}
        };

        /** When calling a procedure, a new aggregator has to be
            produced which keeps the accessors as they are and
            wrap the references.
         */
        template <typename ...Args> struct package_args;

        template <class First, typename ...Args>
        struct package_args<First, Args...>
        {
            using thefirst = typename std::decay<First>::type;
            typedef typename boost::mpl::if_c<
                is_accessor<thefirst>::value,
                thefirst,
                wrap_reference<thefirst>
                >::type to_pack;
            typedef typename boost::mpl::push_front<
                typename package_args<Args...>::type, to_pack>::type type;
        };

        template <class T>
        struct package_args<T>
        {
            using thefirst = typename std::decay<T>::type;
            typedef typename boost::mpl::if_c<
                is_accessor<thefirst>::value,
                thefirst,
                wrap_reference<thefirst>
                >::type to_pack;
            typedef boost::mpl::vector<to_pack> type;
        };

        template <>
        struct package_args<>
        {
            typedef boost::mpl::vector<> type;
        };

        /** Maker to wrap an argument if it is not an accessor.

            Used to apply the transformation to a variadic pack.
         */
        template <typename T>
        inline
        typename boost::enable_if_c<is_accessor<T>::value, T>::type
        make_wrap(T const& v) {
            return v;
        }

        template <typename T>
        inline
        typename boost::enable_if_c<not is_accessor<T>::value, _impl::wrap_reference<T> >::type
        make_wrap(T const& v) {
            return _impl::wrap_reference<typename std::decay<T>::type >(v);
        }


    } // namespace _impl
} // namespace gridtools
