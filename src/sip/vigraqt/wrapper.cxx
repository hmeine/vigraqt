class QWidget;

QWidget *wrapWidget(void *p)
{
    return static_cast<QWidget *>(p);
}

class QObject;

void *unwrapQObject(QObject *p)
{
    return p;
}
