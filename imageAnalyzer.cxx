#include "imageAnalyzer.hxx"
#include "colormap.hxx"
#include "cmeditor.hxx"

#include <vigraqimage.hxx>
#include <qimageviewer.hxx>
#include <imagecaption.hxx>

#include <qlayout.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <qtimer.h>

#include <vigra/impex.hxx>
#include <vigra/inspectimage.hxx>
#include <vigra/transformimage.hxx>
#include <vigra/stdimage.hxx>

#include <cmath>

typedef float PixelType;
typedef vigra::BasicImage<PixelType> OriginalImage;

struct ImageAnalyzerPrivate
{
    OriginalImage                originalImage;
    vigra::FindMinMax<PixelType> minmax;
    ColorMap                    *cm;
    ColorMapEditor              *cme;
    ImageCaption                *imageCaption;
    double                       gamma;
    QTimer                      *displayTimer;
};

ImageAnalyzer::ImageAnalyzer(QWidget *parent, const char *name)
: ImageAnalyzerBase(parent, name),
  p(new ImageAnalyzerPrivate)
{
    p->cm = createCM();
    p->cme = new ColorMapEditor(centralWidget(), "colorMapEditor");
    p->cme->setColorMap(p->cm);
    connect(p->cme, SIGNAL(colorMapChanged()), SLOT(updateDisplay()));
    ImageAnalyzerLayout->addWidget(p->cme);
    p->imageCaption = NULL;
    p->gamma = 1.0;
    p->displayTimer = new QTimer(this);
    connect(p->displayTimer, SIGNAL(timeout()), SLOT(computeDisplay()));

    connect(gammaSlider, SIGNAL(valueChanged(int)),
            SLOT(gammaSliderChanged(int)));
    gammaSpinBox->hide();
}

void ImageAnalyzer::load(const char *filename)
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
    connect(imageViewer, SIGNAL(mouseMoved(int,int)),
            p->imageCaption, SLOT(update(int,int)));
    connect(p->imageCaption, SIGNAL(captionChanged(const QString&)),
            statusBar(), SLOT(message(const QString&)));
}

void ImageAnalyzer::updateDisplay()
{
    p->displayTimer->start(100, true);
}

void ImageAnalyzer::computeDisplay()
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

void ImageAnalyzer::gammaSliderChanged(int pos)
{
    p->gamma = std::pow(1.1, pos);
    updateDisplay();
}
