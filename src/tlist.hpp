#pragma once 

#include "tbasic.hpp"

using namespace tbasic;

namespace tlist {
    struct Empty {
        static const bool isEmpty = true;
    };

    template<typename HEAD, typename TAIL>
    struct Cons {
        using Head = HEAD;
        using Tail = TAIL; 
        static const bool isEmpty = false;
    };

    template<typename HEAD, typename TAIL>
    struct BoxCons {
        using Type = Cons<HEAD, TAIL>;
    };

    template<typename...>
    struct TList {
        using Type = Empty;
    };

    template<typename HEAD, typename... TAIL>
    struct TList<HEAD, TAIL...> {
        using Type = Cons<HEAD, typename TList<TAIL...>::Type>;
    };

    // Foldr

    template<template<class...> class F, typename ACC, typename EMPTY_LIST, typename ENABLED = void>
    struct Foldr {
        using Type = ACC;
    };

    template<template<class...> class F, typename ACC, typename LIST>
    struct Foldr<F, ACC, LIST, std::enable_if_t<!LIST::isEmpty>> {
        using Type = 
            typename F<
                typename LIST::Head, 
                typename Foldr<F, ACC, typename LIST::Tail>::Type
            >::Type;
    };

    // Foldl

    template<template<class...> class F, typename ACC, typename EMPTY_LIST, typename ENABLED = void> 
    struct Foldl {
        using Type = ACC;
    };

    template<template<class...> class F, typename ACC, typename LIST>
    struct Foldl<F, ACC, LIST, std::enable_if_t<!LIST::isEmpty>> {
        using Type =
            typename Foldl<
                F, 
                typename F<ACC, typename LIST::Head>::Type, 
                typename LIST::Tail
            >::Type;
    };

    // Map

    template<template<class...> class F>
    struct ConsF {
        template<typename HEAD, typename TAIL>
        struct Template {
            using Type = Cons<typename F<HEAD>::Type, TAIL>;
        };
    };

    template<template<class...> class F, typename LIST>
    struct Map {
        using Type =
            typename Foldr<
                ConsF<F>::template Template,
                TList<>::Type,
                LIST
            >::Type;
    };

    // Common Functions

    template<typename EMPTY_LIST, typename ENABLED = void>
    struct IsEmpty {
        using Type = True;
    };

    template<typename LIST>
    struct IsEmpty<LIST, std::enable_if_t<!LIST::isEmpty>> {
        using Type = False;
    };

    template<typename LIST>
    struct Length {
        using Type = 
            typename Foldl<
                Inc,
                Nat<0>,
                LIST
            >::Type;
    };

    template<typename EMPTY_LIST, typename ENABLED = void>
    struct Last {
        using Type = void;   
    };

    template<typename SINGLETON_LIST>
    struct Last<SINGLETON_LIST, std::enable_if_t<SINGLETON_LIST::Tail::isEmpty>> {
        using Type = typename SINGLETON_LIST::Head;
    };

    template<typename LIST>
    struct Last<LIST, std::enable_if_t<!LIST::Tail::isEmpty>> {
        using Type = typename Last<typename LIST::Tail>::Type;
    };

    template<typename LIST>
    struct Reverse {
        using Type = 
            typename Foldl<
                Flip<BoxCons>::Template,
                TList<>::Type,
                LIST
            >::Type;
    };

    template<size_t N, typename T, typename ENABLED = void>
    struct Repeat {
        using Type = TList<>::Type;
    };

    template<size_t N, typename T>
    struct Repeat<N, T, std::enable_if_t<(N > 0)>> {
        using Type = Cons<T, typename Repeat<N - 1, T>::Type>;
    };

    template<typename LIST1, typename LIST2>
    struct Append {
        using Type = 
            typename Foldr<BoxCons, LIST2, LIST1>::Type;
    };

    template<typename LIST_OF_LISTS>
    struct Concat {
        using Type = 
            typename Foldl<
                Append,
                TList<>::Type,
                LIST_OF_LISTS 
            >::Type;
    };

    template<template<class, class...> class PRED, typename EMPTY_LIST, typename ENABLED = void>
    struct Filter {
        using Type = EMPTY_LIST;
    };

    template<template<class, class...> class PRED, typename LIST>
    struct Filter<PRED, LIST, std::enable_if_t<!LIST::isEmpty>> {
        using Type =
            typename If<
                typename PRED<typename LIST::Head>::Type,
                BoxCons<typename LIST::Head, typename Filter<PRED, typename LIST::Tail>::Type>,
                Filter<PRED, typename LIST::Tail>
            >::Type;
    };

    template<template<class, class...> class WEIGHT, class T1>
    struct ComparatorLT {
        template<class T2, class...> 
        struct Template {
            using Type =
                typename IsNatLT<
                    typename WEIGHT<T2>::Type,
                    typename WEIGHT<T1>::Type
                >::Type;
        };
    };

    template<template<class, class...> class WEIGHT, class T1>
    struct ComparatorGTEQ {
        template<class T2, class...> 
        struct Template {
            using Type =
                typename IsNatGTEQ<
                    typename WEIGHT<T2>::Type,
                    typename WEIGHT<T1>::Type
                >::Type;
        };
    };

    template<template<class, class...> class WEIGHT, typename SORTED_LIST1, typename SORTED_LIST2>
    struct Merge;

