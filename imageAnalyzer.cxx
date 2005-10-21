#include "imageAnalyzer.hxx"
#include "colormap.hxx"
#include "cmEditor.h"

#include <vigraqimage.hxx>
#include <qimageviewer.hxx>
#include <imagecaption.hxx>

#include <qlayout.h>
#include <qstatusbar.h>

#include <vigra/impex.hxx>
#include <vigra/inspectimage.hxx>
#include <vigra/stdimage.hxx>

typedef float PixelType;
typedef vigra::BasicImage<PixelType> OriginalImage;

struct ImageAnalyzerPrivate
{
    OriginalImage                originalImage;
    vigra::FindMinMax<PixelType> minmax;
    ColorMap                    *cm;
	ColorMapEditor              *cme;
    ImageCaption                *imageCaption;
};

ImageAnalyzer::ImageAnalyzer(QWidget *parent, const char *name)
: ImageAnalyzerBase(parent, name),
  p(new ImageAnalyzerPrivate)
{
    p->cm = createCM();
	p->cme = new ColorMapEditor(centralWidget(), "colorMapEditor");
	p->cme->setColorMap(p->cm);
	ImageAnalyzerLayout->addWidget(p->cme);
    p->imageCaption = NULL;
}

void ImageAnalyzer::load(const char *filename)
{
    delete p->imageCaption;

    vigra::ImageImportInfo info(filename);
    p->originalImage.resize(info.size());
    importImage(info, destImage(p->originalImage));

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
    vigra::QRGBImage displayImage(p->originalImage.size());

    copyImage(srcImageRange(p->originalImage),
              destImage(displayImage, *p->cm));

    imageViewer->setImage(displayImage.qImage());
}
