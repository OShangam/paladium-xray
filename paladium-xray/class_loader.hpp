#pragma once

#include <jni.h>

class ClassLoader {
public:
    virtual ~ClassLoader() {}

    virtual jclass find_class(const char* class_name) = 0;
};