    template<template<class, class...> class WEIGHT, typename SORTED_LIST1, typename SORTED_LIST2>
    struct MergeNotEmptyLists {
        using Type =
            typename If<
                typename IsNatLTEQ<
                    typename WEIGHT<typename SORTED_LIST1::Head>::Type,
                    typename WEIGHT<typename SORTED_LIST2::Head>::Type
                >::Type,
                BoxCons<
                    typename SORTED_LIST1::Head,
                    typename Merge<
                        WEIGHT,
                        typename SORTED_LIST1::Tail,
                        SORTED_LIST2
                    >::Type
                >,
                BoxCons<
                    typename SORTED_LIST2::Head,
                    typename Merge<
                        WEIGHT,
                        SORTED_LIST1,
                        typename SORTED_LIST2::Tail
                    >::Type
                >
            >::Type;
    };

    template<template<class, class...> class WEIGHT, typename SORTED_LIST1, typename SORTED_LIST2>
    struct Merge {
        using Type = 
            typename 
            If<typename IsEmpty<SORTED_LIST1>::Type, Box<SORTED_LIST2>,
            If<typename IsEmpty<SORTED_LIST2>::Type, Box<SORTED_LIST1>,
                MergeNotEmptyLists<WEIGHT, SORTED_LIST1, SORTED_LIST2>
            >>::Type;
    };

    template<template<class, class...> class WEIGHT, typename LIST_OF_SORTED_LISTS>
    struct MergePairs;

    template<template<class, class...> class WEIGHT, typename LIST_OF_SORTED_LISTS>
    struct MergePairsInListWithMoreThanTwoElements {
        using Type =
            Cons<
                typename Merge<
                    WEIGHT, 
                    typename LIST_OF_SORTED_LISTS::Head, 
                    typename LIST_OF_SORTED_LISTS::Tail::Head
                >::Type,
                typename MergePairs<
                    WEIGHT, typename LIST_OF_SORTED_LISTS::Tail::Tail
                >::Type
            >;
    };

    template<template<class, class...> class WEIGHT, typename LIST_OF_SORTED_LISTS>
    struct MergePairsInNotEmptyList {
        using Type =
            typename If<typename IsEmpty<typename LIST_OF_SORTED_LISTS::Tail>::Type,
                Box<LIST_OF_SORTED_LISTS>,
                MergePairsInListWithMoreThanTwoElements<WEIGHT, LIST_OF_SORTED_LISTS>
            >::Type;
    };

    template<template<class, class...> class WEIGHT, typename LIST_OF_SORTED_LISTS>
    struct MergePairs {
        using Type =
            typename If<typename IsEmpty<LIST_OF_SORTED_LISTS>::Type, 
                Box<LIST_OF_SORTED_LISTS>,
                MergePairsInNotEmptyList<WEIGHT, LIST_OF_SORTED_LISTS>
            >::Type;
    };

    template<template<class, class...> class WEIGHT, typename LIST_OF_SORTED_LISTS>
    struct SortByRunMergesForNonEmptyList {
        using Type =    
            typename If<typename IsEmpty<typename LIST_OF_SORTED_LISTS::Tail>::Type,
                Box<typename LIST_OF_SORTED_LISTS::Head>,
                SortByRunMergesForNonEmptyList<
                    WEIGHT, 
                    typename MergePairs<WEIGHT, LIST_OF_SORTED_LISTS>::Type
                >
            >::Type;
    };

    template<template<class, class...> class WEIGHT, typename LIST_OF_SORTED_LISTS>
    struct SortByRunMerges {
        using Type =    
            typename If<typename IsEmpty<LIST_OF_SORTED_LISTS>::Type,
                Box<LIST_OF_SORTED_LISTS>,
                SortByRunMergesForNonEmptyList<WEIGHT, LIST_OF_SORTED_LISTS>
            >::Type;
    };

    template<template<class, class...> class WEIGHT, typename LIST>
    struct SortBy {
        using Type =
            typename SortByRunMerges<WEIGHT, 
                typename Map<
                    TList,
                    LIST
                >::Type
            >::Type;
    };

    template<template<class...> class F, typename LIST, typename... ARGS>
    struct Apply;

    template<template<class...> class F, typename LIST, typename... ARGS>
    struct ApplyToNotEmptyList {
        using Type =
            typename Apply<F, typename LIST::Tail, ARGS..., typename LIST::Head>::Type;
    };

    template<template<class...> class F, typename LIST, typename... ARGS>
    struct Apply {
        using Type =
            typename If<typename IsEmpty<LIST>::Type,
                F<ARGS...>,
                ApplyToNotEmptyList<F, LIST, ARGS...>
            >::Type;
    };

    template<template<class...> class PREDICATE, typename EMPTY_LIST, size_t INDEX = 0, typename ENABLED = void>
    struct FindBy {
        using Type = void;
    };

    template<template<class...> class PREDICATE, typename LIST, size_t INDEX>
    struct FindBy<PREDICATE, LIST, INDEX, std::enable_if_t<!LIST::isEmpty>> {
        using Type =
            typename If<
                typename PREDICATE<typename LIST::Head>::Type,
                Box<Nat<INDEX>>,
                FindBy<PREDICATE, typename LIST::Tail, INDEX + 1>
            >::Type;
    };
}