#include "colorize.hxx"

#include <VigraQt/colormap.hxx>
#include <VigraQt/cmeditor.hxx>
#include <VigraQt/vigraqimage.hxx>
#include <VigraQt/qimageviewer.hxx>
#include <VigraQt/imagecaption.hxx>

#include <QLayout>
#include <QSlider>
#include <QSpinBox>
#include <QStatusBar>
#include <QTimer>

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
    LinearColorMap              *cm;
    ImageCaption                *imageCaption;
    double                       gamma;
    QTimer                      *displayTimer;
};

Colorize::Colorize(QWidget *parent)
: QMainWindow(parent),
  p(new ColorizePrivate)
{
	setupUi(this);
    p->cm = static_cast<LinearColorMap *>(createCM());
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

void Colorize::updateDisplay()
{
    p->displayTimer->start(100);
}

void Colorize::computeDisplay()
{
    vigra::QRGBImage displayImage(p->originalImage.size());

//     copyImage(srcImageRange(p->originalImage),
//               destImage(displayImage, *p->cm));
    transformImage(srcImageRange(p->originalImage),
                   destImage(displayImage, *p->cm),
                   vigra::GammaFunctor<PixelType>(
                       p->gamma, p->minmax.min, p->minmax.max));

    imageViewer->setImage(displayImage.qImage());
}

void Colorize::gammaSliderChanged(int pos)
{
    p->gamma = std::pow(1.1, pos);
    updateDisplay();
}
