#ifndef QGLIMAGEVIEWER_HXX
#define QGLIMAGEVIEWER_HXX

#include "qimageviewer.hxx"
#include <qgl.h>

class QGLImageWidget;

class QGLImageViewer : public QImageViewerBase
{
public:
    QGLImageViewer(QWidget *parent = 0, const char *name = 0);

    virtual void setImage(QImage const &image, bool retainView= false);
    virtual void updateROI(QImage const &roiImage, QPoint const &upperLeft);

    virtual void slideBy(QPoint const &diff);

protected:
    bool ensureGLWidget();
    virtual QGLImageWidget *createGLWidget();

    QGLImageWidget *glWidget_;
};

/********************************************************************/

// internal helper widget
class QGLImageWidget : public QGLWidget
{
public:
    QGLImageWidget(QWidget *parent = 0, const char *name = 0);

    void setImage(QImage const &image);
    void roiChanged(QPoint upperLeft, QSize size);

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void initGLTransform();
    void paintImage();

protected:
    void initTexture();
    void checkGLError(const char *where);

        // should be user-configurable properties in the future:
    bool useTexture_, compression_;

    unsigned int textureWidth_, textureHeight_;
    GLint pixelFormat_, pixelType_;
    GLuint textureID_;
    const QImage *image_;
};

#endif // QGLIMAGEVIEWER_HXX
