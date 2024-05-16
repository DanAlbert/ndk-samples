plugins {
    id("ndksamples.android.application")
    id("ndksamples.android.kotlin")
}

android {
    namespace = "com.android.ndk.samples.graphics"

    defaultConfig {
        // AChoreographer was first introduced in API 24. This sample could be
        // ported to older minSdkVersions by using Choreographer via JNI, but
        // minSdkVersion 24 (or higher) is a pretty sensible choice for a new
        // app.
        minSdk = 24;
    }

    externalNativeBuild {
        cmake {
            path("src/main/cpp/CMakeLists.txt")
        }
    }

    buildFeatures {
        prefab = true
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.games.gameactivity)
    implementation(libs.appcompat)
    implementation(libs.material)
}
