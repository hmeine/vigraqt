#include <VigraQt/qimageviewer.hxx>
#include <qapplication.h>
#include <iostream>

int main(int argc, char **argv)
{
    unsigned int count = 0;

    QApplication app(argc, argv);
    for(int i = 1; i < argc; ++i)
    {
        QImage img(argv[i]);
        if(img.isNull())
        {
            std::cerr << "could not load '" << argv[i] << "'!\n";
        }
        else
        {
            QImageViewerBase *v = new QImageViewer();
            v->setImage(img);
            v->show();
            ++count;
        }
    }

    if(count)
    {
        QObject::connect(&app, SIGNAL(lastWindowClosed()),
                         &app, SLOT(quit()));
        app.exec();
    }
    else
    {
        std::cerr << "USAGE: " << argv[0] << " <image filename> ...\n";
    }
}
