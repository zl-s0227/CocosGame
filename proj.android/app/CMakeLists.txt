#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.6)

set(APP_NAME CocosGame)

project(${APP_NAME})

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0)
    endif()
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)
set(GAME_ROOT ../../../CocosGame)

set(GAME_RES_FOLDER
        "${GAME_ROOT}/Resources"
        )

# add cross-platforms source files and header files
list(APPEND GAME_SOURCE
        jni/CocosGame.cpp
        ${GAME_ROOT}/Classes/AppDelegate.cpp
        ${GAME_ROOT}/Classes/RootScene.cpp
        ${GAME_ROOT}/Classes/Games/GameFactory.cpp
        ${GAME_ROOT}/Classes/Games/SnacksGame.cpp
        ${GAME_ROOT}/Classes/Games/Control.cpp
        ${GAME_ROOT}/Classes/Games/GameObject/SnackSprite.cpp
        ${GAME_ROOT}/Classes/Games/GameObject/SnackFoods.cpp
        )

list(APPEND GAME_HEADER
        ${GAME_ROOT}/Classes/AppDelegate.h
        ${GAME_ROOT}/Classes/RootScene.h
        ${GAME_ROOT}/Classes/Games/GameFactory.h
        ${GAME_ROOT}/Classes/Games/SnacksGame.h
        ${GAME_ROOT}/Classes/Games/Control.h
        ${GAME_ROOT}/Classes/Games/GameDefs.h
        ${GAME_ROOT}/Classes/Games/GameObject/SnackSprite.h
        ${GAME_ROOT}/Classes/Games/GameObject/SnackFoods.h
        )

# mark app complie info and libs info
set(all_code_files
        ${GAME_HEADER}
        ${GAME_SOURCE}
        )

link_directories(${GAME_ROOT}/lib/android/Release)
add_library(${APP_NAME} SHARED ${all_code_files})
target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)

set(DEPEND_LIBS)
list(APPEND DEPEND_LIBS
        cocos2d
        ext_cpufeatures
        ext_clipper
        ext_convertUTF
        ext_edtaa3func
        ext_md5
        ext_poly2tri
        ext_pvmp3dec
        ext_recast
        ext_tinyxml2
        ext_tremolo
        ext_unzip
        ext_xxhash
        ext_xxtea
        external
        )

list(APPEND DEPEND_LIBS
        box2d
        BulletDynamics
        BulletCollision
        BulletMultiThreaded
        LinearMath
        MiniCL
        chipmunk
        curl
        freetype
        jpeg
        crypto
        ssl
        png
        uv_a
        webp
        websockets
        z
        )

list(APPEND DEPEND_LIBS
        dl
        GLESv2
        EGL
        log
        android
        OpenSLES
        atomic
        m
        )
target_link_libraries(${APP_NAME} ${DEPEND_LIBS})

set(INCLUDE_ROOT_PATH ../../include)
target_include_directories(${APP_NAME}
        PRIVATE ${GAME_ROOT}/Classes
        PRIVATE ${INCLUDE_ROOT_PATH}
        PRIVATE ${INCLUDE_ROOT_PATH}/cocos
        PRIVATE ${INCLUDE_ROOT_PATH}/extensions/GUI
        )

# mark app resources
#setup_cocos_app_config(${APP_NAME})