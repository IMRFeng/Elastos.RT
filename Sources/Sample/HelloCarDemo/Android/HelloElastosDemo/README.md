> ## 1.简介

此示例展示了如何在 Android 平台上调用 Elastos Runtime 以及 Elastos 第三方构件（库）。开发者可以根据该例去熟悉在 Android 平台上使用 Elastos 接口的流程，并学习开发自己的 Elastos 构件（库）。

参考示例:

android app: ~/Elastos.RT/Sources/Sample/HelloCarDemo/Android/HelloElastosDemo

elastos Car Demo: ~/Elastos.RT/Sources/Sample/HelloCarDemo/Car

> ## 2.原理

欲要在 android app 中使用在elastos上开发的构件，首先需要在elastos上进行该构件的创建和开发；其次，完成编译后导出相关的.so和头文件；然后，android app中加载elastos相关的库，声明native方法同时在jni文件声明与native方法对应的C函数，native方法调用这个C函数，而在这个C函数中调用car构件中的方法。通过这个方式即可实现在android app中调用car构件。

> ## 3.操作步骤
> ### 3.1.编写 car 文件

在Sources文件下创建一个自己的代码文件。例如: ~/Elastos.RT/Sources/Sample/test/，然后在该目录下编写一个Car文件。

下面请看范例 CAR 文件(Elastos.HelloCarDemo.car)内容:

``` cpp
module
{
    importlib("Elastos.CoreLibrary");

    interface IHelloCar {
        Hello(
            [out] String* name);
    }

    class CHelloCar {
        interface IHelloCar;
    }

}
```

示例 Elastos.HelloCarDemo.car 文件中，定义了接口 IHelloCar 和类 CHelloCar。IHelloCar 接口提供 Hello 方法；类 CHelloCar 实现接口 IHelloCar。

> ### 3.2.编译Car文件

在本示例中，执行下面语句:

``` shell
emake Elastos.HelloCarDemo.car
```

通过该命令可以自动生成代码框架。如 CHelloCar.h、 CHelloCar.cpp、 CMakeLists.txt 以及 Elastos.HelloCarDemo.cmake 文件。
一般情况下，CMakeLists.txt、Elastos.HelloCarDemo.cmake 文件是不需要修改的。

car文件中定义的每个类都会对应生成一个该类的.h和.cpp文件：

其中，.h文件中包含了该构件类中需要实现的所有接口（回调接口除外）方法的声明；.cpp文件则包含了这些方法的实现框架。在默认情况下，方法返回E_NOT_IMPLEMENTED，表示方法都未实现。

在头文件中可以添加构件类的私有方法，并在C++文件中添加其的具体实现。

> ### 3.3.填入实现代码

在生成的文件框架中，方法返回的 ECode 值为 E_NOT_IMPLEMENTED，表示此方法未实现。在填写完实现代码后，需要修改为适当的 ECode 值。

下面请看构件的实现 CHelloCar.cpp 文件:

``` cpp
#include "CHelloCar.h"

CAR_OBJECT_IMPL(CHelloCar)

CAR_INTERFACE_IMPL(CHelloCar, Object, IHelloCar);

ECode CHelloCar::Hello(
    /* [out] */ String * pName)
{
    VALIDATE_NOT_NULL(pName)
    *pName = "Elastos";
    return NOERROR;
}
```

> ### 3.4.编译和集成

1. Compile android-version RT

    ```shell
    source SetEnv.sh arm_android_32
    emake
    ```

2. Integrate elastos library information and third-party library information into android app

* integrate elastos library

    ``` shell
    cd ~/Elastos.RT/Sources/Sample/HelloCarDemo/Android/HelloElastosDemo
    dropsdk --output-dir app --lib-subdir armeabi-v7a
    ```

* integrate Car component header and library

    ``` shell
    # 拷贝car构件so和头文件
    cd ~/Elastos.RT/Sources/Sample/HelloCarDemo/Car
    pd m
    cp Elastos.HelloCarDemo.h ~/Elastos.RT/Sources/Sample/HelloCarDemo/Android/HelloElastosDemo/app/elastos/include
    pd @
    cp libElastos.HelloCarDemo.so ~/Elastos.RT/Sources/Sample/HelloCarDemo/Android/HelloElastosDemo/app/elastos/libs/armeabi-v7a/
    ```

