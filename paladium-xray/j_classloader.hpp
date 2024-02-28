#pragma once

#include "class_loader.hpp"

class JClassLoader : public ClassLoader {
public:
    JClassLoader(JNIEnv* env, jobject class_loader);
    virtual ~JClassLoader();

    virtual jclass find_class(const char* class_name) override;
private:
    JNIEnv* env;
    jobject classloader;
    jmethodID m_findClass;
};