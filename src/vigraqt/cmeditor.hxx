/************************************************************************/
/*                                                                      */
/*                  Copyright 2005-2006 by Hans Meine                   */
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

#ifndef CMEDITOR_HXX
#define CMEDITOR_HXX

#include "colormap.hxx"

#include <QContextMenuEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QToolTip>
#include <QWidget>

#include <vector>

class ColorMapEditor : public QWidget
{
	Q_OBJECT

public:
	ColorMapEditor(QWidget *parent);
	void setColorMap(ColorMap *cm);

	virtual QSize sizeHint() const;

	// opens the color changing dialog:
	void editColor(unsigned int i);
	void remove(unsigned int i);
	unsigned int insert(double domainPosition, bool select = true);

public slots:
	void rereadColorMap();

signals:
	void colorMapChanged();

protected:
	virtual bool event(QEvent *event);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);
	virtual void dropEvent(QDropEvent *e);

	double x2Value(int x) const;
	int value2X(double value) const;

	void updateDomain();
	ColorMap::TransitionIterator findTriangle(const QPoint &pos) const;
	unsigned int editIndex(const ColorMap::TransitionIterator &it, int x) const;
	QRect triangleBounds(unsigned int i) const;
	bool tip(const QPoint &p, QRect *r, QString *s);

	// layout constants:
	enum { xMargin = 10, yMargin = 2, triangleWidth = 12, triangleHeight = 12 };

	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

	ColorMap *cm_, cmBackup_;

	// dynamic layout values:
	QRect gradientRect_;
	double valueOffset_, valueScale_;

	bool dragging_, changed_;
	int dragStartX_, dragPrevX_, selectIndex_;

	std::vector<bool> selected_;
};

#endif // CMEDITOR_HXX