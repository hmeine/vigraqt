#include "qglimageviewer.hxx"
#include <qlayout.h>
#include <iostream>

QGLImageWidget::QGLImageWidget(QWidget *parent, const char *name)
: QGLWidget(parent, name),
  textureID_(0),
  objectID_(0),
  image_(NULL)
{
}

void QGLImageWidget::setImage(QImage const &image)
{
    image_ = &image;
    if(objectID_)
        initObjects();

    updateGL();
}

void QGLImageWidget::roiChanged(QPoint upperLeft, QSize size)
{
	GLint pixelFormat, pixelType;
	if(image_->depth() == 32)
	{
		pixelFormat = GL_BGRA;
		pixelType = GL_UNSIGNED_INT_8_8_8_8_REV;
	}
	else if(image_->depth() == 8)
	{
		pixelFormat = GL_LUMINANCE;
		pixelType = GL_UNSIGNED_BYTE;
	}
	else
	{
		std::cerr << "setImage(): Unhandled depth.\n";
		return;
	}

    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    upperLeft.x(), upperLeft.y(),
                    size.width(), size.height(),
                    pixelFormat, pixelType, image_->bits());
    updateGL();
}

void QGLImageWidget::initializeGL()
{
    qglClearColor(backgroundColor());
    glGenTextures(1, &textureID_);
    objectID_ = glGenLists(1);
    std::cerr << "using texture " << textureID_
              << " and object " << objectID_ << "\n";

    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // only needed if image hasAlphaBuffer():
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(image_)
        initObjects();
}

void QGLImageWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(image_) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, textureID_);

        QImageViewerBase *viewer = (QImageViewerBase *)parent();
        QPoint ul(viewer->upperLeft());
        glLoadIdentity();
        glTranslatef(ul.x(), height()-ul.y(), 0);
        glScalef(pow(2.0, viewer->zoomLevel()),
                 pow(2.0, viewer->zoomLevel()), 1.f);
        glTranslatef(0, -image_->height(), 0);
        glCallList(objectID_);

        glDisable(GL_TEXTURE_2D);
    }

    glFlush();
}

void QGLImageWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLint)w, (GLint)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void QGLImageWidget::initObjects()
{
    // setup texture
    glBindTexture(GL_TEXTURE_2D, textureID_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLint pixelFormat, pixelType, targetFormat = GL_RGB;
	if(image_->depth() == 32)
	{
        if(image_->hasAlphaBuffer())
            targetFormat = GL_RGBA;
		pixelFormat = GL_BGRA;
		pixelType = GL_UNSIGNED_INT_8_8_8_8_REV;
	}
	else if(image_->depth() == 8)
	{
		pixelFormat = GL_LUMINANCE;
		pixelType = GL_UNSIGNED_BYTE;
	}
	else
	{
		std::cerr << "setImage(): Unhandled depth (" << image_->depth() << ").\n";
		return;
	}

    glTexImage2D(GL_TEXTURE_2D,
                 0, // level of detail
                 targetFormat,
                 image_->width(), image_->height(), 0,
                 pixelFormat, pixelType, image_->bits());

    // setup rectangle object
    glNewList(objectID_, GL_COMPILE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2i(0, image_->height());
    glTexCoord2f(0.0, 1.0); glVertex2i(0, 0);
    glTexCoord2f(1.0, 1.0); glVertex2i(image_->width(), 0);
    glTexCoord2f(1.0, 0.0); glVertex2i(image_->width(), image_->height());
    glEnd();

    glEndList();

    std::cerr << "  texture " << textureID_
              << " and object " << objectID_ << " initialized\n";
}

/********************************************************************/

QGLImageViewer::QGLImageViewer(QWidget *parent, const char *name)
: QImageViewerBase(parent, name)
{
    QBoxLayout *l = new QHBoxLayout(this);
    l->setAutoAdd(true);
    glWidget_ = new QGLImageWidget(this, "glWidget");
    connect(this, SIGNAL(zoomLevelChanged(int)),
            glWidget_, SLOT(updateGL()));
}

void QGLImageViewer::setImage(QImage const &image, bool retainView)
{
    QImageViewerBase::setImage(image, retainView);
    glWidget_->setImage(image);
}

void QGLImageViewer::updateROI(QImage const &roiImage, QPoint const &upperLeft)
{
    QImageViewerBase::updateROI(roiImage, upperLeft);
    glWidget_->roiChanged(upperLeft, roiImage.size());
}

void QGLImageViewer::slideBy(QPoint const &diff)
{
    QImageViewerBase::slideBy(diff);
    glWidget_->updateGL();
}
