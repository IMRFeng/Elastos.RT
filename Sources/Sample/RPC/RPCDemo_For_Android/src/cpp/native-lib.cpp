#include <jni.h>
#include <string>
#include "Chat.h"
#include <android/log.h>
#include "ela_session.h"
#include "elastos.h"
#include "elapi.h"
#include "ElastosCore.h"
#include "CCarrierListener.h"
#include "CMessageListener.h"
#include "elastos/utility/etl/HashMap.h"

ICarrier* gCarrier = NULL;
IServiceManager* gServiceManager = NULL;

Elastos::Utility::Etl::HashMap<String, IChat*> gInterfaces;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved){
    ela_session_jni_onload(vm, reserved);
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_elastos_rpcdemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C" JNIEXPORT void JNICALL Java_com_elastos_rpcdemo_MainActivity_startCarrier(
        JNIEnv *env,
        jobject jobj,
        jstring path) {

        ECode ec = (ECode) _CCarrier_GetInstance(&gCarrier);
        if (FAILED(ec)) return;

        CCarrierListener* listener = new CCarrierListener(env, jobj);
        gCarrier->AddCarrierNodeListener(ICarrierListener::Probe(listener));

        const char* nativeString = env->GetStringUTFChars(path, nullptr);
        gCarrier->Start(String(nativeString));
        env->ReleaseStringUTFChars(path, nativeString);
}

extern "C" JNIEXPORT void JNICALL Java_com_elastos_rpcdemo_MainActivity_stopCarrier(
        JNIEnv *env,
        jobject jobj) {
    if (gCarrier == NULL) return;

    gCarrier->Stop();
    gCarrier->Release();
    gCarrier = NULL;
}

extern "C" JNIEXPORT jint JNICALL Java_com_elastos_rpcdemo_MainActivity_addFriend(
        JNIEnv *env,
        jobject jobj,
        jstring uid) {
    if (gCarrier == NULL) return -1;

    const char* nativeString = env->GetStringUTFChars(uid, nullptr);
    ECode ec = gCarrier->AddFriend(String(nativeString), String("hello"));
    env->ReleaseStringUTFChars(uid, nativeString);
    return ec;
}

extern "C" JNIEXPORT jint JNICALL Java_com_elastos_rpcdemo_MainActivity_deleteFriend(
        JNIEnv *env,
        jobject jobj,
        jstring uid) {
    if (gCarrier == NULL) return -1;

    const char* nativeString = env->GetStringUTFChars(uid, nullptr);
    IFriend* pFriend;
    ECode ec = gCarrier->GetFriend(String(nativeString), &pFriend);
    if (FAILED(ec)) {
        env->ReleaseStringUTFChars(uid, nativeString);
        return ec;
    }

    ec = gCarrier->RemoveFriend(pFriend);
    pFriend->Release();
    env->ReleaseStringUTFChars(uid, nativeString);
    return ec;
}

extern "C" JNIEXPORT jobject JNICALL Java_com_elastos_rpcdemo_MainActivity_getFriendList(
        JNIEnv *env,
        jobject jobj) {
    if (gCarrier == NULL) return NULL;

    ArrayOf<IFriend*>* friends = NULL;
    ECode ec = gCarrier->GetFriends(&friends);
    if (FAILED(ec) || friends == NULL) return NULL;

    jclass cls_ArrayList = env->FindClass("java/util/ArrayList");
    jmethodID construct = env->GetMethodID(cls_ArrayList, "<init>", "()V");
    jobject obj_ArrayList = env->NewObject(cls_ArrayList, construct,"");
    jmethodID arrayList_add = env->GetMethodID(cls_ArrayList, "add", "(Ljava/lang/Object;)Z");

    jclass cls_friend = env->FindClass("Friend");
    if (env->ExceptionCheck()) {
        __android_log_print(ANDROID_LOG_DEBUG, "RPCDemo", "find class Friend exception");
        env->ExceptionClear();
        friends->Release();
        return NULL;
    }
    //none argument construct function
    jmethodID construct_user = env->GetMethodID(cls_friend, "<init>", "(Ljava/lang/String;Z)V");
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        friends->Release();
        return NULL;
    }

    for (int i = 0; i < friends->GetLength(); i++) {
        IFriend* iFriend = (*friends)[i];

        String uid;
        iFriend->GetUid(&uid);
        Boolean online;
        iFriend->IsOnline(&online);
        jobject obj_friend = env->NewObject(cls_friend, construct_user, env->NewStringUTF(uid.string()), online);
        env->CallObjectMethod(obj_ArrayList, arrayList_add, obj_friend);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            friends->Release();
            return NULL;
        }
    }

    friends->Release();

    return obj_ArrayList;
}


