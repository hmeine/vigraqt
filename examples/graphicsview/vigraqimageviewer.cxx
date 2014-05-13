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

#include <QtGui>

#include "vigraqimageviewer.hxx"
#include "ui_vigraqimageviewerbase.h"
#include "vigra/impex.hxx"

VigraQImageViewer::VigraQImageViewer(QWidget *parent)
: QMainWindow(parent),
  m_grayImage(NULL),
  m_grayImageItem(NULL),
  m_rgbImage(NULL),
  m_rgbImageItem(NULL),
  m_ui(new Ui::VigraQImageViewerBase)
{
    m_ui->setupUi(this);
	
	m_scene = new VigraQGraphicsScene;
	m_view  = new VigraQGraphicsView(m_scene, this);
	
	m_view->setMouseTracking(true);
    setCentralWidget(m_view);
    statusBar();
	
	connectActions();
}


VigraQImageViewer::~VigraQImageViewer()
{
    removeImage();
    
    delete m_view;
    delete m_scene;
}

void VigraQImageViewer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) 
	{
		try
		{
			const char * file =QFile::encodeName(fileName).data();
			vigra::ImageImportInfo info(file);
			
            //remove former loaded image
            removeImage();
            
			if( info.isGrayscale())
			{
                float min_gray = 0.0, max_gray = 255.0;
                
                if(m_ui->actionShow_normalized_image->isChecked())
                {
                     max_gray = min_gray;
                }
				m_grayImage = new vigra::FImage(info.size());
				vigra::importImage(info, vigra::destImage(*m_grayImage));	
                
                m_grayImageItem = m_scene->addImage(m_grayImage, min_gray, max_gray);
                m_scene->setSceneRect(m_grayImageItem->boundingRect());
			}
			else
			{
                vigra::RGBValue<float>  min_rgb = vigra::RGBValue<float>(0.0,0.0,0.0),
                                        max_rgb = vigra::RGBValue<float>(255.0,255.0,255.0);
                
                if(m_ui->actionShow_normalized_image->isChecked())
                {
                    max_rgb = min_rgb;
                }
                
				m_rgbImage = new vigra::FRGBImage(info.size());
				vigra::importImage(info, vigra::destImage(*m_rgbImage));
                
                m_rgbImageItem = m_scene->addRGBImage(m_rgbImage, min_rgb, max_rgb);
                m_scene->setSceneRect(m_rgbImageItem->boundingRect());
			}
            updateImage();
			
		}
		catch(vigra::StdException & e){
			QMessageBox::warning(this, tr("VIGRA Image Viewer"),
							   tr("<p><b>For some reasons, the image could not be loaded.</b></p> "
								  "<p>It looks like your image is somehow damaged...</p>"
								 "<p>And that's what the error reports:<br/><i>%01</i></p>").arg(e.what()));
								  
		}
    }
}		

void VigraQImageViewer::removeImage()
{
    if (m_grayImageItem != NULL)	
    {
        m_scene->removeItem(m_grayImageItem);
        delete m_grayImageItem;
        m_grayImageItem = NULL;
    }
    
    if (m_grayImage != NULL)	
    {
        delete m_grayImage;
        m_grayImage = NULL;
    }

    if (m_rgbImageItem != NULL)
    {
        m_scene->removeItem(m_rgbImageItem);
        delete m_rgbImageItem;	
        m_rgbImageItem = NULL;
    }
    
    if(m_rgbImage != NULL)
    {
        delete m_rgbImage;
        m_rgbImage = NULL;
    }
}

void VigraQImageViewer::print()
{
	QPrinter* printer = new QPrinter;
	
	QPrintDialog* dialog = new QPrintDialog(printer, this);
	dialog->setWindowTitle(tr("Print out canvas"));
	
    if ( dialog->exec() == QDialog::Accepted ) {
		QPainter pp(printer);
        m_scene->render(&pp);
    }
	delete dialog;
	delete printer;
	
}

void VigraQImageViewer::zoomIn()
{
    m_view->scale(1.25,1.25);
}

