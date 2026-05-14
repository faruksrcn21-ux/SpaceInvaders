cmake_minimum_required(VERSION 3.10)
project(SpaceInvaders)

# C++ Standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML'i DLL olmadan, direkt exe'nin içine gömüyoruz
set(BUILD_SHARED_LIBS OFF)

# SFML'i otomatik indirme bölümü
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

# Kaynak dosyalarımız
add_executable(SpaceInvaders main.cpp GameManager.cpp player.cpp enemy.cpp bullet.cpp barrier.cpp explosion.cpp SoundManager.cpp)

# SFML modüllerini bağla
target_link_libraries(SpaceInvaders sfml-graphics sfml-window sfml-system sfml-audio)

# Windows için ek linker seçenekleri
if(WIN32)
    target_link_libraries(SpaceInvaders winmm gdi32 opengl32 user32 shell32 ws2_32)
endif()

# assets klasörünü çalıştırılabilir dosyanın yanına kopyala
add_custom_command(TARGET SpaceInvaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:SpaceInvaders>/assets"
    COMMENT "Kopyalama: assets -> target dizini"
)

# SFML'in ihtiyaç duyduğu openal32.dll'i çalıştırılabilir dosyanın yanına kopyala
if(WIN32)
    add_custom_command(TARGET SpaceInvaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/bin/x64/openal32.dll"
            "$<TARGET_FILE_DIR:SpaceInvaders>/"
        COMMENT "Kopyalama: openal32.dll -> target dizini"
    )
endif()