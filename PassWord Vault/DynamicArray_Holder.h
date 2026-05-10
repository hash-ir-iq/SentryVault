#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

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


	void AddEntry(const T& entry) {
	
		if (Size >= Capacity)
			resize();
		Array[Size] = entry;
		Size++;
	}

	void resize() {
		T* temp = Array;
		if (Capacity)
			Capacity *= 2;
		else
			Capacity = 1;
		Array = new T[Capacity];

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

	T& operator[](int index) {
		return Array[index];
	}


};

#endif // !DYNAMIC_ARRAY

