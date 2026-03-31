#pragma once

#define ASSERT_MSG(expr, msg) assert((expr) && (msg))

#define GEN_ENUM_FLAG_OPERATORS(type)                                           \
    inline type operator+(type lhs, type rhs)                                   \
    {                                                                           \
        using T = std::underlying_type_t<type>;                                 \
        return static_cast<type>(static_cast<T>(lhs) + static_cast<T>(rhs));    \
    }                                                                           \
    inline type& operator+=(type& lhs, type rhs)                                \
    {                                                                           \
        lhs = lhs + rhs;                                                        \
        return lhs;                                                             \
    }                                                                           \
    inline type operator-(type lhs, type rhs)                                   \
    {                                                                           \
        using T = std::underlying_type_t<type>;                                 \
        return static_cast<type>(static_cast<T>(lhs) - static_cast<T>(rhs));    \
    }                                                                           \
    inline type& operator-=(type& lhs, type rhs)                                \
    {                                                                           \
        lhs = lhs - rhs;                                                        \
        return lhs;                                                             \
    }                                                                           \
    inline type operator|(type lhs, type rhs)                                   \
    {                                                                           \
        using T = std::underlying_type_t<type>;                                 \
        return static_cast<type>(static_cast<T>(lhs) | static_cast<T>(rhs));    \
    }                                                                           \
    inline type& operator|=(type& lhs, type rhs)                                \
    {                                                                           \
        lhs = lhs | rhs;                                                        \
        return lhs;                                                             \
    }                                                                           \
    inline type operator&(type lhs, type rhs)                                   \
    {                                                                           \
        using T = std::underlying_type_t<type>;                                 \
        return static_cast<type>(static_cast<T>(lhs) & static_cast<T>(rhs));    \
    }                                                                           \
    inline type& operator&=(type& lhs, type rhs)                                \
    {                                                                           \
        lhs = lhs & rhs;                                                        \
        return lhs;                                                             \
    }

enum class EWorldType
{
	None,
	Title,
	Loading,
	MainStage0,
	MainStage1,
	Editor
};

enum class EEditorMode
{
	Tile,
	Hair
};

