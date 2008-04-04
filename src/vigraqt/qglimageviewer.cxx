#include "qglimageviewer.hxx"

#include <QImage>
#include <QLayout>

#include <cmath>
#include <iostream>

QGLImageWidget::QGLImageWidget(QWidget *parent)
: QGLWidget(parent),
  useTexture_(true),
  compression_(false),
  textureID_(0)
{
}

void QGLImageWidget::setImage(QImage const &image)
{
    image_ = image;

	if(image_.depth() == 32)
	{
		pixelFormat_ = GL_BGRA;
		pixelType_ = GL_UNSIGNED_INT_8_8_8_8_REV;
	}
	else if(image_.depth() == 8)
	{
		pixelFormat_ = GL_LUMINANCE;
		pixelType_ = GL_UNSIGNED_BYTE;
	}
	else
	{
		std::cerr << "setImage(): Unhandled depth (" << image_.depth() << ").\n";
		return;
	}

    if(textureID_) // initializeGL finished?
    {
        if(useTexture_)
            initTexture();
        updateGL();
    }
}

void QGLImageWidget::roiChanged(QPoint upperLeft, QSize size)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    upperLeft.x(), upperLeft.y(),
                    size.width(), size.height(),
                    pixelFormat_, pixelType_, image_.bits());
    updateGL();
}

void QGLImageWidget::initializeGL()
{
    qglClearColor(palette().brush(backgroundRole()).color());
    glGenTextures(1, &textureID_);

    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // only needed if image hasAlphaBuffer():
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!image_.isNull() && useTexture_)
        initTexture();
}

void QGLImageWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(!image_.isNull())
    {
        initGLTransform();
        paintImage();
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

void QGLImageWidget::initGLTransform()
{
    QImageViewerBase *viewer = (QImageViewerBase *)parent();
    QPoint ul(viewer->upperLeft());
    glLoadIdentity();
    glTranslatef(ul.x(), height()-ul.y(), 0);
    glScalef( std::pow(2.0, viewer->zoomLevel()),
             -std::pow(2.0, viewer->zoomLevel()), 1.f);
}

void QGLImageWidget::paintImage()
{
    if(image_.isNull())
        return;

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, textureID_);

    if(useTexture_)
    {
        double tw = (double)image_.width() / textureWidth_;
        double th = (double)image_.height() / textureHeight_;
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2i(0, 0);
        glTexCoord2f(0.0,  th); glVertex2i(0, image_.height());
        glTexCoord2f( tw,  th); glVertex2i(image_.width(), image_.height());
        glTexCoord2f( tw, 0.0); glVertex2i(image_.width(), 0);
        glEnd();
    }
    else
    {
        // TODO: vertically mirrored, only work on visible region
        glRasterPos2i(0, 0);
        glBitmap(0, 0, 0, 0, 0, -height(), NULL);
        glDrawPixels(image_.width(), image_.height(),
                     pixelFormat_, pixelType_, image_.bits());
    }

    glDisable(GL_TEXTURE_2D);
}

void QGLImageWidget::initTexture()
{
    // setup texture
    glBindTexture(GL_TEXTURE_2D, textureID_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    textureWidth_ = 2;
    while(textureWidth_ < (unsigned)image_.width())
        textureWidth_ *= 2;
    textureHeight_ = 2;
    while(textureHeight_ < (unsigned)height())
        textureHeight_ *= 2;

	GLint targetFormat = compression_ ? GL_COMPRESSED_RGB_ARB : GL_RGB;
    if(image_.hasAlphaChannel())
        targetFormat = compression_ ? GL_COMPRESSED_RGBA_ARB : GL_RGBA;

    if((textureWidth_ != (unsigned)image_.width()) ||
       (textureHeight_ != (unsigned)image_.height()))
    {
        QImage uploadImage(textureWidth_, textureHeight_, image_.format());

        // improve compression by clearing unused pixel data:
        if(compression_)
            memset(uploadImage.bits(), 0, uploadImage.numBytes());

        // copy pixel data
        for(unsigned int y = 0; y < (unsigned)image_.height(); ++y)
        {
            memcpy(uploadImage.scanLine(y), image_.scanLine(y),
                   image_.bytesPerLine());
        }

        glTexImage2D(GL_TEXTURE_2D,
                     0, // level of detail
                     targetFormat,
                     textureWidth_, textureHeight_, 0,
                     pixelFormat_, pixelType_, uploadImage.bits());
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D,
                     0, // level of detail
                     targetFormat,
                     image_.width(), image_.height(), 0,
                     pixelFormat_, pixelType_, image_.bits());
    }
}

bool QGLImageWidget::checkGLError(const char *where)
{
	GLenum error = glGetError();
	if(error)
	{
		std::cerr << "ERROR";
		if(where)
			std::cerr << " (" << where << ")";
		std::cerr << ": OpenGL reports '" << gluErrorString(error) << "'.\n";
		while(error)
		{
			error = glGetError();
			if(error)
				std::cerr << "  and subsequently: '"
						  << gluErrorString(error) << "'.\n";
		}
		return true;
	}
	return false;
}

/********************************************************************/

QGLImageViewer::QGLImageViewer(QWidget *parent)
: QImageViewerBase(parent),
  glWidget_(NULL)
{
    new QHBoxLayout(this);
}

void QGLImageViewer::setImage(QImage const &image, bool retainView)
{
    QImageViewerBase::setImage(image, retainView);
    if(ensureGLWidget())
        glWidget_->setImage(originalImage_);
}

void QGLImageViewer::updateROI(QImage const &roiImage, QPoint const &upperLeft)
{
    QImageViewerBase::updateROI(roiImage, upperLeft);
    if(ensureGLWidget())
        glWidget_->roiChanged(upperLeft, roiImage.size());
}

void QGLImageViewer::slideBy(QPoint const &diff)
{
    QImageViewerBase::slideBy(diff);
    if(ensureGLWidget())
        glWidget_->updateGL();
}

bool QGLImageViewer::ensureGLWidget()
{
    if(glWidget_)
        return true;

    glWidget_ = createGLWidget();
    layout()->addWidget(glWidget_);
    if(glWidget_)
        connect(this, SIGNAL(zoomLevelChanged(int)),
                glWidget_, SLOT(updateGL()));
    else
        std::cerr << "ensureGLWidget(): OOPS!\n";

    return glWidget_ != NULL;
}

QGLImageWidget *QGLImageViewer::createGLWidget()
{
    return new QGLImageWidget(this);
}
