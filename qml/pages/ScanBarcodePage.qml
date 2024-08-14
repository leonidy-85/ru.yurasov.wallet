import QtQuick 2.6
import QtMultimedia 5.6
import Sailfish.Silica 1.0
import Amber.QrFilter 1.0
import Aurora.Controls 1.0

import "../db.js" as DB


Dialog {
    id: scanPage

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

        Button {
            id: save
            width: parent.width
            text: qsTr("Manual input")
            onClicked: {
                   pageStack.replace(Qt.resolvedUrl("AddBarcodePage.qml"), {"codeValue":"" })
            }
         }

        Connections {
            target: qrFilter
            onResultChanged: {
//                console.log("Распознанный результат:", qrFilter.result );
               // console.log("Распознанный Format:", detectFormat(qrFilter.result) );

                pageStack.replace(Qt.resolvedUrl("AddBarcodePage.qml"), {
                                      "codeValue": qrFilter.result,
                                      "codeFormat": detectFormat(qrFilter.result)
                                  })
             //   qrFilter.clearResult();
            }
        }

        function detectFormat(code) {
               if (/^\d{13}$/.test(code)) {
                   return "EAN"
               } else if (/^[ -~]{1,48}$/.test(code)) {
                   return "Code 128"
               } else if (/^[ -~]{1,1100}$/.test(code)) {
                   return "PDF417"
               } else if (/^[0-9A-Fa-f]{8,}$/.test(code)) {
                   return "QR Code"
               } else {
                   return "Unknown";
               }
           }


}
