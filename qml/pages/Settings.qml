import QtQuick 2.2
import Sailfish.Silica 1.0
import Sailfish.Pickers 1.0
import Nemo.Notifications 1.0
import ru.yurasov.wallet.FileIO 1.0
import "../db.js" as DB

Page {
  id: exportPage

  allowedOrientations: Orientation.All

  property QtObject parentPage: null
  property string mode: "import"

  function fillNum(num) {
    if (num < 10) {
      return("0"+num);
    } else {
      return(num)
    }
  }

  function creFileName() {
    var date = new Date();
    return(XDG_HOME_DIR + "/Documents/wallet_"+fillNum(date.getDate())+fillNum(date.getMonth()+1)+date.getFullYear()+".text");
  }

  function checkFileName(file) {
    if (mode == "export") {
      if (exportFile.exists(file) && !fileOverwrite.checked) {
        notify.show(qsTr("File already exists, choose \"Overwrite existing\" to overwrite it."), 4000);
        return(false)
      } else {
        return(true)
      }
    } else {
      if (exportFile.exists(file)) {
        return(true)
      } else {
         DB.banner(qsTr("Given file does not exist!"));
        return(false)
      }
    }
  }

  // FileIO Object for reading / writing files
  FileIO {
    id: exportFile
    source: fileName.text
    onError: { console.log(msg); }
  }
  Notification {
      id: notification
      itemCount: 1
  }


  SilicaFlickable {
    id: exportFlickable
    anchors.fill: parent

    PullDownMenu {
      MenuItem {
        text: mode == "export" ? qsTr("Import") : qsTr("Export")
        onClicked: {
          if (mode == "export") {
            mode = "import"
            fileName.text = ""
          } else {
            mode = "export"
            fileName.text = creFileName()
          }
        }
      }
    }

    // FilePicker for the Input File
    Component {
      id: filePickerPage
      FilePickerPage {
        nameFilters: [ '*.text' ]
        onSelectedContentPropertiesChanged: {
          fileName.text = selectedContentProperties.filePath
        }
      }
    }

    VerticalScrollDecorator {}

     Column {
      spacing: 10
      anchors.fill: parent
      PageHeader {
          objectName: "pageHeader"
          title: mode == "export" ? qsTr("Export") : qsTr("Import")
      }

      TextField {
        id: fileName
        width: parent.width
        text: mode == "export" ? creFileName() : "";
        label: qsTr("Filename")
        placeholderText: qsTr("File to export")
        visible: mode == "export"
        focus: true
        horizontalAlignment: TextInput.AlignLeft
        EnterKey.enabled: text.length > 0
        EnterKey.iconSource: "image://theme/icon-m-enter-accept"
        EnterKey.onClicked: exportPage.accept()
      }

      ValueButton {
        width: parent.width
        label: qsTr("File to import")
        value: fileName.text ? fileName.text : "None"
        visible: mode == "import"
        onClicked: pageStack.push(filePickerPage)
      }

      TextSwitch {
        id: fileOverwrite
        checked: false
        visible: mode == "export"
        text: qsTr("Overwrite existing")
      }

      Text {
        id: importText

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 20
        width: parent.width - 2*Theme.paddingLarge

        wrapMode: Text.Wrap
        maximumLineCount: 15
        font.pixelSize: Theme.fontSizeTiny
        color: Theme.secondaryColor

        visible: mode == "import"
        text: qsTr("Here you can Import for barcode from a file. Put in the file location you used on export. Pull left to start the import.")
      }

      Text {
        id: exportText

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 20
        width: parent.width - 2*Theme.paddingLarge

        wrapMode: Text.Wrap
        maximumLineCount: 15
        font.pixelSize: Theme.fontSizeTiny
        color: Theme.secondaryColor

        visible: mode == "export"
        text: qsTr("Here you can export barcode from a file")
      }
      Separator {
          color: Theme.primaryColor
          width: parent.width
          anchors.horizontalCenter: parent.horizontalCenter
          horizontalAlignment: Qt.AlignHCenter
      }
      Button {
          id: save
          width: parent.width * 0.8
          anchors.horizontalCenter: parent.horizontalCenter
          visible: fileName.text.length > 0 && (mode == "import" ) || checkFileName(fileName.text) ? true : false
          text: mode == "export" ? qsTr("Export") : qsTr("Import")
          onClicked: {
              onDone()
           }
        }
     }
  }

  // Check if we can continue
//    canAccept: fileName.text.length > 0 && (mode == "import" ) || checkFileName(fileName.text) ? true : false
 // canAccept: fileName.text.length > 0 && filePassword.text.length > 0 && (mode == "import" || filePassword.text == filePasswordCheck.text) && checkFileName(fileName.text) ? true : false

  // Do the DB-Export / Import
  function onDone() {
      var plainText = ""

      if (mode == "export") {
        // Export Database to File
        plainText = DB.db2json();
        if (plainText != "") {
              if (!exportFile.write(plainText)) {
                   DB.banner('Error', qsTr("Error writing to file ")+ fileName.text)
              } else {
                  DB.banner('OK', qsTr("Discont card Database exported to ")+ fileName.text)
                }
            } else {
              DB.banner('Error', qsTr( "Could not read Discont card from Database"))
          }
        }
        else if(mode == "import") {

        // Import barcodes from File
        plainText = exportFile.read();
        if (plainText != "") {
             var errormsg = ""
            if (DB.json2db(plainText, errormsg)) {
                   DB.banner('OK', qsTr("Discont card Database imported from ")+ fileName.text)
                 pageStack.pop();
            } else {
              DB.banner(errormsg);
            }
        } else {
            DB.banner('Error', qsTr("Could not read from file ")+ fileName.text)
         }
      }
    }

  }

