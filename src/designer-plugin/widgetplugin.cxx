#define QT_PLUGIN
#include "qimageviewer.hxx"
#include "fimageviewer.hxx"
#include "cmeditor.hxx"

#include <qwidgetplugin.h>
#include <iostream>

class VigraQtPlugin : public QWidgetPlugin
{
  public:
    virtual QStringList keys() const
    {
        return QStringList()
            << "QImageViewer"
            << "FImageViewer"
            << "ColorMapEditor"
        ;
    }

    virtual QString group(const QString &) const
    {
        return "VigraQt";
    }

    virtual QWidget *create(const QString &key,
                            QWidget *parent = 0, const char *name = 0)
    {
        if(key == "QImageViewer")
            return new QImageViewer(parent, name);
        if(key == "FImageViewer")
            return new FImageViewer(parent, name);
        if(key == "ColorMapEditor")
        {
            ColorMapEditor *e = new ColorMapEditor(parent, name);
            e->setColorMap(createCM());
            return e;
        }
        return NULL;
    }

    virtual QString includeFile(const QString &key) const
    {
        if(key == "QImageViewer")
            return "qimageviewer.hxx";
        if(key == "FImageViewer")
            return "fimageviewer.hxx";
        if(key == "ColorMapEditor")
            return "cmeditor.hxx";
        return QString::null;
    }

    virtual QString toolTip(const QString &key) const
    {
        if(key == "QImageViewer")
            return "QImage display widget (zooming, panning..)";
        if(key == "FImageViewer")
            return "vigra::FImage display widget (auto-scaling, logarithmic display..)";
//         if(key == "ColorMapEditor")
//             return "ColorMap editor widget)";
        return QString::null;
    }
        //virtual QString whatsThis(const QString &key) const;
};

Q_EXPORT_PLUGIN(VigraQtPlugin)
