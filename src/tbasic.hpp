#include <stddef.h>

#pragma once 

namespace tbasic {
    template<typename T>
    struct Box {
        using Type = T; 
    };

    template<template<class...> class F>
    struct BoxF {
        template<class... ARGS>
        struct Template {
            using Type =
                F<ARGS...>;
        };
    };

    struct True {
        static const bool value = true; 
    };

    struct False {
        static const bool value = false; 
    };

    template<typename BOOL, typename ENABLED = void>
    struct Not {
        using Type = True;
    };

    template<typename BOOL>
    struct Not<BOOL, std::enable_if_t<BOOL::value>> {
        using Type = False;
    };

    template<typename BOOL1, typename BOOL2, typename ENABLED = void>
    struct And {
        using Type = False;
    };

    template<typename BOOL1, typename BOOL2>
    struct And<BOOL1, BOOL2, std::enable_if_t<BOOL1::value && BOOL2::value>> {
        using Type = True;
    };

    template<typename BOOL1, typename BOOL2, typename ENABLED = void>
    struct Or {
        using Type = False;
    };

    template<typename BOOL1, typename BOOL2>
    struct Or<BOOL1, BOOL2, std::enable_if_t<BOOL1::value || BOOL2::value>> {
        using Type = True;
    };

    template<typename COND, typename THEN_CLAUSE, typename ELSE_CLAUSE, typename ENABLED = void>
    struct If {
        using Type = typename ELSE_CLAUSE::Type;
    };

    template<typename COND, typename THEN_CLAUSE, typename ELSE_CLAUSE>
    struct If<COND, THEN_CLAUSE, ELSE_CLAUSE, std::enable_if_t<COND::value>> {
        using Type = typename THEN_CLAUSE::Type;
    };

    template<size_t N>
    struct Nat {
        static const size_t value = N;
    };

    template<typename NAT1, typename NAT2, typename ENABLED = void>
    struct IsNatEQ {
        using Type = False;
    };

    template<typename NAT1, typename NAT2>
    struct IsNatEQ<NAT1, NAT2, std::enable_if_t<NAT1::value == NAT2::value>> {
        using Type = True;
    };

    template<typename NAT1, typename NAT2, typename ENABLED = void>
    struct IsNatLT {
        using Type = False;
    };

    template<typename NAT1, typename NAT2>
    struct IsNatLT<NAT1, NAT2, std::enable_if_t<(NAT2::value > NAT1::value)>> {
        using Type = True;
    };

    template<typename NAT1, typename NAT2>
    struct IsNatLTEQ {
        using Type = 
            typename Or<
                typename IsNatLT<NAT1, NAT2>::Type,
                typename IsNatEQ<NAT1, NAT2>::Type
            >::Type;
    };

    template<typename NAT1, typename NAT2, typename ENABLED = void>
    struct IsNatGT {
        using Type = False;
    };

    template<typename NAT1, typename NAT2>
    struct IsNatGT<NAT1, NAT2, std::enable_if_t<(NAT1::value > NAT2::value)>> {
        using Type = True;
    };

    template<typename NAT1, typename NAT2>
    struct IsNatGTEQ {
        using Type = 
            typename Or<
                typename IsNatGT<NAT1, NAT2>::Type,
                typename IsNatEQ<NAT1, NAT2>::Type
            >::Type;
    };

    template<typename NAT, typename...>
    struct Inc {
        using Type = Nat<NAT::value + 1>;
    };

    template<typename NAT, typename...>
    struct Dec {
        using Type = Nat<NAT::value - 1>;
    };

    template<typename T>
    struct SizeOf {
        using Type = Nat<sizeof(T)>;
    };

    template<template<class, class, class...> class F>
    struct Flip {
        template<class T1, class T2, class... T_REST> 
        struct Template {
            using Type = typename F<T2, T1, T_REST...>::Type;
        };
    };
}