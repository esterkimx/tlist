#include <iostream>
#include <limits>
#include <tuple>
#include "../src/tlist.hpp"

using namespace tbasic;
using namespace tlist;

// Bools

static_assert(True::value);
static_assert(!False::value);
static_assert(std::is_same<Not<True>::Type, False>::value);
static_assert(std::is_same<Not<False>::Type, True>::value);
static_assert(And<True, True>::Type::value);
static_assert(!And<True, False>::Type::value);
static_assert(!And<False, True>::Type::value);
static_assert(!And<False, False>::Type::value);
static_assert(Or<True, True>::Type::value);
static_assert(Or<True, False>::Type::value);
static_assert(Or<False, True>::Type::value);
static_assert(!Or<False, False>::Type::value);

// Conditionals

static_assert(std::is_same<If<True, Box<int>, Box<double>>::Type, int>::value);
static_assert(std::is_same<If<False, Box<int>, Box<double>>::Type, double>::value);
static_assert(std::is_same<If<True, 
    Box<int>, 
    Repeat<std::numeric_limits<unsigned long long>::max(), double>
>::Type, int>::value);
static_assert(std::is_same<If<False, 
    Repeat<std::numeric_limits<unsigned long long>::max(), int>, 
    Box<double>
>::Type, double>::value);

// Nat

static_assert(Nat<5>::value == 5);
static_assert(IsNatEQ<Nat<5>, Nat<5>>::Type::value);
static_assert(IsNatLT<Nat<0>, Nat<5>>::Type::value);
static_assert(IsNatLTEQ<Nat<0>, Nat<5>>::Type::value);
static_assert(IsNatLTEQ<Nat<5>, Nat<5>>::Type::value);
static_assert(IsNatGT<Nat<5>, Nat<0>>::Type::value);
static_assert(IsNatGTEQ<Nat<5>, Nat<0>>::Type::value);
static_assert(IsNatGTEQ<Nat<5>, Nat<5>>::Type::value);
static_assert(IsNatEQ<Nat<6>, Inc<Nat<5>>::Type>::Type::value);

// TLists

static_assert(TList<>::Type::isEmpty);
static_assert(!TList<int>::Type::isEmpty);
static_assert(std::is_same<IsEmpty<TList<>::Type>::Type, True>::value);
static_assert(std::is_same<IsEmpty<TList<int>::Type>::Type, False>::value);

// List Functions

static_assert(std::is_same<
    TList<
        TList<int>::Type, 
        TList<float>::Type,
        TList<double>::Type
    >::Type,
    Map<
        TList,
        TList<int, float, double>::Type
    >::Type
>::value);

static_assert(std::is_same<
    TList<char, int, float, unsigned int, double, long int, long long int>::Type,
    Merge<
        SizeOf, 
        TList<int, float, double, long int>::Type,
        TList<char, unsigned int, long long int>::Type
    >::Type
>::value);

static_assert(std::is_same<
    TList<
        TList<int, float>::Type, 
        TList<double, long int>::Type,
        TList<char, unsigned int>::Type,
        TList<long long int>::Type
    >::Type,
    MergePairs<
        SizeOf,
        Map<
            TList, 
            TList<int, float, double, long int, unsigned int, char, long long int>::Type
        >::Type
    >::Type
>::value);

static_assert(std::is_same<
    TList<char[1], char[1], char[2], char[2], char[7], char[8], char[10]>::Type,
    SortBy<
        SizeOf,
        TList<char[1], char[8], char[7], char[1], char[2], char[10], char[2]>::Type
    >::Type
>::value);

static_assert(std::is_same<
    std::tuple<int, float, double>,
    Apply<
        BoxF<std::tuple>::Template,
        TList<int, float, double>::Type
    >::Type
>::value);

template<typename T1>
struct TypeEQ {
    template<typename T2, typename ENABLED = void> 
    struct Template {
        using Type = False;
    };

    template<typename T2> 
    struct Template<T2, std::enable_if_t<std::is_same<T1, T2>::value>> {
        using Type = True;
    };
};

static_assert(
    3 == FindBy<
        TypeEQ<int>::Template, 
        TList<double, long int, float, int, char, unsigned int, long long int>::Type
    >::Type::value
);

// Debug 

typedef Cons<int, Empty> Types1;
typedef Cons<int, Cons<float, Empty>> Types2;
typedef Cons<int, Cons<double, Cons<std::string, Empty>>> Types3;

typedef TList<>::Type Types0;
typedef TList<int, double, void, std::string>::Type Types4;
typedef TList<
    int, double, char, float, size_t, int, double, char, float, size_t
>::Type Types10;
typedef Concat<Repeat<10, Types10>::Type>::Type Types100;
typedef Concat<Repeat<10, Types100>::Type>::Type Types1000;

template<typename EMPTY_LIST>
std::enable_if_t<EMPTY_LIST::isEmpty, void> printInternal() { 
    std::cout << "]" << std::endl;
}

template<typename LIST>
std::enable_if_t<!LIST::isEmpty, void> printInternal() { 
    std::cout << ", " << typeid(typename LIST::Head).name();
    printInternal<typename LIST::Tail>();
}

template<typename EMPTY_LIST>
std::enable_if_t<EMPTY_LIST::isEmpty, void> printLn() { 
    std::cout << "T[]" << std::endl;
}

template<typename LIST>
std::enable_if_t<!LIST::isEmpty, void> printLn() { 
    std::cout << "[" << typeid(typename LIST::Head).name();
    printInternal<typename LIST::Tail>();
}

template<typename T, typename ENABLED = void>
struct SizeOfLTEQ4 {
    typedef False Type;
};

template<typename T>
struct SizeOfLTEQ4<T, std::enable_if_t<(4 >= sizeof(T))>> {
    typedef True Type;
};

template<typename T>
struct TestIf {
    typedef typename If<
        True,
        Repeat<3, T>,
        Repeat<1000000, int>
    >::Type Type; 
};

void ________(std::string title) {
    std::cout << "------ " << title << " ------" << std::endl;
}

int main() {
    printLn<TList<>::Type>();
    printLn<Types4>();
    printLn<Repeat<5, int>::Type>();
    printLn<Repeat<8, TList<>::Type>::Type>();
    printLn<Reverse<Types4>::Type>();
    printLn<Append<Types3, Types4>::Type>();

    std::cout << Length<Types100>::Type::value << std::endl;
    std::cout 
        << Length<
            Append<Types4, Reverse<Types1000>::Type>::Type
        >::Type::value 
        << std::endl;

    printLn<
        If<
            True,
            TList<void>,
            Repeat<1000000, int>
        >::Type
    >();

    printLn<TestIf<int>::Type>();
    
    ________("Filter");

    printLn<
        Filter<
            SizeOfLTEQ4,
            TList<int, float, double, long int, double, long long int, char>::Type
        >::Type
    >();

    ________("SortBy");

    printLn<
       Filter<
            ComparatorGTEQ<SizeOf, double>::template Template,
            TList<double, long int, float, int, char, unsigned int, long long int>::Type
        >::Type
    >();

    return 0;
}