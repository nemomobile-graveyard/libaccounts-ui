#include <QDebug>
#include <MApplication>
#include <MApplicationWindow>
#include <MSheet>

#include "avatar-selector.h"

using namespace AccountsUI;

AvatarSelector::AvatarSelector(QObject *parent)
    : QObject(parent)
{
    galleryModel = new GalleryModel();
    imageContentprovider = new ImageContentProvider(*galleryModel);
    galleryModel->addContentProvider(imageContentprovider);
    gridPage = new GalleryGridPage(*galleryModel);
    fullScreenPage = new GalleryFullScreenPage(*galleryModel);
    alreadyInitialized = false;
}

AvatarSelector::~AvatarSelector()
{
    qDebug() << Q_FUNC_INFO;

    if (fullScreenPage != NULL) {
        delete fullScreenPage;
        fullScreenPage = NULL;
    }

    if (gridPage != NULL) {
        delete gridPage;
        gridPage = NULL;
    }

    if (imageContentprovider != NULL) {
        delete imageContentprovider;
        imageContentprovider = NULL;
    }

    if (galleryModel != NULL) {
        delete galleryModel;
        galleryModel = NULL;
    }
}

void AvatarSelector::init()
{
    if (!alreadyInitialized) {
        // Grid page
        gridPage->setTopBarText(//% "Set as avatar"
                qtTrId("qtn_comm_set_avatar"));
        gridPage->showTopBar(true);
        gridPage->selectItem();
        gridPage->setNavigationBarVisible(true);
        connect(gridPage, SIGNAL(itemSelected(QUrl)), this, SLOT(onGridItemSelected(QUrl)));
        connect(gridPage, SIGNAL(singleSelectionCancelled()), this, SLOT(onSingleSelectionCancelled()));

        // Fullscreen page
        connect(fullScreenPage, SIGNAL(croppingDone(QImage)), this, SLOT(onCroppingDone(QImage)));
        connect(fullScreenPage, SIGNAL(croppingCancelled()), &fullScreenPage->sheet(), SLOT(disappear()));
        alreadyInitialized = true;
    }
}

void AvatarSelector::launch()
{
    qDebug() << Q_FUNC_INFO;
    gridPage->sheet().appear(MApplication::instance()->activeWindow());
}

void AvatarSelector::onGridItemSelected(QUrl url)
{
    qDebug() << Q_FUNC_INFO << url;
    fullScreenPage->openItemInCropper(url);
    fullScreenPage->sheet().appear(MApplication::instance()->activeWindow());
}

void AvatarSelector::onCroppingDone(QImage image)
{
    qDebug() << Q_FUNC_INFO;
    connect(&gridPage->sheet(), SIGNAL(disappeared()),
            &fullScreenPage->sheet(), SLOT(disappear()), Qt::UniqueConnection);
    gridPage->sheet().disappear();
    emit avatarSelected(image);
}

void AvatarSelector::onSingleSelectionCancelled()
{
    gridPage->sheet().disappear();
}


