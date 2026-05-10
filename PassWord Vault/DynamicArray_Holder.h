#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

#include<new>
#include "Exceptions.h"

template<typename T>
class DynamicArray {
private:
	T* Array;
	int Size;
	int Capacity;
public:

	DynamicArray() : Size(0), Capacity(0), Array(nullptr) {};

	DynamicArray(T* a, int size,int capacity) : Size(size) , Capacity(capacity) {
		Array = new T[capacity];
		Copy_Data(a, size, Array);
	}

	DynamicArray(const DynamicArray& obj) {
		
		this->Capacity = obj.Capacity;
		this->Size = obj.Size;
		this->Array = new T[this->Capacity];

		Copy_Data(obj.Array, obj.Size, this->Array);
	
	}

	int Get_Size() {
		return Size;
	}

	DynamicArray& operator=(const DynamicArray& obj) {

		if (this == &obj)
			return *this;

		this->Capacity = obj.Capacity;
		this->Size = obj.Size;
		delete[] this->Array;
		this->Array = new T[this->Capacity];

		Copy_Data(obj.Array, obj.Size, this->Array);

		return *this;
	}

	T& operator[](int index) {
		return Array[index];
	}

	void AddEntry(const T& entry) {
	
		if (Size >= Capacity)
			resize();
		Array[Size] = entry;
		Size++;
	}

	void RemoveEntry(int index) {
		if (index < 0 || index >= Size) 
			return;
		
		if (index != Size - 1)
			custom_swap(Array[index], Array[Size - 1]);
		
		Size--;
	}

	void resize() {
		T* temp = Array;
		if (Capacity)
			Capacity *= 2;
		else
			Capacity = 1;
			
		Array = new(std::nothrow) T[Capacity];

		if (Array == nullptr) {
			throw Bad_Alloc("MEMORY ALLOC ERROR: Heap allocation failed in DynamicArray.", Capacity * sizeof(T));
		}

		Copy_Data(temp, Size, Array);
		delete[] temp;
	}

	void Copy_Data(const T* Source, int Source_Size, T* Target) {
		for (int i = 0; i < Source_Size; i++) {
			Target[i] = Source[i];
		}
	}

	~DynamicArray() {
		delete[] Array;
	}
};


//Using templates

template<typename T>
void custom_swap(T& a, T& b) {
	T temp = a;
	a = b;
	b = temp;
}

//keep8ing the indexse in range
template<typename T>
const T& custom_clamp(const T& val, const T& min, const T& max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

#endif // !DYNAMIC_ARRAY

