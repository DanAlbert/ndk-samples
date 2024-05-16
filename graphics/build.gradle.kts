plugins {
    id("ndksamples.android.application")
    id("ndksamples.android.kotlin")
}

android {
    namespace = "com.android.ndk.samples.graphics"

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
