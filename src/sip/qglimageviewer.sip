class QGLImageViewer : QImageViewerBase
{
%TypeHeaderCode
#include <VigraQt/qglimageviewer.hxx>
%End
public:
    QGLImageViewer(QWidget */TransferThis/ = 0);

    virtual void setImage(const QImage &, bool = false);
    virtual void updateROI(const QImage &, const QPoint &);

    virtual void slideBy(const QPoint &);

protected:
    void ensureGLWidget();
    virtual QGLImageWidget *createGLWidget();
};

class QGLImageWidget : QGLWidget
{
%TypeHeaderCode
#include <VigraQt/qglimageviewer.hxx>
%End
public:
    QGLImageWidget(QWidget */TransferThis/ = 0);

    void setImage(const QImage &);
    void roiChanged(QPoint, QSize);

    void initializeGL();
    void paintGL();
    void resizeGL(int, int);

    void initGLTransform();
    void paintImage();

protected:
    void initTexture();
};
