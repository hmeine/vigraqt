#ifndef FIMAGEVIEWER_HXX
#define FIMAGEVIEWER_HXX

#include <qwidget.h>
#include <qimage.h>
#include <vigra/stdimage.hxx>

class QImage;
class QImageViewer;

class FImageViewer: public QWidget
{
	Q_OBJECT

public:
	FImageViewer( QWidget* parent=0, const char* name=0  );
	~FImageViewer();

	virtual int originalWidth() const;
	virtual int originalHeight() const;
	virtual int imageWidth() const;
	virtual int imageHeight() const;

	float imageMin() const { return imageMin_; }
	float imageMax() const { return imageMax_; }
	float displayMin() const { return displayMin_; }
	float displayMax() const { return displayMax_; }

	QImage displayedImage() const;

	bool autoScaleMode() { return autoScaleMode_; }
	bool logarithmicMode() { return logarithmicMode_; }

public slots:
	// display a copy of the given image
	virtual void setImage( const vigra::FImage &newImage );

	void setAutoScaleMode( bool newMode );
	void autoScale();

	void setLogarithmicMode( bool newMode );
	bool isLogarithmicMode() { return logarithmicMode_; }
	void setMarkingMode( bool newMode );
	bool isMarkingMode() { return markingMode_; }

	void displayMinMax(float min, float max);

signals:
	void imageMinMaxChanged(float min, float max);
	void displayedMinMaxChanged(float min, float max);

protected:
	void redisplay(float min, float max);

	void preparePalette();

protected:
	QImageViewer *qimageviewer_;
	vigra::FImage *image_;
	QImage *qimage_;

	bool autoScaleMode_;
	bool logarithmicMode_;
	bool markingMode_;

	float imageMin_, imageMax_;
	float displayMin_, displayMax_;
};

#endif // FIMAGEVIEWER_HXX
