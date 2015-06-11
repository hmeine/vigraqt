/************************************************************************/
/*                                                                      */
/*                  Copyright 2003-2005 by Hans Meine                   */
/*                 meine@kogs.informatik.uni-hamburg.de                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*  This file is part of the VigraQt library.                           */
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

#ifndef FIMAGEVIEWER_HXX
#define FIMAGEVIEWER_HXX

#include "vigraqt_export.hxx"
#include <qwidget.h>
#include <qimage.h>
#include <vigra/stdimage.hxx>

class QImage;
class QImageViewer;
namespace vigra { class QByteImage; }

class VIGRAQT_EXPORT FImageViewer: public QWidget
{
	Q_OBJECT
	Q_PROPERTY(bool autoScaleMode READ autoScaleMode WRITE setAutoScaleMode)
	Q_PROPERTY(bool logarithmicMode READ logarithmicMode WRITE setLogarithmicMode)
	Q_PROPERTY(bool markingMode READ markingMode WRITE setMarkingMode)

public:
	FImageViewer(QWidget* parent = 0);
	~FImageViewer();

	virtual int originalWidth() const;
	virtual int originalHeight() const;
	virtual int zoomedWidth() const;
	virtual int zoomedHeight() const;

	float imageMin() const { return imageMin_; }
	float imageMax() const { return imageMax_; }
	float displayMin() const { return displayMin_; }
	float displayMax() const { return displayMax_; }

	const QImage &displayedImage() const;
	QImageViewer *imageViewer() const { return qimageviewer_; }

	bool autoScaleMode() const { return autoScaleMode_; }
	bool logarithmicMode() const { return logarithmicMode_; }
	bool markingMode() const { return markingMode_; }

public Q_SLOTS:
	// display a copy of the given image
	virtual void setImage( const vigra::FImage &newImage );

	void setAutoScaleMode( bool newMode );
	void autoScale();
	void setLogarithmicMode( bool newMode );
	void setMarkingMode( bool newMode );

	void displayMinMax(float min, float max);

Q_SIGNALS:
	void imageMinMaxChanged(float min, float max);
	void displayedMinMaxChanged(float min, float max);

protected:
	void redisplay(float min, float max);

	void preparePalette();

protected:
	QImageViewer *qimageviewer_;
	vigra::FImage *image_;
	vigra::QByteImage *qByteImage_;

	bool autoScaleMode_;
	bool logarithmicMode_;
	bool markingMode_;

	float imageMin_, imageMax_;
	float displayMin_, displayMax_;
};

#endif // FIMAGEVIEWER_HXX
