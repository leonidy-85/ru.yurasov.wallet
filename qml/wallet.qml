import QtQuick 2.5
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow {
    id: mainapp
    property bool groupsChanged: false
    property bool barcodesChanged: false
    property bool barcodeDisplayed: false
    property bool search: false

    property bool barcodeUpdate: false

    property string codeDescription: ""
    property string iconSource: ""

    property int view: 1//list


    initialPage: Component {
        MainPage {}
    }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    allowedOrientations: defaultAllowedOrientations
    _defaultPageOrientations: defaultAllowedOrientations
}
