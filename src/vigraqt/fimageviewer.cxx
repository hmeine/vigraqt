#include "qimageviewer.hxx"
#include "fimageviewer.hxx"

#include <vigraqimage.hxx>

#include <vigra/inspectimage.hxx>
#include <vigra/copyimage.hxx>
#include <vigra/stdimagefunctions.hxx>

#include <qimage.h>
#include <qlayout.h>

#include <math.h>

FImageViewer::FImageViewer(QWidget* parent= 0, const char* name= 0)
	: QWidget(parent, name),
	  qimageviewer_(new QImageViewer(this, "qimageviewer")),
	  image_(0),
	  qimage_(0),
	  autoScaleMode_(true),
	  logarithmicMode_(false),
	  markingMode_(false)
{
	QLayout *imageLayout= new QVBoxLayout( this );
	imageLayout->add(qimageviewer_);
}

FImageViewer::~FImageViewer()
{
	delete image_;
	delete qimage_;
}

QImage FImageViewer::displayedImage() const
{
	return *qimage_;
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

    delete qimage_;
	qimage_= new QImage;
    bool flag = qimage_->create(image_->width(), image_->height(), 8, 256);
    vigra_precondition(flag, "QImage creation failed");
	preparePalette();

	if (autoScaleMode_)
	{
		displayMin_= 1.0f; displayMax_= 0.0f; // make values "dirty"
		autoScale();
	}
	else
		redisplay(displayMin_, displayMax_);
}

void FImageViewer::preparePalette()
{
	for(int i=0; i<256; ++i)
		qimage_->setColor(i, qRgb(i,i,i));
	if (markingMode_)
		qimage_->setColor(255, qRgb(255,0,0));
}

void FImageViewer::setAutoScaleMode(bool newMode)
{
	if (autoScaleMode_ != newMode)
	{
		autoScaleMode_= newMode;
		if (autoScaleMode_)
			autoScale();
	}
}

void FImageViewer::autoScale()
{
	displayMinMax(imageMin(), imageMax());
}

void FImageViewer::setLogarithmicMode(bool newMode)
{
	if (logarithmicMode_ != newMode)
	{
		logarithmicMode_= newMode;
		redisplay(displayMin_, displayMax_);
	}
}

void FImageViewer::setMarkingMode(bool newMode)
{
	if (markingMode_ != newMode)
	{
		markingMode_= newMode;
		preparePalette();
		redisplay(displayMin_, displayMax_);
	}
}

void FImageViewer::displayMinMax(float min, float max)
{
	if ((displayMin_!=min) || (displayMax_!=max))
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
		: max_(max), scale_(254.0/log(max)) {};
	uchar operator()(float f) const
		{ return f<=max_? (f<=1? 0: (uchar)(scale_*log(f))) : 255; }
};

void FImageViewer::redisplay(float min, float max)
{
	int w= image_->width(), h= image_->height();

	vigra::QByteImage qi(qimage_);
	if (!logarithmicMode_)
		if (!markingMode_)
			vigra::transformImage(srcImageRange(*image_), destImage(qi),
								  FloatToByteFunctor(min, max));
		else
			vigra::transformImage(srcImageRange(*image_), destImage(qi),
								  FloatToByteMarkFunctor(min, max));
	else
		if (!markingMode_)
			vigra::transformImage(srcImageRange(*image_), destImage(qi),
								  FloatToByteLogFunctor(max));
		else
			vigra::transformImage(srcImageRange(*image_), destImage(qi),
								  FloatToByteLogMarkFunctor(max));

    qimageviewer_->setImage(*qimage_);

	//cerr << "redisplayed, emitting... " << min << "," << max << "\n";
	emit displayedMinMaxChanged(min, max);
}
