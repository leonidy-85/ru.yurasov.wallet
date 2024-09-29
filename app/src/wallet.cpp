#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QtQml>
#include <qqml.h>
#include <QtGui>
#include <QQuickView>
#include <QtQml/QQmlContext>
#include <auroraapp.h>
#include <QString>
#include <QDBusInterface>
#include <QDBusError>

#include "fileio.h"

#include "image/ImageProcessor.h"
#include "image/resizeimage.h"
#include "image/ColorGenerator.h"
#include "image/imageconverter.h"

#include "camerahelper.h"
#include "qmlimage.h"
#include "generator/barcodeimageprovider.h"

#include "scan/types.h"
#include "scan/helper.h"
#include "scan/photoeditor.h"
#include "scan/photocamera.h"
#include "scan/photorecognizer.h"
#include "scan/photoaligner.h"
#include "scan/page.h"
#include "scan/pagemodel.h"
#include "scan/document.h"
#include "scan/documentmodel.h"
#include "scan/storage.h"
#include "scan/service.h"

#include "pkpass/settingsstore.h"
#include "pkpass/zipfileimageprovider.h"
#include "pkpass/homewatcher.h"
#include "pkpass/notificator.h"
#include "pkpass/datetimeformat.h"
#include "pkpass/currencyformat.h"
#include "pkpass/passhandler.h"
#include "pkpass/passdb.h"

using namespace Aurora;

template <typename T>
static QObject *singletonCreate(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    return new T;
}

int main(int argc, char *argv[])
{

    QScopedPointer<QGuiApplication> app(Aurora::Application::application(argc, argv));
    QQuickView* view = Application::createView();


    QDBusInterface other("ru.yurasov.wallet", "/ru/yurasov/wallet", "ru.yurasov.wallet");
   // QDBusInterface other("ru.yurasov.wallet", "/ru/yurasov/wallet", "ru.yurasov.wallet");
    // check if another instance is running
    if (other.isValid()) {
        // yes, it's running, so signal it and exit
        QString origin;
        if (argc == 2)
            origin = argv[1];
        other.call("openPass", origin);
        return 0;
    }

    QString homepath;
    homepath = QDir::homePath();
    // Register FileIO Class
    qmlRegisterType<FileIO, 1>("ru.yurasov.wallet.FileIO", 1, 0, "FileIO");
    //convertor to base64
    qmlRegisterType<ImageConverter>("ru.yurasov.wallet.imageConverter", 1, 0, "ImageConverter");
    qmlRegisterType<QmlImage>("ru.yurasov.wallet.QmlImage", 1, 0, "QmlImage");
    qmlRegisterType<ImageProcessor>("ru.yurasov.wallet.imageProcessor", 1, 0, "ImageProcessor");
    qmlRegisterType<resizeImage>("ru.yurasov.wallet.resizeImage", 1, 0, "ResizeImage");

    qmlRegisterSingletonType<DSHelper>("ru.yurasov.wallet", 1, 0, "DSHelper", singletonCreate<DSHelper>);
    qmlRegisterUncreatableType<DSPageSize>("ru.yurasov.wallet", 1, 0, "DSPageSize", "");
    qmlRegisterUncreatableType<DSFrame>("ru.yurasov.wallet", 1, 0, "DSFrame", "");
    qmlRegisterType<DSPhotoCamera>("ru.yurasov.wallet", 1, 0, "DSPhotoCamera");
    qmlRegisterType<DSPhotoRecognizer>("ru.yurasov.wallet", 1, 0, "DSPhotoRecognizer");
    qmlRegisterType<DSPhotoAligner>("ru.yurasov.wallet", 1, 0, "DSPhotoAligner");
    qmlRegisterType<DSPhotoEditor>("ru.yurasov.wallet", 1, 0, "DSPhotoEditor");
    qmlRegisterType<DSPage>("ru.yurasov.wallet", 1, 0, "DSPage");
    qmlRegisterType<DSPageModel>("ru.yurasov.wallet", 1, 0, "DSPageModel");
    qmlRegisterType<DSDocument>("ru.yurasov.wallet", 1, 0, "DSDocument");
    qmlRegisterType<DSDocumentModel>("ru.yurasov.wallet", 1, 0, "DSDocumentModel");

    qRegisterMetaType<DSPageSize>("DSPageSize");
    qRegisterMetaType<DSPageSize::Id>("DSPageSize::Id");
    qRegisterMetaType<DSFrame>("DSFrame");
    qRegisterMetaType<DSPage *>("DSPage *");
    qRegisterMetaType<DSPageModel *>("DSPageModel *");
    qRegisterMetaType<DSDocument *>("DSDocument *");
    qRegisterMetaType<DSDocumentModel *>("DSDocumentModel *");
    qRegisterMetaType<DSStorage *>("DSStorage *");

    app->setOrganizationName(QStringLiteral("ru.yurasov"));
    app->setApplicationName(QStringLiteral("wallet"));

    QStringList applicationArgs = app->arguments();
    applicationArgs.removeFirst();

    QLocale::setDefault(QLocale::c());

    CameraHelper camera_helper;

    view->rootContext()->setContextProperty("homedir", homepath);
    view->rootContext()->setContextProperty("version", APP_VERSION);
    view->rootContext()->setContextProperty("builddate", BUILD_DATE);
    view->rootContext()->setContextProperty("cameraHelper", &camera_helper);


    view->rootContext()->setContextProperty("XDG_HOME_DIR", QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    view->rootContext()->setContextProperty("XDG_CACHE_DIR", QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    SettingsStore settingsStore;
    view->rootContext()->setContextProperty("settingsStore", &settingsStore);

    view->engine()->addImageProvider("barcode", new BarcodeImageProvider());
    view->engine()->addImageProvider("zipimage", new ZipFileImageProvider());

    HomeWatcher homeWatcher;
    view->rootContext()->setContextProperty("homeWatcher", &homeWatcher);
    Notificator notificator;
    view->rootContext()->setContextProperty("notificator", &notificator);
    DateTimeFormat dateTimeFormat;
    view->rootContext()->setContextProperty("dateTimeFormat", &dateTimeFormat);
    CurrencyFormat currencyFormat;
    view->rootContext()->setContextProperty("currencyFormat", &currencyFormat);
    PassHandler passHandler;
    view->rootContext()->setContextProperty("passHandler", &passHandler);
    PassDB passDB;
    view->rootContext()->setContextProperty("passDB", &passDB);

    QScopedPointer<DSService> service(new DSService(app.data()));
    QScopedPointer<DSStorage> storage(new DSStorage());
    QSurfaceFormat format = view->format();
    format.setSamples(16);
    view->setFormat(format);
    view->rootContext()->setContextProperty(QStringLiteral("Storage"), storage.data());
    view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/wallet.qml")));
    view->show();
    return app->exec();
}
