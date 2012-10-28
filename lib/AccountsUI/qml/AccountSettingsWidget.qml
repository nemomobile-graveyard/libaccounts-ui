// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: accountSettingsWidget

    signal saveSettings()
    signal cancel()
    signal removeAccount()
    function setProviderName(pName) {settingsHeader.title = pName}
    function setAccountDname(accountDName) {settingsHeader.subTitle = accountDName}
    function setProviderIcon(pIcon) {settingsHeader.iconName = pIcon}

    function setEnableAccount(accountStatus) {
        enableButton.checked = accountStatus}

    PageHeader {
        id: header
        text: "Settings"
    }

    Row {
        y: 100
        width: parent.width
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        BasicHeaderWidget {
            id: settingsHeader
        }

        CheckBox {
            id: enableButton
            onClicked: {
                accountSettings.enable(checked)
            }
        }
    }


    ServiceSettingsWidget {
        id: ssw
        objectName: "serviceListView"
        y: 200
        clip: true
        model: servicesModel
    }

    QueryDialog {
            id: queryDialog
            titleText: "Delete account"
            message: "All content related to this account will be deleted permanently"
            acceptButtonText: "Yes"
            rejectButtonText: "No"
            onAccepted: accountSettingsWidget.removeAccount()
    }

    Menu {
         id: mainMenu
         // define the items in the menu and corresponding actions
         content: MenuLayout {
             MenuItem {
                 text: "Delete"
                 onClicked: queryDialog.open()
             }
         }
     }

    tools: ToolBarLayout {
        id: toolBarLayout
        Row {
            spacing: 5
            Button {
                text: "Save"
                onClicked: accountSettingsWidget.saveSettings()
            }
            Button {
                text: "Cancel"
                onClicked: accountSettingsWidget.cancel()
            }
        }
        ToolIcon {
            iconId: "toolbar-view-menu"
            onClicked: mainMenu.open()
        }
    }

}
