#pragma once

namespace Insight
{
	template <size_t S>
	struct IE_ENUM_FLAG_INTEGER_FOR_SIZE;

	template <>
	struct IE_ENUM_FLAG_INTEGER_FOR_SIZE<1>
	{
		typedef Int8 type;
	};

	template <>
	struct IE_ENUM_FLAG_INTEGER_FOR_SIZE<2>
	{
		typedef Int16 type;
	};

	template <>
	struct IE_ENUM_FLAG_INTEGER_FOR_SIZE<4>
	{
		typedef Int32 type;
	};

	template <>
	struct IE_ENUM_FLAG_INTEGER_FOR_SIZE<8>
	{
		typedef Int64 type;
	};
	// used as an approximation of std::underlying_type<T>
	template <class T>
	struct IE_ENUM_FLAG_SIZED_INTEGER
	{
		typedef typename IE_ENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
	};
#define IE_DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
extern "C++" { \
inline _ENUM_FLAG_CONSTEXPR ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) | ((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE& operator |= (ENUMTYPE& a, ENUMTYPE b) throw() { return (ENUMTYPE&)(((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type&)a) |= ((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline _ENUM_FLAG_CONSTEXPR ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) & ((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE& operator &= (ENUMTYPE& a, ENUMTYPE b) throw() { return (ENUMTYPE&)(((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type&)a) &= ((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline _ENUM_FLAG_CONSTEXPR ENUMTYPE operator ~ (ENUMTYPE a) throw() { return ENUMTYPE(~((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); } \
inline _ENUM_FLAG_CONSTEXPR ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) ^ ((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE& operator ^= (ENUMTYPE& a, ENUMTYPE b) throw() { return (ENUMTYPE&)(((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type&)a) ^= ((IE_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
}
}
