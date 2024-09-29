import QtQuick 2.0
import Sailfish.Silica 1.0
import '../../db.js' as Utils

Rectangle {
    id: root
    property string jsondata: ''
    property color backgroundColor: 'white'
    property color labelColor: 'black'
    property color textColor: 'black'
    property alias backFields: backFieldsModel

    height: body.height + Theme.paddingMedium * 2
    color: backgroundColor

    Column {
        id: body
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Theme.paddingMedium
        spacing: Theme.paddingMedium

        Repeater {
            model: ListModel {
                id: backFieldsModel
                ListElement { title: ''; value: '' }
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
                    font.pixelSize: Theme.fontSizeExtraSmall
                    width: body.width
                    wrapMode: Text.Wrap
                }
            }
        }
    }

    onJsondataChanged: {
        //clear old data
        backgroundColor = 'white';
        labelColor = 'black';
        textColor = 'black';
        backFields.clear();

        // get general pass info
        var pass = JSON.parse(jsondata);
        if ('backgroundColor' in pass)
            backgroundColor = Utils.interpretColor(pass.backgroundColor);
        if ('labelColor' in pass)
            labelColor = Utils.interpretColor(pass.labelColor);
        if ('foregroundColor' in pass)
            textColor = Utils.interpretColor(pass.foregroundColor);
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
        Utils.setFields(pass, style, 'backFields', backFields, dateTimeFormat, currencyFormat);
    }
}
