#include "array.h"

void initArray(Array* arr) {
    arr->capacity = 4;
    arr->size = 0;
    arr->data = new std::string[arr->capacity];
}

void freeArray(Array* arr) {
    delete[] arr->data;
    arr->data = nullptr;
    arr->size = 0;
}

static void growArray(Array* arr) {
    int newCapacity = arr->capacity * 2;
    std::string* newData = new std::string[newCapacity];
    for (int i = 0; i < arr->size; i++) {
        newData[i] = arr->data[i];
    }
    delete[] arr->data;
    arr->data = newData;
    arr->capacity = newCapacity;
}

void arrayPush(Array* arr, const std::string& value) {
    if (arr->size == arr->capacity) {
        growArray(arr);
    }
    arr->data[arr->size] = value;
    arr->size++;
}

bool arrayGet(const Array* arr, int index, std::string& result) {
    if (index < 0 || index >= arr->size) {
        return false;
    }
    result = arr->data[index];
    return true;
}

int arrayLength(const Array* arr) {
    return arr->size;
}
