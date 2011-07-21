#ifndef IMAGECONTENTPROVIDER_H
#define IMAGECONTENTPROVIDER_H

#include <gallerycontentprovider.h>

class GalleryModel;
class TrackerLiveQuery;

//! Class ImageContentProvider is a concrete class for getting
//! the list of all images in the device from Tracker.
class ImageContentProvider : public GalleryContentProvider
{
    Q_OBJECT

public:
    //! Constructor.
    //! \param galleryModel.
    //! \param parent Optional parent.
    explicit ImageContentProvider(GalleryModel& galleryModel, QObject* parent = 0);

    //! Destructor.
    virtual ~ImageContentProvider();

public: // from GalleryContentProvider
    //! \reimp
    QAbstractItemModel* model() const;

    //! \reimp
    virtual void queryContent(int limit=-1);

    //! \reimp
    virtual bool isLoadingContent() const;

private Q_SLOTS:
    void onInitialQueryFinished();

private:

    Q_DISABLE_COPY(ImageContentProvider);
    TrackerLiveQuery *liveQuery;
    bool queryRunning;
};

#endif // IMAGECONTENTPROVIDER_H
