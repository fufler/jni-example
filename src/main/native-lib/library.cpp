#include "library.h"

#include <cstdint>
#include <iostream>
#include <vector>
#include <exception>
#include <string>
#include <cstring>

using std::cout;
using std::endl;
using std::hex;
using std::dec;

using std::vector;
using std::string;

using std::strncpy;

void Java_com_example_JNILoaderUtilsKt_defineClass(JNIEnv *env, jobject thisObject, jbyteArray bytes) {
    jbyte* class_bytes = env->GetByteArrayElements(bytes, nullptr);

    auto ptr = class_bytes;

    auto magic = *reinterpret_cast<uint32_t*>(ptr);
    ptr += sizeof(magic);

    cout << "Magic: " << hex << htobe32(magic) << dec << endl;

    auto minor_version = be16toh(*reinterpret_cast<uint16_t*>(ptr));
    ptr += sizeof(minor_version);

    auto major_version = be16toh(*reinterpret_cast<uint16_t*>(ptr));
    ptr += sizeof(major_version);

    cout << "Major version: " << major_version << endl;
    cout << "Minor version: " << minor_version << endl;

    auto cp_count = be16toh(*reinterpret_cast<uint16_t*>(ptr));
    ptr += sizeof(cp_count);

    cout << "Constant pool count: " << cp_count << endl;

    vector<jbyte*> entries;
    vector<int> class_infos;

    for (int i = 0; i < cp_count - 1; i++) {
        entries.push_back(ptr);

        auto tag = *reinterpret_cast<uint8_t*>(ptr);
        ptr += sizeof(tag);

        int offset = -1;

        if (tag == 7) {
            auto name_index = be16toh(*reinterpret_cast<uint16_t*>(ptr));
            ptr += sizeof(name_index);
            class_infos.push_back(name_index);
        } else {
            switch (tag) {
                case 9:
                case 10:
                case 11:
                    offset = 4;
                    break;
                case 8:
                    offset = 2;
                    break;
                case 3:
                case 4:
                    offset = 4;
                    break;
                case 5:
                case 6:
                    offset = 8;
                    break;
                case 12:
                    offset = 4;
                    break;
                case 15:
                    offset = 3;
                    break;
                case 16:
                    offset = 2;
                    break;
                case 17:
                    offset = 4;
                    break;
                case 1:
                    auto length = be16toh(*reinterpret_cast<uint16_t*>(ptr));
                    ptr += sizeof(length);
                    offset = length;
                    break;
            }

            if (offset == -1)
                throw std::exception();

            ptr += offset;
        }
    }

    for (const auto infoIndex: class_infos) {
        ptr = entries[infoIndex - 1];
        auto constant_length = be16toh(*reinterpret_cast<uint16_t *>(ptr + 1));
        ptr += sizeof(constant_length) + 1;

        string class_name(reinterpret_cast<char *>(ptr), constant_length);

        if (class_name == "com/example/ClassToBeLoadedByJNI") {
            strncpy(reinterpret_cast<char*>(ptr), "wtf", 3);
            break;
        }

        cout << "Class name: " << class_name << endl;
    }

    jclass this_class = env->GetObjectClass(thisObject);
    jmethodID get_class_loader_method_id = env->GetMethodID(this_class, "getClassLoader","()Ljava/lang/ClassLoader;");
    jobject class_loader = env->CallObjectMethod(thisObject, get_class_loader_method_id);

    env->DefineClass("wtf/example/ClassToBeLoadedByJNI", class_loader, class_bytes, env->GetArrayLength(bytes));
}
