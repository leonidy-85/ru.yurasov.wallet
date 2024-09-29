import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import '../../db.js' as Utils



Rectangle {
    id: root
    property string jsondata: ''
    property string path: ''
    property string relevantDate: ''
    property color backgroundColor: 'white'
    property color labelColor: 'black'
    property color textColor: 'black'
    property string logoText: ''
    property alias headerFields: headerFieldsModel
    property string boardingFromKey: ''
    property string boardingFromTitle: ''
    property string boardingFromValue: ''
    property string boardingToKey: ''
    property string boardingToTitle: ''
    property string boardingToValue: ''
    property string primaryKey: ''
    property string primaryTitle: ''
    property string primaryValue: ''
    property alias secondaryFields: secondaryFieldsModel
    property alias tertiaryFields: tertiaryFieldsModel
    property string barcodeType: ''
    property string barcodeEncoding: ''
    property string barcodeContent: ''
    property string barcodeAltText: ''
    property int barcodeType2: 20

    height: body.height + Theme.paddingMedium * 2

    Rectangle {
        id: background
        anchors.fill: parent
        color: backgroundColor
    }

    Image {
        id: background_image
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source:  path + "/background.png"
    }

    FastBlur {
        anchors.fill: background_image
        source: background_image
        radius: 32
    }

    Column {
        id: body
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Theme.paddingMedium
        spacing: Theme.paddingMedium

        Row {
            id: headerRow
            spacing: Theme.paddingLarge

            Image {
                id: logo
                source: "image://zipimage" + path + "/logo.png"
                width: sourceSize.width * Theme.iconSizeMedium / sourceSize.height
                height: Theme.iconSizeMedium
            }

            Label {
                text: logoText
                textFormat: Text.PlainText
                color: textColor
                font.pixelSize: Theme.fontSizeSmall
                width: body.width - logo.width - headerFieldsRow.width - parent.spacing * 2
            }

            Row {
                id: headerFieldsRow
                spacing: Theme.paddingLarge

                Repeater {
                    model: ListModel {
                        id: headerFieldsModel
                        ListElement { field: ""; title: ""; value: "" }
                    }

                    Column {

                        Label {
                            text: title
                            textFormat: Text.PlainText
                            color: labelColor
                            font.pixelSize: Theme.fontSizeTiny
                        }

                        Label {
                            text: value
                            textFormat: Text.StyledText
                            color: textColor
                            font.pixelSize: Theme.fontSizeTiny
                        }
                    }
                }
            }
        }

        Row {
            id: boardingPrimary
            visible: boardingFromTitle != '' || boardingFromValue != '' || boardingToTitle != '' || boardingToValue != ''

            Column {
                width: body.width / 2 - parent.spacing

                Label {
                    text: boardingFromTitle
                    textFormat: Text.PlainText
                    color: labelColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                }

                Label {
                    text: boardingFromValue
                    textFormat: Text.StyledText
                    color: textColor
                    font.pixelSize: text.length <= 3 ? Theme.fontSizeHuge : Theme.fontSizeLarge
                }
            }

            Column {
                width: body.width / 2 - parent.spacing

                Label {
                    anchors.right: parent.right
                    text: boardingToTitle
                    textFormat: Text.PlainText
                    color: labelColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                }

                Label {
                    anchors.right: parent.right
                    text: boardingToValue
                    textFormat: Text.StyledText
                    color: textColor
                    font.pixelSize: text.length <= 3 ? Theme.fontSizeHuge : Theme.fontSizeLarge
                }
            }
        }

        Image {
            id: standardPrimary
            visible: primaryTitle != '' || primaryValue != ''
            source:  path + "/strip.png"
            width: body.width
            height: sourceSize.width != 0 ? sourceSize.height * body.width / sourceSize.width : standardPrimaryRow.height

            Row {
                id: standardPrimaryRow
                spacing: Theme.paddingLarge

                Column {
                    width: body.width - thumbnailImage.width - parent.spacing

                    Label {
                        text: primaryTitle
                        textFormat: Text.PlainText
                        color: labelColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }

                    Label {
                        width: parent.width
                        text: primaryValue
                        textFormat: Text.StyledText
                        color: textColor
                        font.pixelSize: Theme.fontSizeLarge
                        wrapMode: Text.Wrap
                    }
                }

                Image {
                    id: thumbnailImage
                    width: sourceSize.width != 0 ? Theme.itemSizeExtraLarge : 0
                    height: sourceSize.width != 0 ? sourceSize.height * Theme.itemSizeExtraLarge / sourceSize.width : 0
                    source:  path + "/thumbnail.png"
                }
            }
        }

        Flow {
            width: body.width
            spacing: Theme.paddingLarge

            Repeater {
                model: ListModel {
                    id: secondaryFieldsModel
                    ListElement { field: ""; title: ""; value: "" }
                }

                Column {

                    Label {
                        text: title
                        textFormat: Text.PlainText
                        color: labelColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }

                    Label {
                        text: value
                        textFormat: Text.StyledText
                        color: textColor
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }
            }
        }

        Flow {
            width: body.width
            spacing: Theme.paddingLarge

            Repeater {
                model: ListModel {
                    id: tertiaryFieldsModel
                    ListElement { field: ""; title: ""; value: "" }
                }

                Column {

                    Label {
                        text: title
                        textFormat: Text.PlainText
                        color: labelColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }

                    Label {
                        text: value
                        textFormat: Text.StyledText
                        color: textColor
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }
            }
        }

        Image {
            source:  path + "/footer.png"
            width: sourceSize.width != 0 ? body.width : 0
            height: sourceSize.width != 0 ? sourceSize.height * body.width / sourceSize.width : 0
        }

        Column {
            width: parent.width
            visible: barcodeType != '' && barcodeEncoding != '' && barcodeContent != ''

            Rectangle {
                width: barcodeImage.width + Theme.fontSizeMedium // 0.5em border
                height: barcodeImage.height + Theme.fontSizeMedium
                anchors.horizontalCenter: parent.horizontalCenter
                color: 'white'

                Image {
                    id: barcodeImage
                    anchors.centerIn: parent
                    width: sourceSize.width !== 0 ? Utils.barcodeSize(sourceSize.width, sourceSize.height, body.width, Theme.fontSizeMedium)[0] : 0
                    height: sourceSize.height !== 0 ? Utils.barcodeSize(sourceSize.width, sourceSize.height, body.width, Theme.fontSizeMedium)[1] : 0
                    smooth: false
                    //source: "image://barcode/" + barcodeType + "/" + barcodeEncoding + "/" + barcodeContent;
                }

                MouseArea {
                    anchors.fill: parent
                   // enabled: settingsStore.barcodeTap

                    onClicked:{
                           console.log(barcodeContent, barcodeType2)
                        pageStack.push(Qt.resolvedUrl("../BarcodeDisplayPage.qml"), { barcode_code: barcodeContent, barcodeEncoding: barcodeEncoding, zint_code: barcodeType2 })
                }
                }
            }

            Label {
                text: barcodeAltText
                textFormat: Text.PlainText
                color: textColor
                font.pixelSize: Theme.fontSizeTiny
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    onJsondataChanged: {
        function update_marks(model, changes) {
            // underline updated fields
            for (var field = 0; field < model.count; field++) {
                var key = model.get(field).field;
                var found = false;
                for (var change = 0; change < changes.length; change++) {
                    if (changes[change] === key) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    if (model.get(field).value.substring(0,3) !== '<u>') {  // avoid double marks
                        model.get(field).value = '<u>' + model.get(field).value + '</u>';
                    }
                }
            }
        }

        function update_primary_marks(changes) {
            // underline updated primary fields
            for (var priChange = 0; priChange < changes.length; priChange++) {
                switch (changes[priChange]) {
                case boardingFromKey:
                    if (boardingFromValue.substring(0,3) !== '<u>')
                        boardingFromValue = '<u>' + boardingFromValue + '</u>';
                    break;
                case boardingToKey:
                    if (boardingToValue.substring(0,3) !== '<u>')
                        boardingToValue = '<u>' + boardingToValue + '</u>';
                    break;
                case primaryKey:
                    if (primaryValue.substring(0,3) !== '<u>')
                        primaryValue = '<u>' + primaryValue + '</u>';
                }
            }
        }

        // clear old data
        relevantDate = '';
        backgroundColor = 'white';
        labelColor = 'black';
        textColor = 'black';
        logoText = '';
        headerFields.clear();
        boardingFromKey = '';
        boardingFromTitle = '';
        boardingFromValue = '';
        boardingToKey = '';
        boardingToTitle = '';
        boardingToValue = '';
        primaryKey = '';
        primaryTitle = '';
        primaryValue = '';
        secondaryFields.clear();
        tertiaryFields.clear();
        barcodeType = '';
        barcodeEncoding = '';
        barcodeContent = '';
        barcodeAltText = '';

        // get general pass data
        var pass = JSON.parse(jsondata);
        if ('relevantDate' in pass)
            relevantDate = pass.relevantDate;
        if ('backgroundColor' in pass)
            backgroundColor = Utils.interpretColor(pass.backgroundColor);
        if ('labelColor' in pass)
            labelColor = Utils.interpretColor(pass.labelColor);
        if ('foregroundColor' in pass)
            textColor = Utils.interpretColor(pass.foregroundColor);
        if ('logoText' in pass)
            logoText = pass.logoText
        var styles = ["boardingPass", "coupon", "eventTicket", "storeCard", "generic"];
        var style = '';
        for (var key = 0; key < styles.length; key++) {
            if (styles[key] in pass) {
                style = styles[key];
                break;
            }
        }
        // complete undefined fields
        Utils.checkFields(pass, style);
        // set field contents
        Utils.setFields(pass, style, 'headerFields', headerFields, dateTimeFormat, currencyFormat);
        if (style === "boardingPass") {
            if (pass.boardingPass.primaryFields.length > 0) {
                boardingFromKey = pass.boardingPass.primaryFields[0].key;
                boardingFromTitle = pass.boardingPass.primaryFields[0].label;
                boardingFromValue = Utils.htmlescape(pass.boardingPass.primaryFields[0].value);
            }
            if (pass.boardingPass.primaryFields.length > 1) {
                boardingToKey = pass.boardingPass.primaryFields[1].key;
                boardingToTitle = pass.boardingPass.primaryFields[1].label;
                boardingToValue = Utils.htmlescape(pass.boardingPass.primaryFields[1].value);
            }
            Utils.setFields(pass, style, 'auxiliaryFields', secondaryFields, dateTimeFormat, currencyFormat);
            Utils.setFields(pass, style, 'secondaryFields', tertiaryFields, dateTimeFormat, currencyFormat);
        }
        else {
            if (pass[style].primaryFields.length > 0) {
                primaryKey = pass[style].primaryFields[0].key;
                primaryTitle = pass[style].primaryFields[0].label;
                primaryValue = Utils.htmlescape(pass[style].primaryFields[0].value);
            }
            Utils.setFields(pass, style, 'secondaryFields', secondaryFields, dateTimeFormat, currencyFormat);
            Utils.setFields(pass, style, 'auxiliaryFields', tertiaryFields, dateTimeFormat, currencyFormat);
        }
        // check for changes
//        var changes = passDB.getPassChanges(pass.passTypeIdentifier + '/' + pass.serialNumber);
//        // underline changed fields
//        update_marks(headerFields, changes);
//        update_marks(secondaryFields, changes);
//        update_marks(tertiaryFields, changes);
//        update_primary_marks(changes);
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
            case 'aztec':
            case 'pdf417':
                barcodeEncoding = 'messageEncoding' in pass.barcodes[barcode] ?  pass.barcodes[barcode].messageEncoding: 'iso-8859-1';
                barcodeType = pass.barcodes[barcode].format.substring(15).toLowerCase();
                barcodeAltText = 'altText' in pass.barcodes[barcode] ? pass.barcodes[barcode].altText : '';
//                barcodeContent = 'message' in pass.barcodes[barcode] ? Qt.btoa(pass.barcodes[barcode].message) : '';
                barcodeContent = 'message' in pass.barcodes[barcode] ? pass.barcodes[barcode].message : '';


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
                   barcodeImage.source =  "image://barcode/" + barcodeType2 + "/" + barcodeEncoding + "/" + barcodeContent;


                validCode = true;
                break;
            }
            if (validCode)
                break;
        }
    }
}
