// Copyright © 2025 Giorgio Audrito. All Rights Reserved.

/**
 * @file beautify.hpp
 * @brief Header defining macros for cleaning up FCPP code.
 */

#ifndef FCPP_BEAUTIFY_H_
#define FCPP_BEAUTIFY_H_


//! @cond INTERNAL
#define __TYPE_ARG__(T) typename T
#define __PLACE_ARG__(p) tier_t p

#define __MAPPER0__(M)
#define __MAPPER1__(M,A)                          M(A)
#define __MAPPER2__(M,A,B)                        __MAPPER1__(M,A), __MAPPER1__(M,B)
#define __MAPPER3__(M,A,B,C)                      __MAPPER1__(M,A), __MAPPER2__(M,B,C)
#define __MAPPER4__(M,A,B,C,D)                    __MAPPER1__(M,A), __MAPPER3__(M,B,C,D)
#define __MAPPER5__(M,A,B,C,D,E)                  __MAPPER1__(M,A), __MAPPER4__(M,B,C,D,E)
#define __MAPPER6__(M,A,B,C,D,E,F)                __MAPPER1__(M,A), __MAPPER5__(M,B,C,D,E,F)
#define __MAPPER7__(M,A,B,C,D,E,F,G)              __MAPPER1__(M,A), __MAPPER6__(M,B,C,D,E,F,G)
#define __MAPPER8__(M,A,B,C,D,E,F,G,H)            __MAPPER1__(M,A), __MAPPER7__(M,B,C,D,E,F,G,H)
#define __MAPPER9__(M,A,B,C,D,E,F,G,H,I)          __MAPPER1__(M,A), __MAPPER8__(M,B,C,D,E,F,G,H,I)
#define __MAPPERX__(M,A,B,C,D,E,F,G,H,I,X,...)    X
//! @endcond

//! @brief Maps a macro to a variable number of arguments (up to 9), comma separating the calls.
#define MACRO_MAPPER(...)                         __MAPPERX__(__VA_ARGS__, __MAPPER9__, __MAPPER8__, \
                                                              __MAPPER7__, __MAPPER6__, __MAPPER5__, \
                                                              __MAPPER4__, __MAPPER3__, __MAPPER2__, \
                                                              __MAPPER1__, __MAPPER0__)(__VA_ARGS__)

//! @brief Macro defining a non-generic aggregate function.
#define FUN             template <typename node_t>

//! @brief Macro defining the type arguments of an aggregate function.
#define GEN(...)        template <MACRO_MAPPER(__TYPE_ARG__, node_t, __VA_ARGS__)>

//! @brief Bounds generic function type F to comply with signature T.
#define BOUND(F, T)     = common::type_unwrap<void(common::type_sequence<common::if_signature<F, T>>)>

//! @brief Macro inserting the default arguments.
#define ARGS            node_t& node, trace_t call_point

//! @brief Macro inserting the default arguments at function call.
#define CALL            node, __COUNTER__-trace_counter

//! @brief Macro inserting inlining arguments at function call, within a function missing the CODE macro.
#define INLINE          node, call_point

//! @brief Macro inserting the default code at function start.
#define CODE            constexpr int trace_counter = __COUNTER__+1;                    \
                        internal::trace_call trace_caller(node.stack_trace, call_point);

//! @brief Macro defining a non-generic aggregate function.
#define FUN_EXPORT      using

//! @brief Macro defining the type arguments of an aggregate function.
#define GEN_EXPORT(...) template <MACRO_MAPPER(__TYPE_ARG__, __VA_ARGS__)> using

//! @brief Macro defining the index of an aggregate for loop.
#define LOOP(v, s)      internal::trace_cycle v{node.stack_trace, trace_t(s)}

/**
 * @brief Macro for defining a main class to be used in the calculus component. Usage:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * MAIN() {
 *   ...
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * The code of the main function has access to the `node` object.
 */
#define MAIN()                                          \
struct main {                                           \
    static constexpr int trace_counter = __COUNTER__+1; \
    template <typename node_t>                          \
    void operator()(node_t&, times_t);                  \
};                                                      \
template <typename node_t>                              \
void main::operator()(node_t& node, times_t)


