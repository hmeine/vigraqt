#include "imageAnalyzer.hxx"
#include "colormap.hxx"

#include <vigraqimage.hxx>
#include <qimageviewer.hxx>

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
};

ImageAnalyzer::ImageAnalyzer(QWidget *parent, const char *name)
: ImageAnalyzerBase(parent, name),
  p(new ImageAnalyzerPrivate)
{
    p->cm = NULL;
    p->cm = createCM();
}

void ImageAnalyzer::load(const char *filename)
{
    vigra::ImageImportInfo info(filename);
    p->originalImage.resize(info.size());
    importImage(info, destImage(p->originalImage));
    
    p->minmax.reset();
    vigra::inspectImage(srcImageRange(p->originalImage), p->minmax);

    p->cm->setDomain(p->minmax.min, p->minmax.max);

    updateDisplay();
}

void ImageAnalyzer::updateDisplay()
{
    vigra::QRGBImage displayImage(p->originalImage.size());

    copyImage(srcImageRange(p->originalImage),
              destImage(displayImage, *p->cm));

    /*OriginalImage::traverser    srcRow(p->originalImage.upperLeft());
    vigra::QRGBImage::traverser dispRow(displayImage.upperLeft());
    for(; srcRow.y != p->originalImage.lowerRight().y; ++srcRow.y, ++dispRow.y)
    {
        OriginalImage::traverser    src(srcRow);
        vigra::QRGBImage::traverser disp(dispRow);
        for(; src.x != p->originalImage.lowerRight().x; ++src.x, ++disp.x)
        {
            PixelType v(*src);
            disp->setRed(p->cm->red(v));
            disp->setGreen(p->cm->green(v));
            disp->setBlue(p->cm->blue(v));
        }
        }*/

    imageViewer->setImage(displayImage.qImage());
}

