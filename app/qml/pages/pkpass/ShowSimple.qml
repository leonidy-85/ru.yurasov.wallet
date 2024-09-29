import QtQuick 2.0
import Sailfish.Silica 1.0
import "../../db.js" as DB


Page {
    id: page
    //allowedOrientations: Orientation.All

    property string jsondata: ''
    property string path: ''
    property string barcodeType: "qr"
    property string barcodeEncoding: "iso-8859-1"
    property string barcodeContent: ""
    property string barcodeAltText: ""
    property int barcodeType2: 20

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: body.height + Theme.paddingLarge * 2


        Column {
            id: body
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: Theme.horizontalPageMargin
            anchors.rightMargin: Theme.horizontalPageMargin
            anchors.topMargin: Theme.paddingLarge
            spacing: Theme.paddingMedium

            Image {
                source: "image://zipimage" + path + "/logo.png"
                width: sourceSize.width * Theme.iconSizeMedium / sourceSize.height
                height: Theme.iconSizeMedium
            }

            Label {
                id: logoText
                text: ''
                textFormat: Text.PlainText
                color: Theme.highlightColor
            }

            Repeater {
                model: ListModel {
                    id: frontFields
                    ListElement { title: ''; value: '' }
                }

                Column {

                    Label {
                        text: title
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeLarge
                        color: Theme.primaryColor
                        x: Theme.paddingLarge
                    }

                    Label {
                        text: value
                        textFormat: Text.PlainText
                        color: Theme.highlightColor
                    }
                }
            }

            Image {
                source: path + "/footer.png"
                width: sourceSize.width != 0 ? body.width : 0
                height: sourceSize.width != 0 ? sourceSize.height * body.width / sourceSize.width : 0

            }

            Rectangle {
                visible: barcodeImage.width != 0
                width: barcodeImage.width + Theme.fontSizeMedium
                height: barcodeImage.height + Theme.fontSizeMedium
                color: 'white'

                Image {
                    id: barcodeImage
                    anchors.centerIn: parent
                    width: sourceSize.width !== 0 ? DB.barcodeSize(sourceSize.width, sourceSize.height, body.width, Theme.fontSizeMedium)[0] : 0
                    height: sourceSize.height !== 0 ? DB.barcodeSize(sourceSize.width, sourceSize.height, body.width, Theme.fontSizeMedium)[1] : 0
                    smooth: false
                    fillMode: Image.PreserveAspectFit
                    //source: "image://barcode/" + barcodeType2 + "/" + barcodeEncoding + "/" + barcodeContent;
                    MouseArea{
                          anchors.fill: parent
                           onClicked: {
                       pageStack.push(Qt.resolvedUrl("../BarcodeDisplayPage.qml"), { barcode_code: barcodeAltText, barcodeEncoding: barcodeEncoding, zint_code: barcodeType2 })
                      }
                   }
                }

            }

            Label {
                text: barcodeAltText
                textFormat: Text.PlainText
                color: Theme.highlightColor
            }

            Repeater {
                model: ListModel {
                    id: backFields
                    ListElement { title: ''; value: '' }
                }

                Column {

                    Label {
                        text: title
                        textFormat: Text.PlainText
                        font.pixelSize:  Theme.fontSizeLarge
                        color: Theme.primaryColor
                        x: Theme.paddingLarge
                    }

                    Label {
                        text: value
                        textFormat: Text.PlainText
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.highlightColor
                        width: body.width
                        wrapMode: Text.Wrap
                    }
                }
            }
        }

        VerticalScrollDecorator {}
    }

    Component.onCompleted: {




        function setFields(pass, style, fieldType, target) {
            // set a field model from json
            var fields = [];
            if (fieldType in pass[style]) {
                for (var field = 0; field < pass[style][fieldType].length; field++) {
                    var data = pass[style][fieldType][field];
                    target.append({ title: String(data.label), value: String(data.value) });
                }
            }
        }

        // get general pass data
        var pass = JSON.parse(jsondata);
        if ('logoText' in pass)
            logoText.text = pass.logoText
        var styles = ["boardingPass", "coupon", "eventTicket", "storeCard", "generic"];
        var style = '';
        for (var key = 0; key < styles.length; key++) {
            if (styles[key] in pass) {
                style = styles[key];
                break;
            }
        }
        // complete undefined fields
        DB.checkFields(pass, style);
        // set front field contents
        frontFields.clear();
        setFields(pass, style, 'headerFields', frontFields);
        setFields(pass, style, 'primaryFields', frontFields);
        setFields(pass, style, 'secondaryFields', frontFields);
        setFields(pass, style, 'auxiliaryFields', frontFields);
        // look for barcodes
        if (!('barcodes' in pass)) {
            pass.barcodes = [];
            if ('barcode' in pass) {
                pass.barcodes.push(pass.barcode);
            }
        }
        // paint the first useable barcode
        var validCode = false;
        for (var barcode = 0; barcode <= pass.barcodes.length; barcode++) {
            switch(pass.barcodes[barcode].format.substring(15).toLowerCase()) {
            case 'code128':
            case 'qr':
            case 'ean':
            case 'aztec':
            case 'pdf417':
                barcodeContent = 'message' in pass.barcodes[barcode] ? Qt.btoa(pass.barcodes[barcode].message) : '';
                barcodeEncoding = 'messageEncoding' in pass.barcodes[barcode] ?  pass.barcodes[barcode].messageEncoding: 'iso-8859-1';
                barcodeType = pass.barcodes[barcode].format.substring(15).toLowerCase();
                if(barcodeType==="ean"){
                    barcodeType2=13
                }
                if(barcodeType==="code128"){
                    barcodeType2=20
                }
                 if(barcodeType==="code39"){
                    barcodeType2=8
                }
                   if(barcodeType==="pfd417"){
                    barcodeType2=55
                }
                   if(barcodeType==="qr"){
                    barcodeType2=58
                }
                   console.log(barcodeType)
                   console.log(barcodeType2)
                barcodeAltText = 'altText' in pass.barcodes[barcode] ? pass.barcodes[barcode].altText : '';
                validCode = true;
                 barcodeImage.source =  "image://barcode/" + barcodeType2 + "/" + barcodeEncoding + "/" + barcodeAltText;
                break;
            }
            if (validCode)
                break;
        }
        // set back field contents
        backFields.clear();
        setFields(pass, style, 'backFields', backFields);
    }
}