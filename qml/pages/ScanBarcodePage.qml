import QtQuick 2.6
import QtMultimedia 5.6
import Sailfish.Silica 1.0
import Amber.QrFilter 1.0
import Aurora.Controls 1.0

import "../db.js" as DB

Dialog {
    id: scanPage
    anchors.fill: parent
    property string barcode_name : ""
    property string barcode_type : ""
    property string barcode_icon : ""
    property int type : 0

    Column {
        anchors.fill: parent
        spacing: 10


        Item {
            anchors.fill: parent
            anchors.bottom: save.top // Привязываем этот элемент к верхней части кнопки

            QrFilter {
                id: qrFilter
                objectName: "qrFilter"
                active: true
                format: QrFilter.QR_CODE | QrFilter.CODE_128 | QrFilter.EAN_13 | QrFilter.PDF_417
            }

            VideoOutput {
                id: viewer
                objectName: "viewer"
                anchors {
                    topMargin: 90
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: Theme.horizontalPageMargin
                }
                fillMode: VideoOutput.PreserveAspectFit
                source: Camera {
                    captureMode: Camera.CaptureVideo
                    focus {
                        focusMode: CameraFocus.FocusContinuous
                        focusPointMode: CameraFocus.FocusPointAuto
                    }
                }
                filters: [ qrFilter ]
            }

            Connections {
                target: qrFilter
                onResultChanged: {
                    pageStack.replace(Qt.resolvedUrl("AddBarcodePage.qml"), {
                        "codeValue": qrFilter.result,
                        "codeFormat": type===1 ? "" : DB.detectFormat(qrFilter.result),
                                          "barcode_name" : barcode_name,
                                          "barcode_type" : barcode_type,
                                           "barcode_icon" : barcode_icon
                    })
                }
            }


        }

        Button {
                   id: save
                   width: parent.width * 0.8
                   anchors.horizontalCenter: parent.horizontalCenter
                   anchors.bottom: parent.bottom
                   anchors.bottomMargin: 30
                   text: qsTr("Cancel")
                   onClicked: {
                       pageStack.replace(Qt.resolvedUrl("AddBarcodePage.qml"), {"codeValue":"",
                                             "barcode_name" : barcode_name,
                                             "barcode_type" : barcode_type,
                                              "barcode_icon" : barcode_icon
                                         })
                   }
               }
    }
}
