import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.XmlListModel 2.0

Page {
    id: page

    // The effective value will be restricted by ApplicationWindow.allowedOrientations
    allowedOrientations: Orientation.All

    property bool downloadError: false

    signal selected(string title, string hostname, string searchEngine)

    function getOpensearch(title, host) {
        if (host.indexOf("http") !== 0) {
            var newhost = "http://" + host
        } else {
            newhost = host
        }

        selected(title, "", newhost)
    }

    function getEngine(host) {
        if (host.indexOf("http") !== 0) {
            var newhost = "http://" + host
        } else {
            newhost = host
        }

        searchModel.clear()
        downloadError = false

        var request = new XMLHttpRequest();
        request.onreadystatechange = function() {
            if (request.status && request.status != 200 && request.status != 301) {
                downloadBusy.visible = false
            }
            if (!request.readyState || request.readyState !== XMLHttpRequest.DONE) {
                return
            }
            var location = request.getResponseHeader("location")
            var osindex = request.responseText.indexOf("application/opensearchdescription+xml")
            if (osindex > 0) {
                var search = request.responseText.match(/<link\s[^>]*type=\"application\/opensearchdescription\+xml\"[^>]*>/i)
                search.forEach(function(link) {
                    var attrs = {}
                    var re = /(\w+)=["']?([^"'\s]*)["']?/g
                    var m
                    while (m = re.exec(link)) {
                        attrs[m[1]] = m[2]
                    }
                    attrs["location"] = location
                    console.log(JSON.stringify(attrs))
                    searchModel.append(attrs)
                })
            } else {
                downloadError = true
            }

            downloadBusy.visible = false
        }

        request.open("GET", newhost);
        request.send();
    }

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: column.height

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: qsTr("Add search engine")
            }
            TextField {
                id: hostField
                width: parent.width
                placeholderText: qsTr("Search hostname")
                label: placeholderText
                focus: true
                inputMethodHints: Qt.ImhUrlCharactersOnly
                EnterKey.iconSource: "image://theme/icon-m-cloud-download"
                EnterKey.onClicked: {
                    if (downloadButton.enabled) {
                        downloadBusy.visible = true
                        getEngine(hostField.text)
                    }
                }
            }

            Button {
                id: downloadButton
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Get search engine")
                enabled: hostField.text && !downloadBusy.visible
                onClicked: {
                    downloadBusy.visible = true
                    getEngine(hostField.text)
                }
            }

            BusyIndicator {
                id: downloadBusy
                anchors.horizontalCenter: parent.horizontalCenter
                size: BusyIndicatorSize.Large
                running: visible
                visible: false
            }

            SectionHeader {
                text: qsTr("Available search engines")
                visible: searchModel.count > 0
            }

            Repeater {
                model: ListModel {
                    id: searchModel
                }
                delegate: BackgroundItem {
                    id: delegate

                    Label {
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingLarge
                        text: title
                        anchors.verticalCenter: parent.verticalCenter
                        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                    }

                    onClicked:  {
                        console.log("$$", title, href)
                        selected(title, location, href)
                        installingBusy.visible = true
                    }
                }
            }

            BusyIndicator {
                id: installingBusy
                anchors.horizontalCenter: parent.horizontalCenter
                size: BusyIndicatorSize.Large
                running: visible
                visible: false
            }

            TextField {
                id: opensearchTitle
                width: parent.width
                placeholderText: qsTr("Search engine title")
                label: placeholderText
                visible: downloadError
                text: "Google"
                EnterKey.iconSource: "image://theme/icon-m-next"
                EnterKey.onClicked: {
                    opensearchLink.forceActiveFocus()
                }
            }

            TextField {
                id: opensearchLink
                width: parent.width
                placeholderText: qsTr("Opensearch link")
                label: placeholderText
                visible: downloadError
                text: "https://www.google.com/searchdomaincheck?format=opensearch"
                inputMethodHints: Qt.ImhUrlCharactersOnly
                EnterKey.iconSource: "image://theme/icon-m-cloud-download"
                EnterKey.onClicked: {
                    if (opensearchButon.enabled) {
                        installingBusy.visible = true
                        getOpensearch(opensearchTitle.text, opensearchLink.text)
                    }
                }
            }

            Button {
                id: opensearchButon
                visible: downloadError
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Get opensearch")
                enabled: opensearchLink.text && opensearchTitle.text && !downloadBusy.visible
                onClicked: {
                    installingBusy.visible = true
                    getOpensearch(opensearchTitle.text, opensearchLink.text)
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                height: Theme.itemSizeExtraSmall
                width: (parent ? parent.width : Screen.width) - x*2
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: Theme.highlightColor
                visible: searchModel.count > 0 || downloadError
                text: installingBusy.visible
                        ? qsTr("Installing search engine...")
                        : downloadError
                          ? qsTr("Can't find opensearch description. Please contact search provider and request adding opensearch description to html head.")
                          : qsTr("Click on search engine above to add")
            }
        }
    }
}
