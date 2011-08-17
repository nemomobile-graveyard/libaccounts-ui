#ifndef AVATARSELECTOR_H
#define AVATARSELECTOR_H

#include <QObject>
#include <QImage>
#include <QUrl>

#include <gallerygridpage.h>
#include <galleryfullscreenpage.h>
#include <gallerymodel.h>

#include "imagecontentprovider.h"

namespace AccountsUI {

class AvatarSelector : public QObject
{
    Q_OBJECT
public:
    AvatarSelector(QObject *parent = NULL);
    ~AvatarSelector();
    void launch();
    void init();

signals:
    void avatarSelected(const QImage &image);

private Q_SLOTS:
    void onGridItemSelected(QUrl url);
    void onCroppingDone(QImage image);
    void onSingleSelectionCancelled();

private:
    GalleryModel *galleryModel;
    ImageContentProvider *imageContentprovider;
    GalleryGridPage *gridPage;
    GalleryFullScreenPage *fullScreenPage;
    bool alreadyInitialized;

};
}

#endif // AVATARSELECTOR_H
