/************************************************************************/
/*                                                                      */
/*               Copyright 2007-2010 by Benjamin Seppke                 */
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

#ifndef VIGRAQGRAPHICSVIEW_HXX
#define VIGRAQGRAPHICSVIEW_HXX

#include "vigraqt_export.hxx"

#include "vigraqgraphicsscene.hxx"
#include <QGraphicsView>
#include <QGraphicsItem>

//Uncomment the following line to enable OpenGL support
//#include <QGLWidget>


class VIGRAQT_EXPORT VigraQGraphicsView : public QGraphicsView
{
    Q_OBJECT

	public:
    VigraQGraphicsView(VigraQGraphicsScene* c,
                       QWidget* parent = NULL,
                       const char * name="",
                       Qt::WindowFlags f = 0);
};

#endif