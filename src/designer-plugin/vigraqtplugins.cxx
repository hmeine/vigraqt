#include "qimageviewer.hxx"
#include "fimageviewer.hxx"
#include "cmeditor.hxx"

#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerCustomWidgetInterface>

#include <iostream>
#include <QtPlugin>

class PluginBase : public QDesignerCustomWidgetInterface
{
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    bool isContainer() const { return false; }
    QIcon icon() const { return QIcon(); }
    QString whatsThis() const { return QString(); }
    QString toolTip() const { return QString(); }
    QString group() const { return "VigraQt"; }
};

class QImageViewerPlugin : public QObject, public PluginBase
{
    Q_OBJECT

public:
    QImageViewerPlugin(QObject *parent = 0) : QObject(parent) { }

    QString includeFile() const { return "qimageviewer.hxx"; }
    QString name() const { return "QImageViewer"; }

    QWidget *createWidget(QWidget *parent)
    {
        return new QImageViewer(parent);
    }
};

class FImageViewerPlugin : public QObject, public PluginBase
{
    Q_OBJECT

public:
    FImageViewerPlugin(QObject *parent = 0) : QObject(parent) { }

    QString includeFile() const { return "fimageviewer.hxx"; }
    QString name() const { return "FImageViewer"; }

    QWidget *createWidget(QWidget *parent)
    {
        return new FImageViewer(parent);
    }
};

class CMEditorPlugin : public QObject, public PluginBase
{
    Q_OBJECT

public:
    CMEditorPlugin(QObject *parent = 0) : QObject(parent) { }

    QString includeFile() const { return "cmeditor.hxx"; }
    QString name() const { return "ColorMapEditor"; }

    QWidget *createWidget(QWidget *parent)
    {
        ColorMapEditor *e = new ColorMapEditor(parent);
        e->setColorMap(createCM());
        return e;
    }
};

class VigraQtPlugins
: public QObject,
  public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    QList<QDesignerCustomWidgetInterface*> customWidgets() const
    {
        QList<QDesignerCustomWidgetInterface *> plugins;
        plugins
            << new QImageViewerPlugin
            << new FImageViewerPlugin
            << new CMEditorPlugin;
        return plugins;
    }
};

#include "vigraqtplugins.moc"

Q_EXPORT_PLUGIN(VigraQtPlugins)
