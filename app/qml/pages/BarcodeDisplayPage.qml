import QtQuick 2.5
import Sailfish.Silica 1.0
import "../db.js" as DB
import "../components"

Item {
    id: barcodeDisplayPage

    property string barcode_name: ""
    property int zint_code: 13
    property string barcodeEncoding: "iso-8859-1"
    property string barcode_code: ""
    property string barcode_icon: ""
    property string barcode_description: ""


    property QtObject element

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
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: col.height
        clip: true

    Column {
           id: col
        anchors.fill: parent
        spacing: Theme.paddingLarge

        PageHeader {
            title:  ""
        }


        Image {
            id: iconImageGrid
            source: barcode_icon !== "" ?  barcode_icon : "../icons/shablon.svg"
            width: parent.width * .8
            fillMode: Image.PreserveAspectFit
            anchors.horizontalCenter: parent.horizontalCenter
            smooth: false

            Column {
                anchors.fill: parent
                anchors.leftMargin:   iconImageGrid.width/4
                spacing: Theme.paddingLarge

                Item {
                    Image {
                        id: iconImageIcon
                        source: DB.img_src(barcode_icon, zint_code, 1)
                        sourceSize: Qt.size(Theme.itemSizeSmall, Theme.itemSizeSmall)
                        fillMode: Image.PreserveAspectFit
                        visible: barcode_icon === ""
                        smooth: false
                         height:  Theme.dp(125)
                    }

                    Label {
                        id: barcodeLabel
                        text: barcode_name
                        visible: barcode_icon === ""
                        font.pixelSize: Theme.fontSizeExtraLargeBase
                        color: Theme.highlightDimmerColor
                        elide: Text.ElideRight
                        anchors.top: iconImageIcon.bottom
                        anchors.topMargin: iconImageGrid.height * 0.3
                        anchors.leftMargin: Theme.dp(20)

                    }
                }
            }
        }


        Separator {
            color: Theme.primaryColor
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Qt.AlignHCenter
        }

        Rectangle {
            id: imageContainer
            color: "white"
         //   anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            width:  zint_code==58 ? Theme.dp(300) : iconImageGrid.width
            height: zint_code==58 || zint_code==13 ? Theme.dp(300) : Theme.dp(200)
            radius: 5
            Image {
                id: qrImage
                anchors.centerIn: parent // Центрируем изображение внутри прямоугольника
                smooth: false
                fillMode: Image.PreserveAspectFit
                source: "image://barcode/" + zint_code + "/" + barcodeEncoding + "/" + barcode_code
                width: imageContainer.width - Theme.dp(12)
                height:  imageContainer.height
            }
        }

        Separator {
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
          //  anchors.topMargin: 40
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
            visible: barcode_description!=''
            //anchors.topMargin: 40
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
           // anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.leftMargin: 0
            anchors.rightMargin: 0
        }


        Component.onCompleted: {
            console.log(barcodeDisplayPage.barcode_name)
            console.log(barcodeDisplayPage.barcode_name)
            console.log(barcodeDisplayPage.barcode_code)
            if (barcodeDisplayPage.barcode_code === "") {
                //notify.show(qsTr("Can't create QR-Code from an empty String"), 4000);
            } else {
                mainapp.iconSource = "image://barcode/" + barcodeDisplayPage.zint_code + "/" + barcodeDisplayPage.barcodeEncoding + "/" + barcodeDisplayPage.barcode_code;
                mainapp.codeDescription = barcode_name;
                mainapp.barcodeDisplayed = true;
            }
        }

    }


}
}
