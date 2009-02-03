#ifndef COLORIZE_HXX
#define COLORIZE_HXX

#include "ui_colorizeBase.h"
#include <QMainWindow>

struct ColorizePrivate;

class QDragEnterEvent;
class QDropEvent;

class Colorize : public QMainWindow, Ui::Colorize
{
    Q_OBJECT

    ColorizePrivate *p;

public:
    Colorize(QWidget *parent = NULL);
    void load(const char *filename);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

public slots:
    void updateDisplay();
    void computeDisplay();
    void gammaSliderChanged(int pos);
};

#endif // COLORIZE_HXX
