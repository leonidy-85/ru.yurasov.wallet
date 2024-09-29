import QtQuick 2.0
import Sailfish.Silica 1.0
import QtPositioning 5.2
import Nemo.DBus 2.0
import "../tabview" as Tabs

Tabs.TabItem {
    id: page
    property string uid: "pkpassPage"
    property bool wide: (Screen.sizeCategory === Screen.Large || Screen.sizeCategory == Screen.ExtraLarge) && (mainapp.orientation === Orientation.Landscape || mainapp.orientation  === Orientation.LandscapeInverted)
    property bool displayOn: true

    Row {
        anchors.fill: parent

        SilicaListView {
            width: page.wide ? parent.width - passColumn.width : parent.width
            height: parent.height


            model: ListModel {
                id: passList
                ListElement { name: ""; relevantDate: ""; path: ""; points: -1; jsondata: ""; typeId: ""; updateable: false }
            }

            delegate: ListItem {
                id: entry
                contentHeight: passIcon.height + Theme.paddingSmall * 2

                Image {
                    id: passIcon
                    width: Theme.iconSizeLauncher
                    height: width
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: Theme.horizontalPageMargin
                    source: "image://zipimage" + path + "/icon.png"
                }

                Label {
                    text: name
                    textFormat: Text.PlainText
                    font.bold: passDisplay.status === Loader.Ready && passDisplay.item.path === path
                    width: parent.width - passIcon.width - Theme.horizontalPageMargin * 2 - Theme.paddingMedium
                    truncationMode: TruncationMode.Fade
                    color: entry.highlighted ? Theme.highlightColor : points != -1 ? Theme.primaryColor : Theme.secondaryColor
                    anchors.left: passIcon.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: Theme.paddingMedium
                }

                Label {
                    text: relevantDate
                    textFormat: Text.PlainText
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: Theme.fontSizeTiny
                    width: parent.width - passIcon.width - Theme.horizontalPageMargin * 2 - Theme.paddingMedium
                    truncationMode: TruncationMode.Fade
                    color: entry.highlighted ? Theme.highlightColor : points != -1 ? Theme.primaryColor : Theme.secondaryColor
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: Theme.horizontalPageMargin
                    anchors.bottomMargin: Theme.paddingSmall
                }

                menu: ContextMenu {

                    MenuItem {
                        text: qsTr("Show")
                        onClicked: {
                            var properties = { name: name, path: path, jsondata: jsondata, updateable: updateable };
                            pageStack.push(Qt.resolvedUrl("pkpass/ShowPass.qml"), properties);
                            pageStack.pushAttached(Qt.resolvedUrl("pkpass/ShowBack.qml"), properties);
                        }
                    }

                    MenuItem {
                        text: qsTr("Show simple")
                        onClicked: {
                               var properties = { path: path, jsondata: jsondata };
                               pageStack.push(Qt.resolvedUrl("pkpass/ShowSimple.qml"), properties);
                        }
                    }
                    MenuItem {
                        text: qsTr("Update")
                        visible: updateable
                        onClicked: {
                            passHandler.updatePass(path);
                        }
                    }

                    MenuItem {
                        text: qsTr("Delete")
                        onClicked: {
                            var delPath = path;
                            deleteRemorse.execute(entry, qsTr("Deleting"), function(){
                                passHandler.removePass(delPath);
                                for (var entry = 0; entry < passList.count; entry++) {
                                    if (passList.get(entry).path === delPath) {
                                        passList.remove(entry);
                                        break;
                                    }
                                }
                            });
                        }
                    }
                }

                onClicked: openPass(path, false)

                ListView.onAdd: AddAnimation {
                    target: entry
                }

                ListView.onRemove: RemoveAnimation {
                    target: entry
                }

                RemorseItem {
                    id: deleteRemorse
                }
            }

            VerticalScrollDecorator {}
        }

        SilicaFlickable {
            visible: page.wide && (!busy.running) && passList.count > 0
            width: Math.min(parent.width / 2, Theme.fontSizeMedium * 20)
            height: parent.height
            contentHeight: passColumn.height + 2 * Theme.paddingLarge

            Column {
                id: passColumn
                width: parent.width
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: Theme.paddingLarge
                spacing: Theme.paddingLarge

                Loader {
                    id: passDisplay
                    active: page.wide
                    enabled: parent.enabled
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: Qt.resolvedUrl("pkpass/Pass.qml")
                }

                Button {
                    text: qsTr("Create Calendar Entry")
                    visible: passDisplay.item.relevantDate !== ""
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        passHandler.createCalendarEntry(passList.get(getPass(passDisplay.item.path)).name, passDisplay.item.relevantDate);
                    }
                }

                Button {
                    text: qsTr("Update")
                    visible: passList.get(getPass(passDisplay.item.path)).updateable
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        passHandler.updatePass(passDisplay.item.path);
                    }
                }

                Loader {
                    id: backDisplay
                    active: page.wide
                    enabled: parent.enabled
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: Qt.resolvedUrl("pkpass/Back.qml")
                }
            }

            VerticalScrollDecorator {}
        }
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: true
    }

    Label {
        anchors.centerIn: parent
        text: qsTr("No passes found")
        color: Theme.highlightColor
        visible: passList.count == 0 && !busy.running
    }

    Timer {
        id: checkTimer
        interval: 60000
        repeat: true
        onTriggered: checkPassList()
    }

    PositionSource {
        id: locator
        property bool precise: false
        active: settingsStore.checkDistance && page.displayOn
        updateInterval: 60000
        preferredPositioningMethods: (precise || !settingsStore.useHere) ? PositionSource.AllPositioningMethods : PositionSource.NonSatellitePositioningMethods
        onPositionChanged: checkPassList()
    }

    Component.onCompleted: {
        // initial pass scan
        passList.clear();
        homeWatcher.scanHome();
    }

    Connections {
        target: homeWatcher
        onPassesFound: {
            // check for vanished passes...
            var removePasses = [];
            for (var oldpass = 0; oldpass < passList.count; oldpass++) {
                var found = false;
                for (var newpass = 0; newpass < list.length; newpass++) {
                    if (passList.get(oldpass).path === list[newpass].path) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    removePasses.push(passList.get(oldpass).path);
            }
            // ...and remove them
            for (var toRemove = 0; toRemove < removePasses.length; toRemove++)
                removePass(removePasses[toRemove]);
            // calculate the points sort the list, and update GPS precision
            var close = false;
            for (var pass = 0; pass < list.length; pass++) {
                if (calcPointsAndTime(list[pass]))
                    close = true;
            }
            list.sort(comparePasses);
            if (locator.precise !== close)
                locator.precise = close;
            // update the pass list
            updatePasses(list);
            // on the first run: stop the busy animation, start the check timer and show the pass called in the CLI (if given)
            if (busy.running) {
                busy.running = false;
                checkTimer.start();
                if (Qt.application.arguments.length === 2)
                    openPass(Qt.application.arguments[1]);
            }
            // report a successful update and redraw the pass, if it's shown
            if (update) {
                notificator.bannerNotification(qsTr("pass update successful"), "");
                if (pageStack.depth > 1)
                    openPass(pageStack.nextPage().path);
                if (page.wide && passDisplay.item.path !== '') {
                    try {
                        var showPass = passList.get(getPass(passDisplay.item.path));
                        passDisplay.item.path = showPass.path;
                        passDisplay.item.jsondata = showPass.jsondata;
                        backDisplay.item.jsondata = showPass.jsondata;
                    }
                    catch(e) {}
                }
            }
            // wide mode: show the first pass if there isn't one yet or if it vanished
            if (page.wide && (passDisplay.item.path === '' || getPass(passDisplay.item.path) === null)) {
                passDisplay.item.path = passList.get(0).path;
                passDisplay.item.jsondata = passList.get(0).jsondata;
                backDisplay.item.jsondata = passList.get(0).jsondata;
            }
        }
    }

    Connections {
        target: settingsStore
        onCheckTimeChanged: checkPassList()
        onHoursBeforeChanged: checkPassList()
        onHoursAfterChanged: checkPassList()
        onCheckDistanceChanged: checkPassList()
        onMaxDistanceChanged: checkPassList()
        onOverrideDistanceChanged: checkPassList()
    }

    Connections {
        target: passHandler
        onUpdateFinished: {
            switch (state) {
            case "not updateable":
                notificator.bannerNotification(qsTr("pass not updateable"), "");
                break;
            case "no new version":
                notificator.bannerNotification(qsTr("no new version for pass"), "");
                break;
            case "update failed":
                notificator.bannerNotification(qsTr("pass update failed"), "");
                break;
            case "ok":
                homeWatcher.scanHome(true);
            }
        }
        onCalendarEntryFinished: {
            if (state === "format")
                notificator.bannerNotification(qsTr("Format Error"), qsTr("Couldn't recognize date/time format"));
            if (state === "xdg-open")
                notificator.bannerNotification(qsTr("Unsupported"), qsTr("Please update your system or install calendar"));
        }
    }

    Connections {
        target: mainapp
        onOpenPass: {
            openPass(origin);
        }
        onOrientationChanged: {
            if (page.wide && (passDisplay.item.path === '' || getPass(passDisplay.item.path) === null)) {
                passDisplay.item.path = passList.get(0).path;
                passDisplay.item.jsondata = passList.get(0).jsondata;
                backDisplay.item.jsondata = passList.get(0).jsondata;
            }
        }
    }

    DBusAdaptor {
        id: dbus
        service: "ru.yurasov.wallet"
        iface: "ru.yurasov.wallet"
        path: "/ru/yurasov/wallet"
        xml: '<interface name="ch.yurasov.wallet">' +
             '  <method name="openPass">' +
             '    <arg name="origin" type="s" direction="in"/>' +
             '  </method>' +
             '</interface>'
        function openPass(origin) {
            page.openPass(origin);
        }
    }

    DBusInterface {
        id: dbus_mce
        bus: DBus.SystemBus
        iface: 'com.nokia.mce.signal'
        path: '/com/nokia/mce/signal'
        service: 'com.nokia.mce'
        signalsEnabled: true
        function display_status_ind(status) {
            if (status === "off")
                page.displayOn = false;
            else
                page.displayOn = true;
        }
    }

    function openPass(origin, immediate) {
        if (typeof immediate === 'undefined')
            immediate = true;
        // bring the app to the foreground
        mainapp.activate();
        // get the canonical path
        origin = passHandler.getCanonicalPath(origin);
        // look for a matching pass
        var pass = getPass(origin);
        if (pass !== null) {
            // found one: let's show it
            if (passDisplay.status === Loader.Ready && backDisplay.status === Loader.Ready) {
                // on wide screen
                passDisplay.item.path = passList.get(pass).path;
                passDisplay.item.jsondata = passList.get(pass).jsondata;
                backDisplay.item.jsondata = passList.get(pass).jsondata;
            }
            else {
                // on small screen
                var properties = { name: passList.get(pass).name, path: passList.get(pass).path, jsondata: passList.get(pass).jsondata, updateable: passList.get(pass).updateable };
                pageStack.pop(page, PageStackAction.Immediate);
                if (immediate)
                    pageStack.push(Qt.resolvedUrl("pkpass/ShowPass.qml"), properties, PageStackAction.Immediate);
                else
                pageStack.push(Qt.resolvedUrl("pkpass/ShowPass.qml"), properties);
                pageStack.pushAttached(Qt.resolvedUrl("pkpass/ShowBack.qml"), properties);
            }
        }
    }

    function updatePasses(newpasses) {
        // inserts, updates or moves the passes in the model
        for (var pass = 0; pass < newpasses.length; pass++) {
            var oldpoints = -1;
            if (pass < passList.count && passList.get(pass).path === newpasses[pass].path) {
                // update
                oldpoints = passList.get(pass).points;
                passList.set(pass, newpasses[pass]);
            }
            else {
                // check if it's further down
                var moved = false;
                for(var oldpass = pass + 1; oldpass < passList.count; oldpass++) {
                    if (passList.get(oldpass).path === newpasses[pass].path) {
                        // move and update
                        oldpoints = passList.get(oldpass).points;
                        passList.move(oldpass, pass, 1);
                        passList.set(pass, newpasses[pass]);
                        moved = true;
                        break;
                    }
                }
                if (!moved)
                    passList.insert(pass, newpasses[pass]);  // new pass
            }
            // update pass notifications
            if (oldpoints === -1 && newpasses[pass].points !== -1)
                notificator.addNotification(newpasses[pass].path, newpasses[pass].name, '');
            if (oldpoints !== -1 && newpasses[pass].points === -1)
                notificator.removeNotification(newpasses[pass].path);
        }
        // if the topmost pass is active, show it on the cover
        if (passList.count > 0 && passList.get(0).points !== -1) {
            var icon = "image://zipimage" + passList.get(0).path + "/icon.png";
            if (topIcon !== icon) {
                topIcon = icon;
                topPath = passList.get(0).path;
            }
        }
        else {
            if (topIcon !== "") {
                topIcon = "";
                topPath = "";
            }
        }
    }

    function getPass(path) {
        // gets the pass with the given path
        for (var pass = 0; pass < passList.count; pass++) {
            if (passList.get(pass).path === path)
                return pass;
        }
        return null;
    }

    function removePass(path) {
        // removes the pass with the given path
        var pass = getPass(path);
        if (pass !== null) {
            notificator.removeNotification(passList.get(pass).path);
            passList.remove(pass);
        }
    }

    function calcPointsAndTime(pass) {
        // calculates the relevancy points of a pass and says whether we're close to target coordinates
        // gets the relevant date and time if available
        /* Lower numbers are more relevant, but -1 means "not active".
           This is because "null" is not allowed in models. */
        pass.points = -1;
        var data = JSON.parse(pass.jsondata);
        var close = false;
        if ("relevantDate" in data) {
            pass.relevantDate = dateTimeFormat.format(data.relevantDate, "medium", "short", false);
        }
        else {
            pass.relevantDate = "";
        }
        if (settingsStore.checkTime && "relevantDate" in data) {
            // close to target time?
            try {
                var targetTime = new Date(data.relevantDate);
            }
            catch(e) {
                notificator.removeNotification(pass.path);
                return false;  // faulty pass
            }
            var now = new Date();
            var timeDiff = targetTime - now;  // time difference in milliseconds
            if (timeDiff >= 0 && timeDiff <= settingsStore.hoursBefore * 3600000) {
                pass.points = timeDiff / 1000;
            }
            else if (timeDiff < 0 && Math.abs(timeDiff) <= settingsStore.hoursAfter * 3600000) {
                pass.points = Math.abs(timeDiff) / 1000;
            }
        }
        if (pass.points === -1 && settingsStore.checkDistance && "locations" in data && locator.valid && locator.position.latitudeValid && locator.position.longitudeValid) {
            // close to one of the target destinations?
            var here = locator.position.coordinate;
            try {
                for (var location = 0; location < data.locations.length; location++) {
                    var there = QtPositioning.coordinate(data.locations[location].latitude, data.locations[location].longitude);
                    var posDiff = here.distanceTo(there);  // distance in meter
                    var maxDistance = settingsStore.maxDistance;
                    if (settingsStore.overrideDistance && "maxDistance" in data)
                        maxDistance = data.maxDistance;
                    if (posDiff <= maxDistance && (pass.points === -1 || pass.points > posDiff))
                        pass.points = posDiff;
                    if (posDiff <= maxDistance + 1000)
                        close = true;  // close enough to always check GPS
                }
            }
            catch (e) {
                notificator.removeNotification(pass.path);
                return false;  // faulty pass
            }
            if (pass.points !== -1)
                pass.points += 36000; // close to target time is more relevant than close to destination
        }
        return close;
    }

    function comparePasses(a, b) {
        // sort active passes to the top
        // "smaller" passes get sorted upwards
        if (a.points !== -1 && b.points === -1)
            return -1;
        if (a.points === -1 && b.points !== -1)
            return 1;
        // if both are active, check who's more relevant
        if (a.points !== b.points)
            return a.points - b.points;
        // group by pass type ID
        if (a.typeId !== b.typeId)
            return a.typeId.localeCompare(b.typeId);
        // otherwise order by name
        return a.name.localeCompare(b.name);
    }

    function checkPassList() {
        // recalculates all relevancy points, reorders the list and updates GPS precision
        var passes = [];
        var close = false;
        for (var pass = 0; pass < passList.count; pass++) {
            // we work with a copy
            var modelPass = passList.get(pass);
            var thisPass = { name: modelPass.name, relevantDate: modelPass.relevantDate, path: modelPass.path, points: modelPass.points, jsondata: modelPass.jsondata, typeId: modelPass.typeId, updateable: modelPass.updateable };
            if (calcPointsAndTime(thisPass))
                close = true;
            passes.push(thisPass);
        }
        passes.sort(comparePasses);
        if (locator.precise !== close)
            locator.precise = close;
        updatePasses(passes);
    }
}
