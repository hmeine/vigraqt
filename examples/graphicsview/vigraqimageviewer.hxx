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

#ifndef VIGRAQIMAGEVIEWER_HXX
#define VIGRAQIMAGEVIEWER_HXX

#include <QMainWindow>
#include <QPrinter>

#include <vigraqt/vigraqgraphicsview.hxx>

namespace Ui {
    class VigraQImageViewerBase;
}

class VigraQImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    VigraQImageViewer(QWidget* parent = NULL);
    ~VigraQImageViewer();
    
private slots:
    void open();
    void print();
    void saveAsPDF();
    
	void zoomIn();
    void zoomOut();
    void normalSize();
    void updateImage();
	void removeImage();
	
    void about();
	
	void updateStatusBar(QPointF p);

private:
	void connectActions();
    
    vigra::FImage    *  m_grayImage;
	VigraQGraphicsImageItem<float>    *  m_grayImageItem;
    
    vigra::FRGBImage *  m_rgbImage;
	VigraQGraphicsRGBImageItem<float> *  m_rgbImageItem;
	
    VigraQGraphicsScene* m_scene;
	VigraQGraphicsView*  m_view;
    
    Ui::VigraQImageViewerBase* m_ui;
};

#endif
