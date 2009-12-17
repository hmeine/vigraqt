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

#ifndef CMEDITOR_HXX
#define CMEDITOR_HXX

#include "cmgradient.hxx"
#include "linear_colormap.hxx"

#include <vector>

class QContextMenuEvent;
class QDragMoveEvent;
class QDropEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;

class VIGRAQT_EXPORT ColorMapEditor : public ColorMapGradient
{
	Q_OBJECT

public:
	ColorMapEditor(QWidget *parent);

	// opens the color changing dialog:
	void editColor(unsigned int i);
	void remove(unsigned int i);
	unsigned int insert(double domainPosition, bool select = true);

	virtual void rereadColorMap();

Q_SIGNALS:
	void colorMapChanged();

protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);
	virtual void dropEvent(QDropEvent *e);

	LinearColorMap::TransitionIterator findTriangle(const QPoint &pos) const;
	unsigned int editIndex(const LinearColorMap::TransitionIterator &it, int x) const;
	QRect triangleBounds(unsigned int i) const;
	bool tip(const QPoint &p, QRect *r, QString *s);

	virtual void paintEvent(QPaintEvent *e);

	// layout constants:
	enum { triangleWidth = 12, triangleHeight = 12 };

	LinearColorMap *lcm_, cmBackup_;

	bool dragging_, changed_;
	int dragStartX_, dragPrevX_, selectIndex_;

	std::vector<bool> selected_;
};

#endif // CMEDITOR_HXX
