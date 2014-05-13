#include "vigraqgraphicsscene.hxx"

template <class T>
VigraQGraphicsImageItem<T> * VigraQGraphicsScene::addImage(const vigra::BasicImage<T> * image,
                                                           T min,
                                                           T max,
                                                           QVector<QRgb> colors)
{
    VigraQGraphicsImageItem<T> * result_item = new VigraQGraphicsImageItem<T>(image, min, max, colors);
    addItem(result_item);
    
    return result_item;
}

template <class T>
VigraQGraphicsRGBImageItem<T> * VigraQGraphicsScene::addRGBImage(const vigra::BasicImage<vigra::RGBValue<T> > * image,
                                            vigra::RGBValue<T> min,
                                            vigra::RGBValue<T> max)
{
    VigraQGraphicsRGBImageItem<T> * result_item = new VigraQGraphicsRGBImageItem<T>(image, min, max);
    addItem(result_item);
    
    return result_item;
}

// Inherited overwritten frunction, which captures the mouse events of the scene
// If the mouse is moved, a signal will be emitted containing the point in scene coordinats.
// Warning: This signal is emitted iff the corresponding view-widget has 
//          the control over the mouse (e.g. using view->setMouseTracking(true)
void VigraQGraphicsScene::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
    QPointF p = mouseEvent->scenePos();
    emit mouseMoved(p);
}

template VigraQGraphicsImageItem<unsigned char> * VigraQGraphicsScene::addImage(const vigra::BasicImage<unsigned char> * image,
                                                                                unsigned char min,
                                                                                unsigned char max,
                                                                                QVector<QRgb> colors);

template VigraQGraphicsImageItem<float> * VigraQGraphicsScene::addImage(const vigra::BasicImage<float> * image,
                                                                        float min,
                                                                        float max,
                                                                        QVector<QRgb> colors);

template VigraQGraphicsImageItem<double> * VigraQGraphicsScene::addImage(const vigra::BasicImage<double> * image,
                                                                         double min,
                                                                         double max,
                                                                         QVector<QRgb> colors);


template VigraQGraphicsRGBImageItem<unsigned char> * VigraQGraphicsScene::addRGBImage(const vigra::BasicImage<vigra::RGBValue<unsigned char> > * image,
                                                                                      vigra::RGBValue<unsigned char> min,
                                                                                      vigra::RGBValue<unsigned char> max);

template VigraQGraphicsRGBImageItem<float> * VigraQGraphicsScene::addRGBImage(const vigra::BasicImage<vigra::RGBValue<float> > * image,
                                                                              vigra::RGBValue<float> min,
                                                                              vigra::RGBValue<float> max);

template VigraQGraphicsRGBImageItem<double> * VigraQGraphicsScene::addRGBImage(const vigra::BasicImage<vigra::RGBValue<double> > * image,
                                                                               vigra::RGBValue<double> min,
                                                                               vigra::RGBValue<double> max);
