/*
   Copyright (C) 2013 Jolla Ltd.
Contact: Thomas Perl <thomas.perl@jollamobile.com>
All rights reserved.

You may use this file under the terms of BSD license as follows:

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the Jolla Ltd nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QtQml>
#include <qqml.h>
#include <QtGui>
#include <QQuickView>
#include "osread.h"
#include <auroraapp.h>


#include "fileio.h"
#include "barcodeimageprovider.h"

#include "imageconverter.h"

using namespace Aurora;

int main(int argc, char *argv[])
{

    QString homepath;
    homepath = QDir::homePath();
    qmlRegisterType<Launcher>("ru.yurasov.wallet.Launcher", 1 , 0 , "App");
       // Register FileIO Class
    qmlRegisterType<FileIO, 1>("ru.yurasov.wallet.FileIO", 1, 0, "FileIO");
    //convertor to base64
    qmlRegisterType<ImageConverter>("ru.yurasov.wallet.imageConverter", 1, 0, "ImageConverter");

    QGuiApplication* app = Application::application(argc, argv);

    QLocale::setDefault(QLocale::c());

    QQuickView* view = Application::createView();
    view->rootContext()->setContextProperty("homedir", homepath);
    view->rootContext()->setContextProperty("version", APP_VERSION);
    view->rootContext()->setContextProperty("buildyear", BUILD_YEAR);

    view->engine()->addImageProvider("barcode", new BarcodeImageProvider());


    view->setSource(Application::pathTo("qml/wallet.qml"));
    view->rootContext()->setContextProperty("XDG_HOME_DIR", QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    view->rootContext()->setContextProperty("XDG_CACHE_DIR", QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    view->show();
    return app->exec();
}
