#pragma once
#include <iostream>
using namespace std;

template <typename T>
class Vector
{
private:
	T* elements;
	size_t size;
	size_t maxSize;

	void Copy(const Vector<T>& v);
	void Delete();
	void Resize(size_t newSize);
public:
	Vector(size_t maxSize = 100);
	Vector(const Vector<T>& v);
	Vector<T>& operator= (const Vector<T>& v);
	~Vector();

	void Push(T elem);
	T& operator[] (size_t index);

    bool isEmpty()const;
	size_t Size() const;
};

template <typename T>
Vector<T>::Vector(size_t maxSize)
{
	this -> maxSize = maxSize;
	this -> elements = new T[this -> maxSize];
	this -> size = 0;
}

template <typename T>
Vector<T>::Vector(const Vector<T>& v)
{
	this -> Copy(v);
}

template <typename T>
Vector<T>& Vector<T>::operator= (const Vector<T>& v)
{
	if(this != &v)
	{
		this -> Delete();
		this -> Copy(v);
	}

	return *this;
}

template <typename T>
Vector<T>::~Vector()
{
	this -> Delete();
}

template <typename T>
void Vector<T>::Copy(const Vector<T>& v)
{
	this -> maxSize = v.maxSize;
	this -> size = v.size;
	this -> elements = new T[this -> maxSize];

	for(size_t i = 0; i < this -> size; ++i)
	{
		this -> elements[i] = v.elements[i];
	}
}

template <typename T>
void Vector<T>::Delete()
{
	delete[] this -> elements;
}

template <typename T>
void Vector<T>::Resize(size_t newSize)
{
	this -> maxSize = newSize;
	T* temp = new T[this -> maxSize];

	for(size_t i = 0; i < this -> size; ++i)
	{
		temp[i] = this -> elements[i];
	}

	T* del = this -> elements;
	this -> elements = temp;
	delete[] del;
}

template <typename T>
void Vector<T>::Push(T elem)
{
	if(this -> size == this -> maxSize)
	{
		this -> Resize(this -> maxSize*2);
	}

	this -> elements[this -> size] = elem;
	++(this -> size);
}

template <typename T>
T& Vector<T>::operator[] (size_t index)
{
	return this -> elements[index];
}

template <typename T>
size_t Vector<T>::Size() const
{
	return this -> size;
}


template<typename T>
bool Vector<T>::isEmpty() const
{
    return size == 0;
}
