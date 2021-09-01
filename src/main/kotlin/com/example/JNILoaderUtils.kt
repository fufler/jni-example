package com.example

private external fun defineClass(classBytes: ByteArray)

fun loadClassAndSayHello(): String {
    println(System.getProperty("java.library.path"))
    System.loadLibrary("native_lib");

    val classResource = (object {}).javaClass.getResourceAsStream("/com/example/ClassToBeLoadedByJNI.class")
    val classBytes = classResource.readBytes()
    defineClass(classBytes)

    val helloSayer = Class.forName("wtf.example.ClassToBeLoadedByJNI").newInstance() as HelloSayer

    return helloSayer.sayHello()
}

