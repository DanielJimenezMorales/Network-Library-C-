#pragma once

/// <summary>
/// The Safe Pointer is a wrapper that ensures safe usage of raw pointers concerning const-correctness.
/// When you have a pointer as a member variable and want to ensure that within const member functions,
/// only the const methods of the pointer are accessible, use a SafePointer.
/// This approach turns your member variable into an object rather than a raw pointer, thereby enforcing
/// const-correctness.
/// </summary>
/// <typeparam name="T">Raw pointer type</typeparam>
template < typename T >
class SafePointer
{
	public:
		SafePointer( T* rawPointer )
		    : _rawPointer( rawPointer )
		{
		}

		SafePointer( const SafePointer< T >& other )
		    : _rawPointer( other._rawPointer )
		{
		}

		T& operator*() { return *_rawPointer; }

		const T& operator*() const { return *_rawPointer; }

		T* operator->() { return _rawPointer; }

		const T* operator->() const { return _rawPointer; }

		bool operator==( const T* other ) const { return _rawPointer == other; }

		bool operator!=( const T* other ) const { return !( *this == other ); }

		bool operator==( const SafePointer< T >& other ) const { return _rawPointer == other._rawPointer; }

		bool operator!=( const SafePointer< T >& other ) const { return !( *this == other ); }

		T* GetRawPointer();
		const T* GetRawPointer() const;
		void Set( T* new_ptr );

	private:
		T* _rawPointer;
};

template < typename T >
inline T* SafePointer< T >::GetRawPointer()
{
	return _rawPointer;
}

template < typename T >
inline const T* SafePointer< T >::GetRawPointer() const
{
	return _rawPointer;
}

template < typename T >
inline void SafePointer< T >::Set( T* new_ptr )
{
	_rawPointer = new_ptr;
}
