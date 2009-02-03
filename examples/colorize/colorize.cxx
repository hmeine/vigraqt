#include "colorize.hxx"

#include <VigraQt/colormap.hxx>
#include <VigraQt/cmeditor.hxx>
#include <VigraQt/vigraqimage.hxx>
#include <VigraQt/qimageviewer.hxx>
#include <VigraQt/imagecaption.hxx>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLayout>
#include <QSlider>
#include <QSpinBox>
#include <QStatusBar>
#include <QTimer>
#include <QUrl>

#include <vigra/impex.hxx>
#include <vigra/inspectimage.hxx>
#include <vigra/transformimage.hxx>
#include <vigra/stdimage.hxx>

#include <cmath>

typedef float PixelType;
typedef vigra::BasicImage<PixelType> OriginalImage;

struct ColorizePrivate
{
    OriginalImage                originalImage;
    vigra::FindMinMax<PixelType> minmax;
    ColorMap                    *cm;
    ImageCaption                *imageCaption;
    double                       gamma;
    QTimer                      *displayTimer;
};

Colorize::Colorize(QWidget *parent)
: QMainWindow(parent),
  p(new ColorizePrivate)
{
	setupUi(this);
    p->cm = createColorMap(CMFire); // CMGray
    cme->setColorMap(p->cm);
    connect(cme, SIGNAL(colorMapChanged()), SLOT(updateDisplay()));
    p->imageCaption = NULL;
    p->gamma = 1.0;
    p->displayTimer = new QTimer(this);
    p->displayTimer->setSingleShot(true);
    connect(p->displayTimer, SIGNAL(timeout()), SLOT(computeDisplay()));

    connect(gammaSlider, SIGNAL(valueChanged(int)),
            SLOT(gammaSliderChanged(int)));
    gammaSpinBox->hide();

	setAcceptDrops(true);
}

void Colorize::load(const char *filename)
{
    delete p->imageCaption;

    vigra::ImageImportInfo info(filename);
    p->originalImage.resize(info.size());

    if(info.isGrayscale())
        importImage(info, destImage(p->originalImage));
    else
    {
        typedef vigra::BasicImage<vigra::RGBValue<PixelType> > TempImage;
        TempImage tempColor(info.size());
        importImage(info, destImage(tempColor));
        copyImage(srcImageRange(
                      tempColor, vigra::RGBToGrayAccessor<TempImage::PixelType>()),
                  destImage(p->originalImage));
    }

    p->minmax.reset();
    vigra::inspectImage(srcImageRange(p->originalImage), p->minmax);

    p->cm->setDomain(p->minmax.min, p->minmax.max);

    updateDisplay();

    p->imageCaption = createImageCaption(p->originalImage, this);
    connect(imageViewer, SIGNAL(mouseOver(int,int)),
            p->imageCaption, SLOT(update(int,int)));
    connect(p->imageCaption, SIGNAL(captionChanged(const QString&)),
            statusBar(), SLOT(message(const QString&)));
}

void Colorize::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

	if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();

		if(urlList.size() != 1)
			return;
        
        QUrl url(urlList.at(0));
        if(!url.isValid() || url.toLocalFile().isEmpty())
            return;

        event->acceptProposedAction();
	}
}

void Colorize::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

	if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();

		if(urlList.size() != 1)
			return;
        
        QUrl url(urlList.at(0));
        if(!url.isValid() || url.toLocalFile().isEmpty())
            return;

        load(url.toLocalFile().toLocal8Bit());
	}
}

void Colorize::updateDisplay()
{
    p->displayTimer->start(100);
}

class GammaAndColorMap
 : public std::unary_function<PixelType, ColorMap::result_type>
{
  private:
    vigra::GammaFunctor<PixelType> gamma_;
    const ColorMap &cm_;

  public:
    GammaAndColorMap(double gamma, PixelType min, PixelType max,
                     const ColorMap *cm)
    : gamma_(gamma, min, max),
      cm_(*cm)
    {
    }

    ColorMap::result_type
    operator()(const PixelType& v) const
    {
        return cm_(gamma_(v));
    }
};

void Colorize::computeDisplay()
{
    if(!p->imageCaption) // HACK: no image loaded yet?
        return;

    vigra::QRGBImage displayImage(p->originalImage.size());

//     copyImage(srcImageRange(p->originalImage),
//               destImage(displayImage, *p->cm));
    transformImage(srcImageRange(p->originalImage),
                   destImage(displayImage),
                   GammaAndColorMap(
                       p->gamma, p->minmax.min, p->minmax.max, p->cm));

    imageViewer->setImage(displayImage.qImage());
}

void Colorize::gammaSliderChanged(int pos)
{
    p->gamma = std::pow(1.1, pos);
    updateDisplay();
}
