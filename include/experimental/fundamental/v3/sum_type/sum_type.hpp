//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef JASEL_FUNDAMENTAL_V3_SUM_TYPE_PRODUCT_TYPE_HPP
#define JASEL_FUNDAMENTAL_V3_SUM_TYPE_PRODUCT_TYPE_HPP
#define YAFPL_X1
#if __cplusplus >= 201402L

///////////////////////////////////////////////////////////////////////////////////////
///
/// A type ST is a model of a ProductType if given variable st of type ST
///
///   sum_type::size<ST>::value
///   sum_type::alternative<I,ST>::type
///   sum_type::get<I>(st)
///
/// The definition of these traits/functions depend on whether P0327 is adopted by the standard and
/// can be implemented only by the compiler, as it does for Range-based for loops.
///
/// What follows is just an emulation for the cases 1 (c-arrays) and 2 (variant-like types).
/// The case 3 can be implemented only by the compiler.
///////////////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <experimental/utility.hpp>
#define JASEL_HAS_EXPERIMENTAL_VARIANT 0
#if JASEL_HAS_EXPERIMENTAL_VARIANT
#include <experimental/variant>
#else


// 20.7.3, variant helper classes
template <class T> struct variant_size; // undefined template <class T> struct variant_size<const T>;
template <class T> struct variant_size<volatile T>; template <class T> struct variant_size<const volatile T>; template <class T> constexpr size_t variant_size_v
      = variant_size<T>::value;

template <size_t I, class T> struct variant_alternative; // undefined
template <size_t I, class T> struct variant_alternative<I, const T>;
template <size_t I, class T> struct variant_alternative<I, volatile T>;
template <size_t I, class T> struct variant_alternative<I, const volatile T>;
template <size_t I, class T>
      using variant_alternative_t = typename variant_alternative<I, T>::type;

constexpr size_t variant_npos = -1;


#endif

#include <experimental/type_traits.hpp>
#include <experimental/meta.hpp>

