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

#include "fimageviewer.hxx"
#include "qimageviewer.hxx"

#include <vigraqimage.hxx>

#include <vigra/inspectimage.hxx>
#include <vigra/copyimage.hxx>
#include <vigra/stdimagefunctions.hxx>

#include <QImage>
#include <QLayout>
#include <Q3VBoxLayout>

#include <math.h>

FImageViewer::FImageViewer(QWidget* parent)
: QWidget(parent),
  qimageviewer_(new QImageViewer(this)),
  image_(0),
  qByteImage_(0),
  autoScaleMode_(true),
  logarithmicMode_(false),
  markingMode_(false)
{
	QLayout *imageLayout= new Q3VBoxLayout( this );
	imageLayout->add(qimageviewer_);
}

FImageViewer::~FImageViewer()
{
	delete qByteImage_;
	delete image_;
}

int FImageViewer::originalWidth() const
{
    return qimageviewer_->originalWidth();
}

int FImageViewer::originalHeight() const
{
    return qimageviewer_->originalHeight();
}

int FImageViewer::zoomedWidth() const
{
    return qimageviewer_->zoomedWidth();
}

int FImageViewer::zoomedHeight() const
{
    return qimageviewer_->zoomedHeight();
}

const QImage &FImageViewer::displayedImage() const
{
	return qByteImage_->qImage();
}

void FImageViewer::setImage(const vigra::FImage &newImage)
{
	delete image_;
	image_= new vigra::FImage(newImage);

    vigra::FindMinMax<float> minmax;
    vigra::inspectImage(srcImageRange(*image_), minmax);
	imageMin_= minmax.min;
	imageMax_= minmax.max;
	emit imageMinMaxChanged(imageMin_, imageMax_);

    delete qByteImage_;
	qByteImage_ = new vigra::QByteImage(image_->width(), image_->height());
	preparePalette();

	if(autoScaleMode_)
	{
		displayMin_= 1.0f; displayMax_= 0.0f; // make values "dirty"
		autoScale();
	}
	else
		redisplay(displayMin_, displayMax_);
}

void FImageViewer::preparePalette()
{
    if(!qByteImage_)
        return;
	for(int i=0; i<256; ++i)
		qByteImage_->qImage().setColor(i, qRgb(i,i,i));
	if(markingMode_)
		qByteImage_->qImage().setColor(255, qRgb(255,0,0));
}

void FImageViewer::setAutoScaleMode(bool newMode)
{
	if(autoScaleMode_ != newMode)
	{
		autoScaleMode_= newMode;
		if(autoScaleMode_)
			autoScale();
	}
}

void FImageViewer::autoScale()
{
	displayMinMax(imageMin(), imageMax());
}

void FImageViewer::setLogarithmicMode(bool newMode)
{
	if(logarithmicMode_ != newMode)
	{
		logarithmicMode_= newMode;
		redisplay(displayMin_, displayMax_);
	}
}

void FImageViewer::setMarkingMode(bool newMode)
{
	if(markingMode_ != newMode)
	{
		markingMode_= newMode;
		preparePalette();
		redisplay(displayMin_, displayMax_);
	}
}

void FImageViewer::displayMinMax(float min, float max)
{
	if((displayMin_!=min) || (displayMax_!=max))
	{
		displayMin_= min;
		displayMax_= max;

		redisplay(min, max);
	}
}

struct FloatToByteFunctor
{
	float offset_, scale_;
	FloatToByteFunctor(float min, float max)
		: offset_(-min), scale_(min==max? 1.0 : 255.0 / (max-min)) {};
	uchar operator()(float f) const
		{ return (uchar)(scale_ *(f + offset_)); }
};

struct FloatToByteMarkFunctor
{
	float offset_, scale_, min_, max_;
	FloatToByteMarkFunctor(float min, float max)
		: offset_(-min), scale_(min==max? 1.0 : 254.0 / (max-min)),
		  min_(min), max_(max) {};
	uchar operator()(float f) const
		{ return (f<min_)||(f>max_)? (uchar)255 : (uchar)(scale_ *(f + offset_)); }
};

struct FloatToByteLogFunctor
{
	float scale_;
	FloatToByteLogFunctor(float max)
		: scale_(255.0/log(max)) {};
	uchar operator()(float f) const
		{ return f<=1? 0: (uchar)(scale_*log(f)); }
};

struct FloatToByteLogMarkFunctor
{
	float scale_, max_;
	FloatToByteLogMarkFunctor(float max)
		: scale_(254.0/log(max)), max_(max) {};
	uchar operator()(float f) const
		{ return f<=max_? (f<=1? 0: (uchar)(scale_*log(f))) : 255; }
};

void FImageViewer::redisplay(float min, float max)
{
    if(!image_ || !qByteImage_)
        return;

	if(!logarithmicMode_)
		if(!markingMode_)
			vigra::transformImage(srcImageRange(*image_), destImage(*qByteImage_),
								  FloatToByteFunctor(min, max));
		else
			vigra::transformImage(srcImageRange(*image_), destImage(*qByteImage_),
								  FloatToByteMarkFunctor(min, max));
	else
		if(!markingMode_)
			vigra::transformImage(srcImageRange(*image_), destImage(*qByteImage_),
								  FloatToByteLogFunctor(max));
		else
			vigra::transformImage(srcImageRange(*image_), destImage(*qByteImage_),
								  FloatToByteLogMarkFunctor(max));

    qimageviewer_->setImage(qByteImage_->qImage());

	//cerr << "redisplayed, emitting... " << min << "," << max << "\n";
	emit displayedMinMaxChanged(min, max);
}
