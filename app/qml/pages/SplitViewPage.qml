import QtQuick 2.6
import Sailfish.Silica 1.0
import Aurora.Controls 1.0
import "../tabview" as Tabs
import "../components"

Tabs.TabItem  {

  id: splitViewPage
  objectName: "splitViewPage"
  property bool blockResetSelection: false

  SplitView {
      id: splitView
      anchors.fill: parent
      splitter: Rectangle {
          color: Theme.rgba(Theme.highlightColor, Theme.opacityLow)
          width: 2
      }

      Component.onCompleted: coordinator.updateToOrientation(isPortraitOrientation())
  }

  Coordinator {
      id: coordinator
      splitView: splitView
  }

  Component.onCompleted: {
      coordinator.start()
      coordinator.updateAllowCloseOnAlbum(true)
      coordinator.updateKeepBackStack(false)
  }


  function isPortraitOrientation() {
      return orientation === Orientation.Portrait || orientation === Orientation.PortraitInverted
  }
}
