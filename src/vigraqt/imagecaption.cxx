#include "imagecaption.hxx"

#include "imagecaption.moc"

ImageCaption::ImageCaption(const QPoint &offset,
                           QObject *parent, const char *name)
: QObject(parent, name),
  offset_(offset)
{}
