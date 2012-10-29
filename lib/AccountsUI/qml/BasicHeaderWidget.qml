// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    id: headerWidget
    property string title : "title"
    property string subTitle: "subTitle"
    property string iconName : "default-name"


        Row {
            id: row
            width: parent.width
            height: 50

            Image {
                id: imageitem
                fillMode: Image.PreserveAspectFit
                //TODO: Replace the below image from the imageName
                source: "image://theme/" + iconName;
            }
            Label {
                id: titleLabel
                height: parent.height
                text: headerWidget.title
                font.pixelSize: 25
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }
        }

        Label {
            id: subTitleLabel
            text: headerWidget.subTitle
            anchors.top: row.bottom
            width: parent.width;
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
        }
    }