void VigraQImageViewer::zoomOut()
{
    m_view->scale(0.8,0.8);
}

void VigraQImageViewer::normalSize()
{	
	m_view->setTransform(QTransform());
}

void VigraQImageViewer::updateImage()
{	
	if(m_grayImageItem)
	{
        float min_gray = 0.0, max_gray = 255.0;
        
        if(m_ui->actionShow_normalized_image->isChecked())
        {
            max_gray = min_gray;
        }
        m_grayImageItem->setMinMax(min_gray, max_gray);
	}
    else if(m_rgbImageItem)
	{
        vigra::RGBValue<float>  min_rgb = vigra::RGBValue<float>(0.0,0.0,0.0),
                                max_rgb = vigra::RGBValue<float>(255.0,255.0,255.0);
        
        if(m_ui->actionShow_normalized_image->isChecked())
        {
            max_rgb = min_rgb;
        }
        
        m_rgbImageItem->setMinMax(min_rgb, max_rgb);
	}
}

void VigraQImageViewer::about()
{
    QMessageBox::about(this, tr("About VIGRA QImage Viewer"),
            tr("<p>The <b>VIGRA QImage Viewer</b> example shows how to combine VigraQt "
               "and derived classes of QGraphicsScene and QGraphicsView for displaying "
               "an grayscale image. </p>"
               "<p>Please note that, internally, the VigraQt bindings are only used to convert "
               "the Vigra BasicImages (for now: vigra::FImage) into QImages. The display and "
               "anteraction with the images is done using the QT Graphics-Framework, which was "
               "introduced in early 4.X versions of Qt.</p>"
               "<p>Thus, it is very easy to add additional object to mark certain image "
			   "features or to stack images. Further, it is very easy to create additional "
			   "views on the scene and to control visibility of the scene's items.</p> "
			   "<p>Last but not least, the View of a GraphicsScene can easily be printed out, "
			   "saved as PDF or rendered with OpenGL by means of QGL</p>"));
}

void VigraQImageViewer::connectActions()
{
	//Menu actions
    connect(m_ui->actionLoad_image, SIGNAL(triggered()), this, SLOT(open()));
	connect(m_ui->actionPrint, SIGNAL(triggered()), this, SLOT(print()));
	connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
	
    connect(m_ui->actionZoom_in,  SIGNAL(triggered()), this, SLOT(zoomIn()));
	connect(m_ui->actionZoom_out, SIGNAL(triggered()), this, SLOT(zoomOut()));
	connect(m_ui->actionReset_zoom, SIGNAL(triggered()), this, SLOT(normalSize()));
	connect(m_ui->actionShow_normalized_image, SIGNAL(triggered()), this, SLOT(updateImage()));
	
	connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	
	//Mouse signals
	connect(m_scene, SIGNAL(mouseMoved(QPointF)), this, SLOT(updateStatusBar(QPointF)));
}

void VigraQImageViewer::updateStatusBar(QPointF p)
{
	//If currently loaded image is of gray-scale
	if (m_grayImage)
    {        
        if(m_grayImage->isInside(vigra::Diff2D(p.x(),p.y())))
        {
            statusBar()->showMessage(QString("Image (%01,%02) := %03").arg(p.x()).arg(p.y()).arg((*m_grayImage)(p.x(),p.y())));	
        }	
	}
    //If currently loaded image is colorful
	else if (m_rgbImage)
    {        
        if(m_rgbImage->isInside(vigra::Diff2D(p.x(),p.y())))
        {
            statusBar()->showMessage(QString("Image (%01,%02) := [R:%03, G:%04, B:%05]").arg(p.x()).arg(p.y()).arg((*m_rgbImage)(p.x(),p.y())[0]).arg((*m_rgbImage)(p.x(),p.y())[1]).arg((*m_rgbImage)(p.x(),p.y())[2]));	
        }
    }
	else
	{
		statusBar()->showMessage(tr("No image loaded yet"));
	}
}