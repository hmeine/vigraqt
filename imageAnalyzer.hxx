#ifndef IMAGEANALYZER_HXX
#define IMAGEANALYZER_HXX

#include "imageAnalyzerBase.h"

struct ImageAnalyzerPrivate;

class ImageAnalyzer : public ImageAnalyzerBase
{
    Q_OBJECT

    ImageAnalyzerPrivate *p;

public:
    ImageAnalyzer(QWidget *parent = NULL, const char *name = NULL);
    void load(const char *filename);

public slots:
    void updateDisplay();
};

#endif // IMAGEANALYZER_HXX
