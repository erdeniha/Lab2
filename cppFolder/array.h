#pragma once

#include <string>

struct Array {
    std::string* data;
    int size;
    int capacity;
};

void initArray(Array* arr);
void freeArray(Array* arr);
void arrayPush(Array* arr, const std::string& value);
bool arrayGet(const Array* arr, int index, std::string& result);
int arrayLength(const Array* arr);
