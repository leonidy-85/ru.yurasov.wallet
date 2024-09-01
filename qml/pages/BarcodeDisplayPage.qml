import QtQuick 2.5
import Sailfish.Silica 1.0
import "../db.js" as DB
import "../components"

Page {
    id: barcodeDisplayPage

    allowedOrientations: Orientation.All

    property string barcode_name: ""
    property int zint_code: 13
    property string barcodeEncoding: "iso-8859-1"
    property string barcode_code: ""
    property string barcode_icon: ""
    property string barcode_description: ""


    Column {
        anchors.fill: parent
        spacing: 10
        anchors.topMargin: 90

        Image {
            id: iconImageGrid
            source: barcode_icon !== "" ?  barcode_icon : "../icons/shablon.svg"
            width: parent.width * 0.8
            height: implicitHeight
            fillMode: Image.PreserveAspectFit
            anchors.horizontalCenter: parent.horizontalCenter
            smooth: false

            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                Item {
                    Image {
                        id: iconImageIcon
                        source: DB.img_src(barcode_icon, zint_code, 1)
                        sourceSize: Qt.size(Theme.itemSizeSmall, Theme.itemSizeSmall)
                        height: 125
                        fillMode: Image.PreserveAspectFit
                        visible: barcode_icon === ""
                        smooth: false
                    }

                    Label {
                        id: barcodeLabel
                        text: barcode_name
                        height: 40
                        visible: barcode_icon === ""
                        font.pixelSize: Theme.fontSizeExtraLargeBase
                        color: Theme.highlightDimmerColor
                        elide: Text.ElideRight
                        anchors.top: iconImageIcon.bottom
                        anchors.topMargin: iconImageGrid.height * 0.3
                        anchors.leftMargin: 20

                    }
                }
            }
        }


        Separator {
           id: separator
            anchors.top: iconImageGrid.bottom
            color: Theme.primaryColor
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
            anchors.topMargin: 40
        }

        Image {
            id: qrImage
            anchors.top: separator.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            smooth: false
            fillMode: Image.PreserveAspectFit
            source: "image://barcode/" + zint_code + "/" + barcodeEncoding + "/" + barcode_code
            width: parent.width * 0.8
            height: 300
        }
        Separator {
           id: separator2
            anchors.top: qrImage.bottom
            anchors.topMargin: 40
            color: Theme.primaryColor
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
        }

        Label {
            id: qRLabel
            text: formatCode(barcode_code)
            height: 40
            font.pixelSize: Theme.fontSizeExtraLargeBase
            color: Theme.primaryColor
            elide: Text.ElideRight
            anchors.top: separator2.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.leftMargin: 0
            anchors.rightMargin: 0
        }
        Label {
            id: qRText
            text: qsTr("Card number")
            height: 20
            font.pixelSize: Theme.fontSizeSmallBase
            color: Theme.highlightColor
            elide: Text.ElideRight
            anchors.top: qRLabel.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.leftMargin: 0
            anchors.rightMargin: 0
        }


        Label {
            id: addLabel
            text: barcode_description
            height: 40
            font.pixelSize: Theme.fontSizeExtraLargeBase
            color: Theme.primaryColor
            elide: Text.ElideRight
            anchors.top: qRText.bottom
            visible: barcode_description!=''
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.leftMargin: 0
            anchors.rightMargin: 0
        }
        Label {
            id: addText
            text: qsTr("Additionall field")
            height: 20
            font.pixelSize: Theme.fontSizeSmallBase
            color: Theme.highlightColor
            elide: Text.ElideRight
            visible: barcode_description!=''
            anchors.top: addLabel.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.leftMargin: 0
            anchors.rightMargin: 0
        }


        Component.onCompleted: {
            if (barcode_code === "") {
                notify.show(qsTr("Can't create QR-Code from an empty String"), 4000);
            } else {
                mainapp.iconSource = "image://barcode/" + zint_code + "/" + barcodeEncoding + "/" + barcode_code;
                mainapp.codeDescription = barcode_name;
                mainapp.barcodeDisplayed = true;
            }
        }
    }

    /** Разбивка по 4 символа для цифровых кодов */
    function formatCode(barCode) {
        var result = ""
        if (isNaN(barCode)) {
            result = barCode
        } else {
            for (var i = 0; i < barCode.length; i+=4) {
              result += barCode.slice(i, i + 4) + " "
            }
        }
        return result.trim();
    }
}
