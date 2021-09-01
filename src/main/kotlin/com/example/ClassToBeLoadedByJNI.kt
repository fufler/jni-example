package com.example

class ClassToBeLoadedByJNI: HelloSayer {
    override fun sayHello() = "Hi there. I was loaded by JNI. My class name is ${javaClass.name}"
}