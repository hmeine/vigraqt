void *unwrapQObject(QObject *);
QWidget *wrapQWidget(void *);

%ModuleHeaderCode
class QObject;
inline void *unwrapQObject(QObject *p) { return p; }

class QWidget;
inline QWidget *wrapQWidget(void *p) { return static_cast<QWidget *>(p); }
%End
