#include "j_classloader.hpp"
#include <Windows.h>

JClassLoader::JClassLoader(JNIEnv* env, jobject classloader)
{
    this->env = env;
    this->classloader = classloader;

    jclass c_urlclassloader = env->FindClass("java/net/URLClassLoader");

    this->m_findClass = env->GetMethodID(c_urlclassloader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    env->DeleteLocalRef(c_urlclassloader);
}

JClassLoader::~JClassLoader()
{
    env->DeleteLocalRef(this->classloader);
}

jclass JClassLoader::find_class(const char* class_name)
{
    jclass clazz = nullptr;
    jstring j_class_name = env->NewStringUTF(class_name);

    clazz = reinterpret_cast<jclass>(env->CallObjectMethod(this->classloader, m_findClass, j_class_name));
    env->DeleteLocalRef(j_class_name);

    return clazz;
}