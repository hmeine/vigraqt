#ifndef COLORIZE_HXX
#define COLORIZE_HXX

#include "ui_colorizeBase.h"
#include <QMainWindow>

struct ColorizePrivate;

class Colorize : public QMainWindow, Ui::Colorize
{
    Q_OBJECT

    ColorizePrivate *p;

public:
    Colorize(QWidget *parent = NULL);
    void load(const char *filename);

public slots:
    void updateDisplay();
    void computeDisplay();
    void gammaSliderChanged(int pos);
};

#endif // COLORIZE_HXX
