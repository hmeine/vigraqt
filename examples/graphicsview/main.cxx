#include <QtGui/QApplication>
#include "vigraqimageviewer.hxx"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VigraQImageViewer w;
    w.show();

    return a.exec();
}
