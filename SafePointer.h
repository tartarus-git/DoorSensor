#pragma once

template <typename T>
using DisposeCallback = void(*)(T handle);

template <typename T>
class SafePointer
{
	void copy(const SafePointer& other);
public:
	T handle;
	DisposeCallback<T> disposer;
	bool disposed = false;

	SafePointer();
	SafePointer(T handle, DisposeCallback<T> disposer);
	void dispose();
	~SafePointer();

	SafePointer(const SafePointer& other) = delete;
	SafePointer& operator =(const SafePointer& other) = delete;
};

#include "SafePointer.ipp"
