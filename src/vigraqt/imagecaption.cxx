#include "imagecaption.hxx"

#ifndef NO_MOC_INCLUSION
#include "imagecaption.moc"
#endif

ImageCaption::ImageCaption(const QPoint &offset,
                           QObject *parent, const char *name)
: QObject(parent, name),
  offset_(offset)
{}
