/************************************************************************/
/*                                                                      */
/*               Copyright 2007-2014 by Benjamin Seppke                 */
/*                  seppke@informatik.uni-hamburg.de                    */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*  This file may become a part of the VigraQt library.                 */
/*                                                                      */
/*  VigraQt is free software; you can redistribute it and/or modify it  */
/*  under the terms of the GNU General Public License as published by   */
/*  the Free Software Foundation; either version 2 of the License, or   */
/*  (at your option) any later version.                                 */
/*                                                                      */
/*  VigraQt is distributed in the hope that it will be useful, but      */
/*  WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                */
/*  See the GNU General Public License for more details.                */
/*                                                                      */
/*  You should have received a copy of the GNU General Public License   */
/*  along with VigraQt; if not, write to the                            */
/*                   Free Software Foundation, Inc.,                    */
/*       51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA         */
/*                                                                      */
/************************************************************************/

#ifndef VIGRAQGRAPHICSIMAGEITEM_HXX
#define VIGRAQGRAPHICSIMAGEITEM_HXX

#include "vigraqt_export.hxx"

#include <QGraphicsPixmapItem>
#include <vigra/stdimage.hxx>

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
