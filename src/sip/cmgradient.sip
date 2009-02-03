class ColorMapGradient : QFrame
{
%TypeHeaderCode
#include <cmgradient.hxx>
%End

public:
	ColorMapGradient(QWidget *parent /TransferThis/ = 0);
	void setColorMap(ColorMap *cm /Transfer/);

	virtual QSize sizeHint() const;

public slots:
	virtual void rereadColorMap();

protected:
	virtual bool event(QEvent *event);

	double x2Value(double x) const;
	int value2X(double value) const;
	double value2XF(double value) const;

	void updateDomain();
	virtual bool tip(const QPoint &p, QRect *r /Out/, QString *s /Out/);

	virtual void paintEvent(QPaintEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
};
