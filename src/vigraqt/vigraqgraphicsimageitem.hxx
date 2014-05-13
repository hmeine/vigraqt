/************************************************************************/
/*                                                                      */
/*                  Copyright 2008-2012 by Hans Meine                   */
/*                 meine@kogs.informatik.uni-hamburg.de                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/************************************************************************/

#ifndef VigraQGraphicsImageItem_HXX
#define VigraQGraphicsImageItem_HXX

#include "vigraqt_export.hxx"

//qt
#include <QGraphicsPixmapItem>

//the vigra stuff
#include "vigra/stdimage.hxx"

template <class T>
class VIGRAQT_EXPORT VigraQGraphicsImageItem : public QGraphicsPixmapItem
{
public:
    VigraQGraphicsImageItem(const vigra::BasicImage<T> * image = NULL,
                            T min = vigra::NumericTraits<T>::zero(),
                            T max = vigra::NumericTraits<T>::zero(),
                            QVector<QRgb> colors = QVector<QRgb>(),
                            QGraphicsItem * parent = NULL);
    
    void setMinMax(T min, T max);
    void resetMinMax();
    T min() const;
    T max() const;
    bool isNormalized() const;
    
    const vigra::BasicImage<T> * image() const;
    void setImage(vigra::BasicImage<T> * image);
    
    QVector<QRgb> colors() const;
    void setColors(QVector<QRgb> colors);
    
protected:
    void updateImagePixmap();
    bool imageInitialized() const;
    
    const vigra::BasicImage<T> * m_image;
    T m_min, m_max;
    QVector<QRgb> m_colors;
};




template <class T>
class VigraQGraphicsRGBImageItem : public QGraphicsPixmapItem
{
public:
    VigraQGraphicsRGBImageItem(const vigra::BasicImage<vigra::RGBValue<T> > * image = NULL,
                               vigra::RGBValue<T> min = vigra::NumericTraits<vigra::RGBValue<T> >::zero(),
                               vigra::RGBValue<T> max = vigra::NumericTraits<vigra::RGBValue<T> >::zero(),
                               QGraphicsItem * parent = NULL);
    
    void setMinMax(vigra::RGBValue<T> min, vigra::RGBValue<T> max);
    void resetMinMax();
    
    vigra::RGBValue<T> min() const;
    
    vigra::RGBValue<T> max() const;
    
    bool isNormalized() const;
    
    const vigra::BasicImage<vigra::RGBValue<T> > * image() const;
    void setImage(vigra::BasicImage<vigra::RGBValue<T> > * image);
    
protected:	
    void updateImagePixmap();
    bool imageInitialized();
    
    const vigra::BasicImage<vigra::RGBValue<T> > * m_image;
    vigra::RGBValue<T> m_min, m_max;
};


#endif // VigraQGraphicsImageItem_HXX
