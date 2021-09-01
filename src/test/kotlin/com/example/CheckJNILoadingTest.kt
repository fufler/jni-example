package com.example

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Test

class CheckJNILoadingTest {
    @Test
    fun test() {
        assertEquals(
            "Hi there. I was loaded by JNI. My class name is wtf.example.ClassToBeLoadedByJNI",
            loadClassAndSayHello()
        )
    }
}
