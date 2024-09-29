################################################################################
##
## Copyright (C) 2021-2022 Open Mobile Platform LLC.
## Contact: https://community.omprussia.ru/open-source
##
## This file is part of the Document Scanner project.
##
## Redistribution and use in source and binary forms,
## with or without modification, are permitted provided
## that the following conditions are met:
##
## * Redistributions of source code must retain the above copyright notice,
##   this list of conditions and the following disclaimer.
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer
##   in the documentation and/or other materials provided with the distribution.
## * Neither the name of the copyright holder nor the names of its contributors
##   may be used to endorse or promote products derived from this software
##   without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
## THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
## IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
## OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
## LOSS OF USE, DATA, OR PROFITS;
## OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
## WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
## EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
################################################################################

TEMPLATE = aux

OPENCV_PATH = $$OUT_PWD/opencv

opencv_build.commands = \
    cmake -S $$IN_PWD/opencv/ -B $$OPENCV_PATH \
    -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SKIP_RPATH=ON \
        -DBUILD_LIST=core,imgproc \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_DOCS=OFF \
        -DBUILD_PERF_TESTS=OFF \
        -DBUILD_TESTS=OFF \
        -DBUILD_SHARED_LIBS=ON \
        -DBUILD_ZLIB=ON \
        -DBUILD_TIFF=ON \
        -DBUILD_WEBP=ON \
        -DBUILD_OPENJPEG=ON \
        -DBUILD_PNG=ON \
        -DBUILD_TIFF=ON \
        -DBUILD_WEBP=ON \
        -DBUILD_OPENJPEG=ON \
        -DBUILD_PNG=ON \
        -DBUILD_PROTOBUF=OFF \
        -DPROTOBUF_UPDATE_FILES=ON \
        -DCV_TRACE=OFF \
        -DENABLE_PRECOMPILED_HEADERS=OFF \
        -DOPENCV_GENERATE_PKGCONFIG=OFF \
        -DOpenGL_GL_PREFERENCE=GLVND \
        -DWITH_CAROTENE=OFF \
        -DWITH_GDAL=OFF \
        -DWITH_IPP=OFF \
        -DWITH_ITT=OFF \
        -DWITH_OPENGL=ON \
        -DWITH_OPENCL=OFF \
        -DWITH_OPENEXR=OFF \
        -DWITH_QT=ON \
        -DWITH_V4L=OFF; \
     make $$OPENCV_MFLAGS -C $$OPENCV_PATH \

opencv_install.depends = opencv_build
opencv_install.path = /usr/share/ru.yurasov.wallet/lib
opencv_install.files = $$OPENCV_PATH/lib/*.so*
opencv_install.CONFIG = no_check_exist # this flag is necessary for the correct generation of the makefile

PRE_TARGETDEPS += \
    opencv_build \

QMAKE_EXTRA_TARGETS += \
    opencv_build \
    opencv_install \

INSTALLS += \
    opencv_install \