extern "C" JNIEXPORT jint JNICALL Java_com_elastos_rpcdemo_MainActivity_acceptFriend(
        JNIEnv *env,
        jobject jobj,
        jstring uid) {
    if (gCarrier == NULL) return -1;

    const char* nativeString = env->GetStringUTFChars(uid, nullptr);
    __android_log_print(ANDROID_LOG_DEBUG, "JNI", "accept friend %s", nativeString);
    ECode ec = gCarrier->AccpetFriendRequest(String(nativeString));
    env->ReleaseStringUTFChars(uid, nativeString);
    return ec;
}

extern "C" JNIEXPORT jstring JNICALL Java_com_elastos_rpcdemo_MainActivity_getAddress(
        JNIEnv *env,
        jobject jobj) {
    if (gCarrier == NULL) return env->NewStringUTF("");

    String address;
    gCarrier->GetAddress(&address);
    return env->NewStringUTF(address.string());
}

extern "C" JNIEXPORT void JNICALL Java_com_elastos_rpcdemo_MainActivity_startServiceManager(
        JNIEnv *env,
        jobject jobj) {
    if (gCarrier == NULL) return;
    if (gServiceManager != NULL) {
        return;
    }

    _CServiceManager_AcquireInstance(&gServiceManager);
}

extern "C" JNIEXPORT void JNICALL Java_com_elastos_rpcdemo_MainActivity_stopServiceManager(
        JNIEnv *env,
        jobject jobj) {
    if (gServiceManager) {
        gServiceManager->Release();
        gServiceManager = NULL;
    }
}

extern "C" JNIEXPORT jint JNICALL Java_com_elastos_rpcdemo_MainActivity_addService(
        JNIEnv *env,
        jobject jobj,
        jstring name) {
    if (gServiceManager == NULL) {
        __android_log_print(ANDROID_LOG_DEBUG, "JNI", "service manager not init");
        return -1;
    }

    IChat* chat;
    ECode ec = CChat::New(&chat);
    if (FAILED(ec)) return ec;

    CMessageListener* listener = new CMessageListener(env, jobj);
    chat->SetMessageListener(IMessageListener::Probe(listener));

    const char* nativeString = env->GetStringUTFChars(name, nullptr);
    __android_log_print(ANDROID_LOG_DEBUG, "JNI", "add service %s", nativeString);
    ec = gServiceManager->AddService(String(nativeString), chat);
    env->ReleaseStringUTFChars(name, nativeString);
    chat->Release();
    return ec;
}

extern "C" JNIEXPORT jint JNICALL Java_com_elastos_rpcdemo_MainActivity_getService(
        JNIEnv *env,
        jobject jobj,
        jstring uid,
        jstring name) {
    if (gServiceManager == NULL) return -1;

    IInterface* pInterface;
    const char* uidstr = env->GetStringUTFChars(uid, nullptr);
    const char* namestr = env->GetStringUTFChars(name, nullptr);
    ECode ec = gServiceManager->GetService(String(uidstr), String(namestr), &pInterface);
    env->ReleaseStringUTFChars(name, namestr);
    if (FAILED(ec)) {
        env->ReleaseStringUTFChars(uid, uidstr);
        return ec;
    }


    __android_log_print(ANDROID_LOG_DEBUG, "RPCDdemo", "pInterface: %p", pInterface);

    IChat* pChat = IChat::Probe(pInterface);
    if (pChat == NULL) {
        __android_log_print(ANDROID_LOG_DEBUG, "RPCDdemo", "probe IChat failed");
        env->ReleaseStringUTFChars(uid, uidstr);
        pInterface->Release();
        return -1;
    }

    gInterfaces[String(uidstr)] = pChat;
    env->ReleaseStringUTFChars(uid, uidstr);
    return NOERROR;
}

extern "C" JNIEXPORT jint JNICALL Java_com_elastos_rpcdemo_MainActivity_sendMessage(
        JNIEnv *env,
        jobject jobj,
        jstring uid,
        jstring msg) {
    if (gServiceManager == NULL) return -1;

    const char* uidstr = env->GetStringUTFChars(uid, nullptr);
    IChat* pChat = gInterfaces[String(uidstr)];
    __android_log_print(ANDROID_LOG_DEBUG, "RPCDemo", "=== get IChat: %p", pChat);
    if (pChat == NULL) {
        env->ReleaseStringUTFChars(uid, uidstr);
        return -1;
    }
    const char* msgstr = env->GetStringUTFChars(msg, nullptr);
    __android_log_print(ANDROID_LOG_DEBUG, "RPCDemo", "=== call IChat send msgstr: %s", msgstr);
//    pChat->AddRef();
    ECode ec = pChat->Send(String(msgstr));
    env->ReleaseStringUTFChars(uid, uidstr);
    env->ReleaseStringUTFChars(msg, msgstr);
    return 0;
}

