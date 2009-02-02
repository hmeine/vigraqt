/************************************************************************/
/*                                                                      */
/*                  Copyright 2005-2008 by Hans Meine                   */
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

#ifndef CMGRADIENT_HXX
#define CMGRADIENT_HXX

#include "colormap.hxx"
#include "vigraqt_export.hxx"

#include <QFrame>

#include <vector>

class QPaintEvent;
class QResizeEvent;

class VIGRAQT_EXPORT ColorMapGradient : public QFrame
{
	Q_OBJECT

public:
	ColorMapGradient(QWidget *parent);
	void setColorMap(ColorMap *cm);

	virtual QSize sizeHint() const;

public slots:
	virtual void rereadColorMap();

protected:
	virtual bool event(QEvent *event);

	double x2Value(int x) const;
	int value2X(double value) const;

	void updateDomain();
	virtual bool tip(const QPoint &p, QRect *r, QString *s);

	// layout constants:
	enum { xMargin = 10, yMargin = 2 };

	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

	ColorMap *cm_;

	// dynamic layout values:
	double valueOffset_, valueScale_;
};

#endif // CMGRADIENT_HXX
