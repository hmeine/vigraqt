#ifndef IMAGEANALYZER_HXX
#define IMAGEANALYZER_HXX

#include "ui_imageAnalyzerBase.h"
#include <Q3MainWindow>

struct ImageAnalyzerPrivate;

class ImageAnalyzer : public Q3MainWindow, Ui::ImageAnalyzer
{
    Q_OBJECT

    ImageAnalyzerPrivate *p;

public:
    ImageAnalyzer(QWidget *parent = NULL);
    void load(const char *filename);

public slots:
    void updateDisplay();
    void computeDisplay();
    void gammaSliderChanged(int pos);
};

#endif // IMAGEANALYZER_HXX