> ### 3.5.android 调用 car 构件

    接下来，只列举涉及到elastos调用的代码是如何编写的。

1. 新建一个文件TestUtils.java

    该文件包含两个内容：一，加载Elastos.RT的库、加载作为demo示例的库信息（在这里作为第三方库来看待）、加载实现调用helloCarDemo中car构件的jni文件；二，声明native方法，并定义了另外一个Java方法来调用这个native方法。

    路径：app/src/main/java/org/elastos/runtime/sample/helloelastos/TestUtils.java

    ``` java
    static {
        System.loadLibrary("Elastos.Runtime");
        System.loadLibrary("Elastos.CoreLibrary");
        System.loadLibrary("Elastos.HelloCarDemo");
        System.loadLibrary("testThirdLib");
    }

    private static native String nativeGetHelloCarString();

    public String getHelloCarString() {
        return nativeGetHelloCarString();
    }
    ```

2. jni文件内容

    app/src/main/cpp/elastos_testthirdlib.cpp，在nativeGetHelloCarString函数中，new了一个CHelloCar对象，然后通过调用接口中的函数Hello，实现了在android中调用Car构件。

    ``` cpp
    //For Elastos
    #include "Elastos.HelloCarDemo.h"

    #define JNIREG_CLASS "org/elastos/runtime/sample/helloelastos/TestUtils"

    static jstring JNICALL nativeGetHelloCarString(JNIEnv *env, jobject jobj)
    {
        AutoPtr<IHelloCar> helloCarObj;
        ECode ec = CHelloCar::New((IHelloCar**)&helloCarObj);
        if (ec != NOERROR || helloCarObj == NULL) {
            return 0;
        }
        String helloString;

        helloCarObj->Hello(&helloString);

        String newString("[native] Hello, ");
        newString += helloString + "!";

        return env->NewStringUTF(newString.string());
    }

    static const JNINativeMethod gMethods[] = {
        {"nativeGetHelloCarString", "()Ljava/lang/String;", (void*)nativeGetHelloCarString}
    };

    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){
        JNIEnv * env;
        jclass cls;
        jint result = -1;
        if(vm->GetEnv((void **)&env,JNI_VERSION_1_6) != JNI_OK){
            return JNI_ERR;
        }

        cls = env->FindClass(JNIREG_CLASS);
        env->RegisterNatives(cls, gMethods, sizeof(gMethods)/sizeof(JNINativeMethod));
        return JNI_VERSION_1_6;
    }
    ```

3. MainActivity内容

    app/src/main/java/org/elastos/runtime/sample/helloelastos/MainActivity.java，具体的调用如下：

    ``` java
    //Show the string that comes from a Elastos car objects[CHelloCar]
    TextView textLabel = (TextView)findViewById(R.id.sample_text);
    TestUtils tu = new TestUtils();
    String helloString = tu.getHelloCarString();
    textLabel.setText(helloString);
    ```

4. 修改 app/CMakeLists.txt

    把 `native-lib` 改为 `testThirdLib`， 把 `src/main/cpp/native-lib.cpp` 改为 `src/main/cpp/elastos_testthirdlib.cpp`

    添加
    ```
    include_directories(elastos/include)
    link_directories(libs/armeabi-v7a)
    ```

    添加 `Elastos.Runtime` 和 `Elastos.Runtime_static` 到 target_link_libraries

5. 修改 app/build.gradle

    ```
    externalNativeBuild {
        cmake {
            cppFlags "-std=c++11", "-D_GNUC", "-D_linux", "-D_UNDEFDLLEXP", "-fvisibility=hidden"
        }
    }
    ndk {
        abiFilters 'armeabi-v7a'
    }
    ```

> ### 3.6.运行Android app

和普通android app运行一样，此处略。
