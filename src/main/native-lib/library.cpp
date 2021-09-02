#include "library.h"

#include <cstdint>
#include <vector>
#include <exception>
#include <string>
#include <cstring>

#include <unordered_map>
#include <tuple>

using std::vector;
using std::string;

using std::unordered_map;

using std::tuple;

using std::strncpy;

const u_int8_t TAG_CONSTANT_Utf8 = 1;
const u_int8_t TAG_CONSTANT_Integer = 3;
const u_int8_t TAG_CONSTANT_Float = 4;
const u_int8_t TAG_CONSTANT_Long = 5;
const u_int8_t TAG_CONSTANT_Double = 6;
const u_int8_t TAG_CONSTANT_Class = 7;
const u_int8_t TAG_CONSTANT_String = 8;
const u_int8_t TAG_CONSTANT_Fieldref = 9;
const u_int8_t TAG_CONSTANT_Methodref = 10;
const u_int8_t TAG_CONSTANT_InterfaceMethodref = 11;
const u_int8_t TAG_CONSTANT_NameAndType = 12;
const u_int8_t TAG_CONSTANT_MethodHandle = 15;
const u_int8_t TAG_CONSTANT_MethodType = 16;
const u_int8_t TAG_CONSTANT_Dynamic = 17;
const u_int8_t TAG_CONSTANT_InvokeDynamic = 18;
const u_int8_t TAG_CONSTANT_Module = 19;
const u_int8_t TAG_CONSTANT_Package = 20;

unordered_map<uint8_t, uint8_t> BLOCK_SIZE = {
        {TAG_CONSTANT_Utf8,              -1},
        {TAG_CONSTANT_Integer,            4},
        {TAG_CONSTANT_Float,              4},
        {TAG_CONSTANT_Long,               8},
        {TAG_CONSTANT_Double,             8},
        {TAG_CONSTANT_Class,             -1},
        {TAG_CONSTANT_String,             2},
        {TAG_CONSTANT_Fieldref,           4},
        {TAG_CONSTANT_Methodref,          4},
        {TAG_CONSTANT_InterfaceMethodref, 4},
        {TAG_CONSTANT_NameAndType,        4},
        {TAG_CONSTANT_MethodHandle,       3},
        {TAG_CONSTANT_MethodType,         2},
        {TAG_CONSTANT_Dynamic,            4},
        {TAG_CONSTANT_InvokeDynamic,      4},
        {TAG_CONSTANT_Module,             2},
        {TAG_CONSTANT_Package,            2}
};


tuple<uint16_t, const char*> get_class_name_info(const jbyte* bytes) {
    // https://docs.oracle.com/javase/specs/jvms/se16/html/jvms-4.html#jvms-4.4.1
    auto ptr = bytes + 8 * sizeof(uint8_t);

    auto cp_count = be16toh(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += sizeof(cp_count);

    vector<const jbyte*> entries;
    vector<int> class_infos;

    for (int i = 0; i < cp_count - 1; i++) {
        entries.push_back(ptr);

        auto tag = *reinterpret_cast<const uint8_t*>(ptr);
        ptr += sizeof(tag);

        int offset;

        if (tag == TAG_CONSTANT_Class) {
            auto name_index = be16toh(*reinterpret_cast<const uint16_t*>(ptr));
            offset = sizeof(name_index);
            class_infos.push_back(name_index);
        } else if (tag == TAG_CONSTANT_Utf8) {
            auto length = be16toh(*reinterpret_cast<const uint16_t *>(ptr));
            ptr += sizeof(length);
            offset = length;
        } else {
            offset = BLOCK_SIZE[tag];
        }

        if (offset == -1)
            throw std::exception();

        ptr += offset;
    }

    auto this_class_index = be16toh(*reinterpret_cast<const uint16_t*>(ptr + sizeof(uint16_t)));

    ptr = entries[this_class_index - 1];
    ptr += sizeof(uint8_t);

    auto class_reference_index = be16toh(*reinterpret_cast<const uint16_t*>(ptr));

    ptr = entries[class_reference_index - 1];
    ptr += sizeof(uint8_t);

    auto constant_length = be16toh(*reinterpret_cast<const uint16_t *>(ptr));
    ptr += sizeof(constant_length);

    return { constant_length, reinterpret_cast<const char *>(ptr) };
}

void Java_com_example_JNILoaderUtilsKt_defineClass(JNIEnv *env, jobject thisObject, jbyteArray bytes) {
    jbyte* class_bytes = env->GetByteArrayElements(bytes, nullptr);

    auto [length, class_name] = get_class_name_info(class_bytes);

    strncpy(const_cast<char*>(class_name), "wtf", 3);

    char* null_terminated_class_name = new char[length + 1];
    strncpy(null_terminated_class_name, class_name, length);
    null_terminated_class_name[length] = 0;

    jclass this_class = env->GetObjectClass(thisObject);
    jmethodID get_class_loader_method_id = env->GetMethodID(this_class, "getClassLoader","()Ljava/lang/ClassLoader;");
    jobject class_loader = env->CallObjectMethod(thisObject, get_class_loader_method_id);

    env->DefineClass(null_terminated_class_name, class_loader, class_bytes, env->GetArrayLength(bytes));

    delete[] null_terminated_class_name;
}
