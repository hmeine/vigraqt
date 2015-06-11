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

#ifndef VIGRAQGRAPHICSSCENE_HXX
#define VIGRAQGRAPHICSSCENE_HXX

#include "vigraqt_export.hxx"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <vigra/stdimage.hxx>
#include "vigraqgraphicsimageitem.hxx"


class VIGRAQT_EXPORT VigraQGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

signals:
	/* Signal, which will be emitted each time the mouse is moved over the scene.
	 * Warning: This signal is emitted iff the corresponding view-widget has
	 *          the control over the mouse (e.g. using view->setMouseTracking(true)
     */
	void mouseMoved(QPointF);

public:

	/* This function can only handle scalar images
     * It will be implemented by means of the cxx-file for:
     *  - unsigned char
     *  - float
     *  - double
     * pixel-types.
     */
	template <class T>
	VigraQGraphicsImageItem<T> * addImage(const vigra::BasicImage<T> * image,
                                          T min = vigra::NumericTraits<T>::zero(),
                                          T max = vigra::NumericTraits<T>::zero(),
                                          QVector<QRgb> colors = QVector<QRgb>());
    
	/* This function can only handle rgb-valued images
     * It will be implemented by means of the cxx-file for:
     *  - unsigned char
     *  - float
     *  - double
     * pixel-types (for each channel).
     */
	template <class T>
	VigraQGraphicsRGBImageItem<T> * addRGBImage(const vigra::BasicImage<vigra::RGBValue<T> > * image,
                                                vigra::RGBValue<T> min = vigra::NumericTraits<vigra::RGBValue<T> >::zero(),
                                                vigra::RGBValue<T> max = vigra::NumericTraits<vigra::RGBValue<T> >::zero());
    
	/* Inherited overwritten frunction, which captures the mouse events of the scene
	 * If the mouse is moved, a signal will be emitted containing the point in scene coordinats.
	 * Warning: This signal is emitted iff the corresponding view-widget has
	 *          the control over the mouse (e.g. using view->setMouseTracking(true)
     */
	void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
};

#endif
