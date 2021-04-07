/*
	File Cast.h

	Author: Garrett Courtney

	Description:
	Wrapper for C++ casting operations.
*/
#pragma once

/*
	Performs a const_cast on specified value.
*/
template <typename As, typename Value>
inline constexpr As CCast(Value* Val)
{
	return const_cast<As>(Val);
}

/*
	Performs a reinterpret_cast on specified value.
*/
template <typename As, typename Value>
inline constexpr As RCast(Value* Val)
{
	return reinterpret_cast<As>(Val);
}

/*
	Performs a dynamic_cast on specified value.
*/
template <typename As, typename Value>
inline constexpr As DCast(Value* Val)
{
	return dynamic_cast<As>(Val);
}

/*
	Performs a static_cast on specified value.
*/
template <typename As, typename Value>
inline constexpr As SCast(Value& Val)
{
	return static_cast<As>(Val);
}
