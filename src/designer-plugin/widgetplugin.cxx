#define QT_PLUGIN
#include "qimageviewer.hxx"
#include "fimageviewer.hxx"

#include <qwidgetplugin.h>
#include <iostream>

class ImageViewersPlugin : public QWidgetPlugin
{
  public:
    virtual QStringList keys() const
    {
        return QStringList()
            << "QImageViewer"
            << "FImageViewer";
    }

    virtual QString group (const QString &) const
    {
        return "Image Viewers";
    }

    virtual QWidget *create(const QString &key,
                            QWidget *parent = 0, const char *name = 0)
    {
        if(key == "QImageViewer")
            return new QImageViewer(parent, name);
        if(key == "FImageViewer")
            return new FImageViewer(parent, name);
        return NULL;
    }

    virtual QString includeFile(const QString &key) const
    {
        if(key == "QImageViewer")
            return "qimageviewer.hxx";
        if(key == "FImageViewer")
            return "fimageviewer.hxx";
        return QString::null;
    }

    virtual QString toolTip(const QString &key) const
    {
        if(key == "QImageViewer")
            return "QImage display widget (zooming, panning..)";
        if(key == "FImageViewer")
            return "vigra::FImage display widget (auto-scaling, logarithmic display..)";
        return QString::null;
    }
        //virtual QString whatsThis(const QString &key) const;
};

Q_EXPORT_PLUGIN(ImageViewersPlugin)
