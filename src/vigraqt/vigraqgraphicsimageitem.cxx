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

#include "vigraqgraphicsimageitem.hxx"
#include "createqimage.hxx"

template <class T>
VigraQGraphicsImageItem<T>::VigraQGraphicsImageItem(const vigra::BasicImage<T> * image,
                                                 T min,
                                                 T max,
                                                 QVector<QRgb> colors,
                                                 QGraphicsItem * parent)
:   QGraphicsPixmapItem(parent),
    m_image(image),
    m_min(min),
    m_max(max),
    m_colors(colors)
{
    updateImagePixmap();
}
    

template <class T>
void VigraQGraphicsImageItem<T>::setMinMax(T min, T max)
{
    m_min = min;
    m_max = max;
    
    updateImagePixmap();
}

template <class T>
void VigraQGraphicsImageItem<T>::resetMinMax()
{
    m_min = vigra::NumericTraits<T>::zero();
    m_max = vigra::NumericTraits<T>::zero();
    
    updateImagePixmap();
}

template <class T>
T VigraQGraphicsImageItem<T>::min() const
{
    return m_min;
}

template <class T>
T VigraQGraphicsImageItem<T>::max() const
{
    return m_max;
}

template <class T>
bool VigraQGraphicsImageItem<T>::isNormalized() const
{
    return (m_min == 0) && (m_max ==0);
}


template <class T>
const vigra::BasicImage<T> * VigraQGraphicsImageItem<T>::image() const
{
    return m_image;
}

template <class T>
void VigraQGraphicsImageItem<T>::setImage(vigra::BasicImage<T> * image)
{
    m_image = image;
    updateImagePixmap();
}


template <class T>
QVector<QRgb> VigraQGraphicsImageItem<T>::colors() const
{
    return m_colors;
}

template <class T>
void VigraQGraphicsImageItem<T>::setColors(QVector<QRgb> colors)
{
    m_colors = colors;
}

template <class T>
void VigraQGraphicsImageItem<T>::updateImagePixmap()
{
    if(imageInitialized())
    {
        //Create the QImage for the vigra-image
        QImage * qimg = createQImage(srcImageRange(*m_image), m_min, m_max);
        
        if(!m_colors.empty())
            qimg->setColorTable(m_colors);
        
        setPixmap(QPixmap::fromImage(*qimg));
        
        //Free memory of temporary QImage
        delete qimg;
    }
}

template <class T>
bool VigraQGraphicsImageItem<T>::imageInitialized() const
{
    return  m_image != NULL;
}


//Provide implementation for most common pixel types:
template class VigraQGraphicsImageItem<unsigned char>;
template class VigraQGraphicsImageItem<float>;
template class VigraQGraphicsImageItem<double>;









template <class T>
VigraQGraphicsRGBImageItem<T>::VigraQGraphicsRGBImageItem(const vigra::BasicImage<vigra::RGBValue<T> > * image,
                               vigra::RGBValue<T> min,
                               vigra::RGBValue<T> max,
                               QGraphicsItem * parent)
: QGraphicsPixmapItem(parent),
  m_image(image),
  m_min(min),
  m_max(max)
{
    updateImagePixmap();
}


template <class T>
void VigraQGraphicsRGBImageItem<T>::setMinMax(vigra::RGBValue<T> min, vigra::RGBValue<T> max)
{
    m_min = min;
    m_max = max;
    
    updateImagePixmap();
}

template <class T>
void VigraQGraphicsRGBImageItem<T>::resetMinMax()
{
    m_min = vigra::NumericTraits<vigra::RGBValue<T> >::zero();
    m_max = vigra::NumericTraits<vigra::RGBValue<T> >::zero();
    
    updateImagePixmap();
}

template <class T>
 vigra::RGBValue<T> VigraQGraphicsRGBImageItem<T>::min() const
{
    return m_min;
}

template <class T>
 vigra::RGBValue<T> VigraQGraphicsRGBImageItem<T>::max() const
{
    return m_max;
}

template <class T>
bool VigraQGraphicsRGBImageItem<T>::isNormalized() const
{
    vigra::RGBValue<T> zero = vigra::NumericTraits<vigra::RGBValue<T> >::zero();
    return (m_min == zero) && (m_max == zero);
}


template <class T>
const vigra::BasicImage<vigra::RGBValue<T> > * VigraQGraphicsRGBImageItem<T>::image() const
{
    return m_image;
}

template <class T>
void VigraQGraphicsRGBImageItem<T>::setImage(vigra::BasicImage<vigra::RGBValue<T> > * image)
{
    m_image = image;
    updateImagePixmap();
}


template <class T>
void VigraQGraphicsRGBImageItem<T>::updateImagePixmap()
{
    if(imageInitialized())
    {
        //Create the QImage for the vigra-image
        QImage * qimg = createQImage(srcImageRange(*m_image), m_min, m_max);
        
        setPixmap(QPixmap::fromImage(*qimg));
        
        //Free memory of temporary QImage
        delete qimg;
    }
}

template <class T>
bool VigraQGraphicsRGBImageItem<T>::imageInitialized()
{
    return m_image != NULL;
}

//Provide implementation for most common pixel types:
template class VigraQGraphicsRGBImageItem<unsigned char>;
template class VigraQGraphicsRGBImageItem<float>;
template class VigraQGraphicsRGBImageItem<double>;
