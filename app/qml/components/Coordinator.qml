import QtQuick 2.0

BaseSplitViewCoordinator {

    property string masterItem: "../pages/WalletPage.qml"
    property string detailsItem: "../pages/BarcodeDisplayPage.qml"

    function start() {
        var master = pushMaster(masterItem)
        master.pressedNext.connect(didMasterPressedNext)
    }

    function didMasterPressedNext(listElement) {
        console.log(listElement.barcode_code)
        var details = pushDetails(
            detailsItem
            , { "barcode_code" : listElement.barcode_code
            ,  "barcode_name" : listElement.barcode_name
            ,  "zint_code" : listElement.zint_code
            ,  "barcode_description" : listElement.barcode_description
            ,  "barcode_icon" : listElement.barcode_icon}
        )
    }

}





