#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QtQml>
#include <qqml.h>
#include <QtGui>
#include <QQuickView>
//#include "osread.h"
#include <auroraapp.h>
//#include <QString>

//#include <QDBusInterface>


#include "fileio.h"
#include "barcodeimageprovider.h"
#include "imageconverter.h"

//#include "zipfileimageprovider.h"


using namespace Aurora;

int main(int argc, char *argv[])
{


//    QDBusInterface other("ru.yurasov.wallet", "/ru/yurasov/wallet", "ru.yurasov.wallet");
//    // check if another instance is running
//    if (other.isValid()) {
//        // yes, it's running, so signal it and exit
//        QString origin;
//        if (argc == 2)
//            origin = argv[1];
//        other.call("openPass", origin);
//        return 0;
//    }


    QString homepath;
    homepath = QDir::homePath();
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
  //  view->engine()->addImageProvider("zipimage", new ZipFileImageProvider());


    view->setSource(Application::pathTo("qml/wallet.qml"));
    view->rootContext()->setContextProperty("XDG_HOME_DIR", QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    view->rootContext()->setContextProperty("XDG_CACHE_DIR", QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    view->show();
    return app->exec();
}
