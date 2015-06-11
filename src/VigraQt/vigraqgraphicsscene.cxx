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

void VigraQGraphicsScene::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
    QPointF p = mouseEvent->scenePos();
    emit mouseMoved(p);
}

/* Providing functions for:
 *  - unsigned char
 *  - float
 *  - double
 * pixel-types.
 */
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
