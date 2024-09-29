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

    property string base64: ""

    //form scan
    property string scan: ""
    property string type: ""

    property int view: 1//list

    signal openPass(string origin)

    initialPage: Component {
        MainPage {}
    }

    property string topIcon: ""
    property string topPath: ""

    function passClicked(origin) {
        openPass(origin);
    }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    allowedOrientations: defaultAllowedOrientations
    _defaultPageOrientations: defaultAllowedOrientations
}
