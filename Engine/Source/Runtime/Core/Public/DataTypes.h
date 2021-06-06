/*
	Header - Primative_Types.h
	Source - None

	Author - Garrett Courtney

	Description:
	Represents primative data types used for mathmatical operations.

*/
#pragma once

#include <Runtime/Core.h>


namespace Insight
{
	/*
		32-bit floating point value.
	*/
	typedef float Float;

	/*
		64-bit double precision floating point value.
	*/
	typedef double Double;

	/*
		8-bit unsigned integer.
	*/
	typedef unsigned char UInt8;

	/*
		16-bit unsigned integer.
	*/
	typedef unsigned short UInt16;

	/*
		32-bit unsigned integer.
	*/
	typedef unsigned long UInt32;

	/*
		64-bit unsigned integer.
	*/
	typedef unsigned long long UInt64;

	/*
		8-bit signed integer.
	*/
	typedef signed char Int8;

	/*
		16-bit signed integer.
	*/
	typedef short Int16;

	/*
		32-bit signed integer.
	*/
	typedef long Int32;

	/*
		64-bit signed integer.
	*/
	typedef long long Int64;

	typedef std::shared_ptr<std::vector<UInt8>> ByteArray;
	
	/*
		A byte array whos memory is reference counted.
	*/
	struct DataBlob
	{
		friend class FileSystem;
	public:
		DataBlob();
		~DataBlob() = default;

		inline bool IsValid() const
		{
			return m_ByteArray.get() && m_DataSize > 0;
		}
		inline void Invalidate()
		{
			m_ByteArray.reset();
			m_ByteArray = NULL;
			m_DataSize = -1;
		}
		inline std::vector<UInt8>* operator->()
		{
			return m_ByteArray.get();
		}
		inline UInt8* GetBufferPointer() const
		{
			return m_ByteArray.get()->data();
		}
		inline size_t GetDataSize() const
		{
			return m_DataSize;
		}
	private:
		ByteArray m_ByteArray;
		size_t m_DataSize;
	};


#ifdef UNICODE
	typedef wchar_t TChar;
	typedef ::std::wstring FString;
#else
	typedef char TChar;
	typedef ::std::string FString;
#endif // UNICODE

	typedef wchar_t WChar;
	typedef char Char;

	template <typename Type>
	inline FString ToString(Type Val)
	{
		return
#ifdef UNICODE
			::std::to_wstring(Val);
#else
			::std::to_string(Val);
#endif // UNICODE
	}
}
// eof
