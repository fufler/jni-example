#ifndef NATIVE_LIB_LIBRARY_H
#define NATIVE_LIB_LIBRARY_H

#include <jni.h>

extern "C" {
    JNIEXPORT void JNICALL Java_com_example_JNILoaderUtilsKt_defineClass(JNIEnv *env, jobject thisObject, jbyteArray bytes);
}

#endif //NATIVE_LIB_LIBRARY_H
