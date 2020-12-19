#pragma once

template <typename T>
SafePointer<T>::SafePointer() { }

template <typename T>
SafePointer<T>::SafePointer(T handle, DisposeCallback<T> disposer) {
	this->handle = handle;
	this->disposer = disposer;
}

template <typename T>
void SafePointer<T>::dispose() {
	disposer(handle);
	disposed = true;
}

template <typename T>
SafePointer<T>::~SafePointer() {
	if (disposed) { return; }
	disposer(handle);
}
