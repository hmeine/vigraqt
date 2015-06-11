/************************************************************************/
/*                                                                      */
/*                  Copyright 2003-2005 by Hans Meine                   */
/*                 meine@kogs.informatik.uni-hamburg.de                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*  This file is part of the VigraQt library.                           */
/*                                                                      */
/*  VigraQt is free software; you can redistribute it and/or modify it  */
/*  under the terms of the GNU General Public License as published by   */
/*  the Free Software Foundation; either version 2 of the License, or   */
/*  (at your option) any later version.                                 */
/*                                                                      */
/*  VigraQt is distributed in the hope that it will be useful, but      */
/*  WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                */
/*  See the GNU General Public License for more details.                */
/*                                                                      */
/*  You should have received a copy of the GNU General Public License   */
/*  along with VigraQt; if not, write to the                            */
/*                   Free Software Foundation, Inc.,                    */
/*       51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA         */
/*                                                                      */
/************************************************************************/

#ifndef IMAGECAPTION_HXX
#define IMAGECAPTION_HXX

#include "vigraqt_export.hxx"
#include <vigra/rgbvalue.hxx>
#include <vigra/numerictraits.hxx>

#include <QObject>
#include <QPoint>
#include <QString>

#include <stdlib.h>
#include <math.h>

template <class T>
QString argPixelValue(QString s, T const & data)
{
    return s.arg(data);
}

template <class T>
QString argPixelValue(QString s, vigra::RGBValue<T> const & data)
{
    return s.arg(argPixelValue(
                     argPixelValue(
                         argPixelValue(
                             QString("(%1 %2 %3)"),
                             data.red()),
                         data.green()),
                     data.blue()));
}

/********************************************************************/

class VIGRAQT_EXPORT ImageCaption
: public QObject
{
    Q_OBJECT

protected:
    QPoint offset_;

public:
    ImageCaption(const QPoint &offset, QObject *parent = NULL);

public Q_SLOTS:
    virtual void update(int x, int y) = 0;

Q_SIGNALS:
    virtual void captionChanged(const QString &);
};

template <class Image>
class ImageCaptionImpl
: public ImageCaption
{
public:
    ImageCaptionImpl(Image const &img, const QPoint &offset = QPoint(0, 0),
                     QObject *parent = NULL)
    : ImageCaption(offset, parent),
      image(img),
      xFieldWidth(image.width() ?
                  (int)(log((double)image.width())/log(10.0) + 1.0) : 3),
      yFieldWidth(image.height() ?
                  (int)(log((double)image.height())/log(10.0) + 1.0) : 2)
    {
    }

    virtual void update(int x, int y)
    {
        x += offset_.x();
        y += offset_.y();
        if(x < 0 || x >= image.width() || y < 0 || y >= image.height())
            return;

        emit captionChanged(argPixelValue(QString("( %1 / %2 ) = %3")
                                          .arg(x, xFieldWidth)
                                          .arg(y, yFieldWidth), image(x,y)));
    }

private:
    const Image &image;
    int xFieldWidth, yFieldWidth;
};

template <class Image>
inline
ImageCaption *createImageCaption(Image const &img, const QPoint &offset,
                                 QObject *parent = NULL)
{
    return new ImageCaptionImpl<Image>(img, offset, parent);
}

template <class Image>
inline
ImageCaption *createImageCaption(Image const &img, QObject *parent = NULL)
{
    return new ImageCaptionImpl<Image>(img, QPoint(0, 0), parent);
}

#endif /* IMAGECAPTION_HXX */