//! @brief Macro defining a non-generic aggregate function with placements.
#define PFUN            template <tier_t tier, typename node_t>

//! @brief Macro defining the template arguments of an aggregate function with placements.
#define PGEN(...)       template <tier_t tier, typename node_t, __VA_ARGS__>

//! @brief Macro defining the type template parameters of an aggregate function with placements.
#define PTYPE(...)      MACRO_MAPPER(__TYPE_ARG__, __VA_ARGS__)

//! @brief Macro defining the placement template arguments of an aggregate function with placements.
#define PPLACE(...)     MACRO_MAPPER(__PLACE_ARG__, __VA_ARGS__)

//! @brief Macro defining the placement parameters of an aggregate function with placements.
#define TIER(tier)      std::integer_sequence<tier_t, tier>

//! @brief Macro inserting the default arguments.
#define PARGS           TIER(tier), node_t& node, trace_t call_point

//! @brief Macro inserting the default arguments at function call.
#define PCALL           TIER(tier){}, node, __COUNTER__

//! @brief Macro adding placement annotations to a local type.
#define place(...)      placed<tier, __VA_ARGS__>

#if FCPP_TIERS == FCPP_TIERS_VARIABLE
/**
 * @brief Macro for defining a main class with placements to be used in the calculus component. Usage:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * PMAIN() {
 *   ...
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * The code of the main function has access to the `node` object and the current `tier` value.
 * If @ref FCPP_TIERS is @ref FCPP_TIERS_VARIABLE , the current tier of a node is taken from the node storage,
 * as a property associated with @ref fcpp::component::tags::node_tier .
 * Otherwise, the main is compiled with the `FCPP_TIER` defining the value for `tier`.
 */
#define PMAIN()                                             \
struct main {                                               \
    template <typename node_t>                              \
    inline void operator()(node_t&, times_t);               \
    template <typename node_t>                              \
    inline void body_rec(TIER(FCPP_TIERS_MAX), node_t&);    \
    template <tier_t t, typename node_t>                    \
    inline void body_rec(TIER(t), node_t&);                 \
    template <tier_t tier, typename node_t>                 \
    void body(node_t&);                                     \
};                                                          \
template <typename node_t>                                  \
void main::operator()(node_t& node, times_t) {              \
    body_rec(TIER(0){}, node);                              \
}                                                           \
template <typename node_t>                                  \
void main::body_rec(TIER(FCPP_TIERS_MAX), node_t&) {        \
    assert(false);                                          \
}                                                           \
template <tier_t t, typename node_t>                        \
void main::body_rec(TIER(t), node_t& node) {                \
    using fcpp::component::tags::node_tier;                 \
    if (node.storage(node_tier{}) == (1<<t))                \
        body<(1<<t)>(node);                                 \
    else body_rec(TIER(t+1){}, node);                       \
}                                                           \
template <tier_t tier, typename node_t>                     \
void main::body(node_t& node)

#else
/**
 * @brief Macro for defining a main class with placements to be used in the calculus component. Usage:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * PMAIN() {
 *   ...
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * The code of the main function has access to the `node` object and the current `tier` value.
 * If @ref FCPP_TIERS is @ref FCPP_TIERS_VARIABLE , the current tier of a node is taken from the node storage,
 * as a property associated with @ref fcpp::component::tags::node_tier .
 * Otherwise, the main is compiled with the `FCPP_TIER` defining the value for `tier`.
 */
#define PMAIN()                                     \
struct main {                                       \
    template <typename node_t>                      \
    inline void operator()(node_t&, times_t);       \
    template <tier_t tier, typename node_t>         \
    void body(node_t&);                             \
};                                                  \
template <typename node_t>                          \
void main::operator()(node_t& node, times_t) {      \
    body<(1<<FCPP_TIER)>(node);                     \
}                                                   \
template <tier_t tier, typename node_t>             \
void main::body(node_t& node)

#endif

#endif // FCPP_BEAUTIFY_H_
