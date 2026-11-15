#if defined(CATALYST_PLATFORM_ANDROID)
#pragma once

//Core.
#include <Core/Essential/CatalystEssential.h>

//Android utilities.
namespace AndroidUtilities
{

    /*
    *   Calls a void JNI method.
    */
    FORCE_INLINE static void CallVoidJNIMethod(const char *const RESTRICT method_name) NOEXCEPT
    {
        //Retrieve the native activity.
        ANativeActivity *native_activity{ CatalystPlatform::_App->activity };

        //Retrieve the Java VM.
        JavaVM *JVM{ native_activity->vm };

        //Retrieve the environment.
        JNIEnv *environment{ nullptr };

        JVM->GetEnv((void**)&environment, JNI_VERSION_1_6);

        //Attach the current thread.
        const jint result{ JVM->AttachCurrentThread(&environment, nullptr) };

        if (result == JNI_ERR)
        {
            PRINT_TO_OUTPUT("Failed to do JNI stuff!");

            return;
        }

        //Retrieve the Java class.
        jclass java_class{ environment->GetObjectClass(native_activity->clazz) };

        //Retrieve the Java method ID.
        jmethodID java_method_id{ environment->GetMethodID(java_class, method_name, "()V") };

        //Call the method!
        environment->CallVoidMethod(native_activity->clazz, java_method_id);

        //Detach the current thread.
        JVM->DetachCurrentThread();
    }

    /*
    *   Sets the current threads priority.
    */
    FORCE_INLINE static void SetThreaPriority(const int32 wanted_priority) NOEXCEPT
    {
        //Retrieve the native activity.
        ANativeActivity *native_activity{ CatalystPlatform::_App->activity };

        //Retrieve the Java VM.
        JavaVM *JVM{ native_activity->vm };

        //Retrieve the environment.
        JNIEnv *environment{ nullptr };

        JVM->GetEnv((void**)&environment, JNI_VERSION_1_6);

        //Attach the current thread.
        const jint result{ JVM->AttachCurrentThread(&environment, nullptr) };

        if (result == JNI_ERR)
        {
            PRINT_TO_OUTPUT("Failed to do JNI stuff!");

            return;
        }

        //Retrieve the Java class.
        jclass java_class{ environment->FindClass("android/os/Process") };

        //Retrieve the Java method ID.
        jmethodID java_method_id{ environment->GetStaticMethodID(java_class, "setThreadPriority", "(I)V") };

        //Call the method!
        environment->CallStaticVoidMethod(java_class, java_method_id, wanted_priority);

        //Detach the current thread.
        JVM->DetachCurrentThread();
    }

}
#endif