namespace std
{
namespace experimental
{
inline namespace fundamental_v3
{

  template <class V, class T>
  auto visit(V&& v, T&& x) -> decltype(v(x))
  {
    return v(x);
  }

namespace detail
{

  template <class T>
  struct has_variant_like_size_access
  {
      struct yes{ char a[1]; };
      struct no { char a[2]; };

      template<class U> static yes test(decltype(variant_size<U>::value)*);
      template<class U> static no test(...);
      static const bool value = sizeof(test<T>(nullptr)) == sizeof(yes);
  };

  template <size_t N, class T>
  struct has_variant_like_alternative_access
  {
      struct yes{ char a[1]; };
      struct no { char a[2]; };

      template <size_t I, class U> static yes test(typename variant_alternative<I,U>::type*);
      template <size_t I, class U> static no test(...);
      static const bool value = sizeof(test<N, T>(nullptr)) == sizeof(yes);
  };
  namespace variant_get_adl {
    using std::get;
    template <size_t N, class T>
    auto xget(T&& t) noexcept -> decltype( get<N>(forward<T>(t)) )
    {
      return get<N>(forward<T>(t));
    }
  }
  namespace variant_visit_adl {
    using std::experimental::visit;
    template <class V, class T>
    auto xvisit(V&& v, T&& t) noexcept -> decltype( visit(forward<V>(v), forward<T>(t)) )
    {
      return visit(forward<V>(v), forward<T>(t));
    }
  }
  template<size_t N, class T>
  struct has_variant_like_get_access
  {
      struct yes{ char a[1]; };
      struct no { char a[2]; };

      template <size_t I, class U>
          static yes test(remove_reference_t<decltype(detail::variant_get_adl::xget<I>(declval<U>()))>*);
      template <size_t I, class U>
          static no test(...);
      static constexpr bool value = sizeof(test<N,T>(nullptr)) == sizeof(yes);
  };

  template <class T, class  Indexes>
  struct has_variant_like_alternative_get_access_aux;
#if defined JASEL_HAS_FOLD_EXPRESSIONS
  template <class T, size_t ...N>
  struct has_variant_like_alternative_get_access_aux<T, index_sequence<N...>> : integral_constant<bool,
    (has_variant_like_alternative_access<N, T>::value && ...)
    &&
    (has_variant_like_get_access<N, T>::value && ...)
    > {};
#else
  template <class T, size_t ...N>
  struct has_variant_like_alternative_get_access_aux<T, index_sequence<N...>> : conjunction<
    has_variant_like_alternative_access<N, T> ...
    ,
    has_variant_like_get_access<N, T> ...
    > {};
#endif

  template <class T, bool B>
  struct has_variant_like_alternative_get_access:
      has_variant_like_alternative_get_access_aux<T, make_index_sequence<variant_size<T>::value>>
  {};
  template <class T>
  struct has_variant_like_alternative_get_access<T, false>: false_type {};


}

  template <class T>
  struct has_variant_like_access : conjunction<
    detail::has_variant_like_size_access<T>,
    detail::has_variant_like_alternative_get_access<T, detail::has_variant_like_size_access<T>::value>
    > {};

namespace sum_type {
    template <class ST, class Enabler=void>
    struct traits : traits<ST, meta::when<true>> {};

    // Default failing specialization
    template <class  ST, bool condition>
    struct traits<ST, meta::when<condition>>
    {
#if 0
        template <class T>
          static constexpr auto get(T&& x) =delete;
#else
        using size = integral_constant<size_t, 1>;

        template <size_t I>
        using alternative = ST;

        template <size_t I, class ST2, class= std::enable_if_t< I < size::value > >
          static constexpr decltype(auto) get(ST2&& st) noexcept
          {
            return forward<ST2>(st);
          }
        template <class V, class ST2>
          static constexpr decltype(auto) visit(V&& v, ST2&& st) noexcept
          {
            return forward<V>(v)(forward<ST2>(st));
          }

#endif
    };

    struct tag{};

    // Forward to customized class using variant-like access
    template <class ST>
    struct traits
    <  ST, meta::when< has_variant_like_access<ST>::value >  > : tag
    {
      using size = variant_size<ST>;

      template <size_t I>
      using alternative = variant_alternative<I, ST>;

      template <size_t I, class ST2, class= std::enable_if_t< I < size::value > >
        static constexpr decltype(auto) get(ST2&& st) noexcept
        {
          return detail::variant_get_adl::xget<I>(forward<ST2>(st));
        }
      template <class V, class ST2>
        static constexpr decltype(auto) visit(V&& v, ST2&& st) noexcept
        {
          return detail::variant_visit_adl::xvisit(forward<V>(v), forward<ST2>(st));
        }
    };

    template <class ST>
    struct size : traits<ST>::size {};
    template <class T> struct size<const T> : size<T> {};
    template <class T> struct size<volatile T> : size<T> {};
    template <class T> struct size<const volatile T> : size<T> {};
    template <class ST>
    constexpr size_t size_v = size<ST>::value;

    template <size_t I, class ST, class= std::enable_if_t< I<size_v<ST> >>
    struct alternative : traits<ST>::template alternative<I> {};
    template <size_t I, class ST>
    using alternative_t = typename alternative<I,ST>::type;
    template <size_t I, class T> struct alternative<I, const T> { using type = const alternative_t<I, T>; };
    template <size_t I, class T> struct alternative<I, volatile T> { using type = volatile alternative_t<I, T>; };
    template <size_t I, class T> struct alternative<I, const volatile T> { using type = const volatile alternative_t<I, T>; };

    template <class ST>
    struct empty : integral_constant<bool, 0 == sum_type::size_v<ST>> {};
    template <class ST>
    constexpr size_t empty_v = empty<ST>::value;
    template <class ST>
    struct not_empty : integral_constant<bool, (0 != sum_type::size_v<ST>)> {};
    template <class ST>
    constexpr size_t not_empty_v = not_empty<ST>::value;

    template <size_t I, class ST
      , class= std::enable_if_t< I < size_v<remove_cv_t<remove_reference_t<ST>>> >
    >
    constexpr decltype(auto) get(ST && st) noexcept
    {
        return traits<remove_cv_t<remove_reference_t<ST>>>::template get<I>(forward<ST>(st));
    }

    template <class V, class ST>
    constexpr decltype(auto) visit(V&& v, ST && st) noexcept
    {
        return traits<remove_cv_t<remove_reference_t<ST>>>::visit(forward<V>(v), forward<ST>(st));
    }

    template <class ST>
    constexpr size_t get_size(ST && st) noexcept { return sum_type::size_v<remove_reference_t<ST>>; }

    template <class ST>
    constexpr size_t is_empty(ST && st) noexcept { return sum_type::empty_v<remove_reference_t<ST>>; }

    template <class ST>
    using alternative_sequence_for = make_index_sequence<sum_type::size_v<remove_cv_t<remove_reference_t<ST>>>>;

    namespace detail {
      template <class ST, class Ids>
      struct alternatives;
      template <class ST, size_t ...I>
      struct alternatives<ST, index_sequence<I...>>
      {
        using type = meta::types<sum_type::alternative_t<I, ST>...>;
      };
    }
    template <class ST>
    struct alternatives : detail::alternatives<remove_cv_t<remove_reference_t<ST>>, sum_type::alternative_sequence_for<ST>> {};

  }

  // fixme redefine it as we did for has_variant_like_access
  template <class T>
  struct is_sum_type : is_base_of<sum_type::tag, sum_type::traits<T>> {};
  template <class T>
  struct is_sum_type<const T> : is_sum_type<T> {};
  template <class T>
  struct is_sum_type<volatile T> : is_sum_type<T> {};
  template <class T>
  struct is_sum_type<const volatile T> : is_sum_type<T> {};
  template <class T>
  constexpr bool is_sum_type_v = is_sum_type<T>::value ;

  namespace sum_type {
    template <class ST, template <class> class Trait, bool B = is_sum_type_v<ST> >
    struct friendly_type_trait : false_type {};
    template <class ST, template <class> class Trait>
    struct friendly_type_trait<ST, Trait, true> : Trait<ST> {};
  }

}}
}
#endif
#endif // header
