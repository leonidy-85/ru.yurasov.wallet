import QtQuick 2.0
import Aurora.Controls 1.0

Item {
    property SplitView splitView
    property bool isAlbum: false
    property bool isAllowCloseOnAlbum: false
    property bool isKeepBackStack: true

    function pushMaster(path) {
        var master = splitView.push(
                    Qt.createComponent(Qt.resolvedUrl(path))
                    , 4
                    , SplitView.Immediate
                )
        splitView.pinnedItem = master
        return master
    }

    function pushDetails(path, properties) {
       if (isAlbum && isAllowCloseOnAlbum) {
           return _replaceWithCloseAllowedOnAlbum(
               Qt.createComponent(Qt.resolvedUrl(path))
               , properties
               , 4
           )
       } else {
           return _replaceWithCloseForbiddenOnAlbum(
               Qt.createComponent(Qt.resolvedUrl(path))
               , properties
               , 4
           )
       }
    }

    function pushNext(path, properties) {
        splitView.setMaxActiveItems(
            _getMaxActiveItemsOnOrientation(false)
            , SplitView.Immediate
        )
        splitView.push(
            Qt.createComponent(Qt.resolvedUrl(path))
            , properties
            , 4
            , SplitView.Animated
        )
    }

    function splitNext(path, properties) {
        splitView.setMaxActiveItems(
            _getMaxActiveItemsOnOrientation(true)
            , SplitView.Immediate
        )
        splitView.push(
            Qt.createComponent(Qt.resolvedUrl(path))
            , properties
            , 4
            , SplitView.Animated
        )
    }

    function pop() { splitView.pop() }

    function updateToOrientation(isPortrait) {
        isAlbum = !isPortrait

        if (isPortrait) {
            splitView.setMaxActiveItems(1, SplitView.Immediate)
            splitView.setMinActiveItems(1, SplitView.Immediate)
        } else {
            splitView.setMaxActiveItems(2, SplitView.Immediate)

            var minActiveItemsCount = 2
            if (isAllowCloseOnAlbum) {
                minActiveItemsCount = 1
            }

            splitView.setMinActiveItems(minActiveItemsCount, SplitView.Immediate)
        }
    }

    function updateAllowCloseOnAlbum(allow) {
        isAllowCloseOnAlbum = allow
        updateToOrientation(!isAlbum)
    }

    function updateKeepBackStack(keep) {
        isKeepBackStack = keep
        updateToOrientation(!isAlbum)
    }

    function _replaceWithCloseForbiddenOnAlbum(target, properties, columnSpan) {
        var animation = SplitView.Animated
        if (splitView.count >= 2 && !isKeepBackStack) {
            animation = SplitView.Immediate
        }

        if (splitView.count > 1 && !isKeepBackStack) {
            splitView.pop(splitView.pinnedItem, SplitView.Immediate)
        }
        return splitView.push(target, properties, columnSpan, animation)
    }

    function _replaceWithCloseAllowedOnAlbum(target, properties, columnSpan) {
        var animation = SplitView.Animated
        if (splitView.count >= 2 && !isKeepBackStack) {
            animation = SplitView.Immediate
        }

        if (splitView.count > 1 && !isKeepBackStack) {
            splitView.setMinActiveItems(2, SplitView.Immediate)
            splitView.pop(splitView.pinnedItem, SplitView.Immediate)
            splitView.setMinActiveItems(1, SplitView.Immediate)
        }
        return splitView.push(target, properties, columnSpan, animation)
    }

    function _getMaxActiveItemsOnOrientation(isSplit) {
        if (isAlbum) {
            return isSplit ? 3 : 2
        } else {
            return 1
        }
    }
}
