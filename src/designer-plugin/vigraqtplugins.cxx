#include "qimageviewer.hxx"
#include "fimageviewer.hxx"
#include "cmeditor.hxx"

#include <QtDesigner/QDesignerContainerExtension>

#if QT_VERSION < 0x050000
	#include <QtDesigner/QDesignerCustomWidgetInterface>
#else
	#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#endif

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

    QString includeFile() const { return "VigraQt/qimageviewer.hxx"; }
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

    QString includeFile() const { return "VigraQt/fimageviewer.hxx"; }
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

    QString includeFile() const { return "VigraQt/cmeditor.hxx"; }
    QString name() const { return "ColorMapEditor"; }

    QWidget *createWidget(QWidget *parent)
    {
        ColorMapEditor *e = new ColorMapEditor(parent);
        e->setColorMap(createColorMap(CMFire));
        return e;
    }
};

class VigraQtPlugins
: public QObject,
  public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
#endif
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

#if QT_VERSION < 0x050000
	#include "vigraqtplugins.moc"
	Q_EXPORT_PLUGIN(VigraQtPlugins)
#endif