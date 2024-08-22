import QtQuick 2.6
import QtMultimedia 5.6
import Sailfish.Silica 1.0
import Amber.QrFilter 1.0
import Aurora.Controls 1.0

import "../db.js" as DB

Dialog {
    id: scanPage
    anchors.fill: parent // Заполняем весь родительский элемент

    Column {
        anchors.fill: parent
        spacing: 10

        // Элемент, который будет занимать оставшееся пространство
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
                        "codeFormat": DB.detectFormat(qrFilter.result)
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
                   text: qsTr("Manual input")
                   onClicked: {
                       pageStack.replace(Qt.resolvedUrl("AddBarcodePage.qml"), {"codeValue":"" })
                   }
               }
    }
}
