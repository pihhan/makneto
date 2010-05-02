/****************************************************************************
** Meta object code from reading C++ file 'mainwin.h'
**
** Created: Wed Apr 14 17:53:50 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Mainwin[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      14,    9,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,    8,    8,    8, 0x0a,
      52,    8,    8,    8, 0x0a,
      66,    8,    8,    8, 0x0a,
      80,    8,    8,    8, 0x0a,
      93,    8,    8,    8, 0x0a,
     113,    8,    8,    8, 0x0a,
     132,    8,    8,    8, 0x0a,
     145,    9,    8,    8, 0x0a,
     172,  166,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Mainwin[] = {
    "Mainwin\0\0path\0fileSelected(QString)\0"
    "displayHandle()\0startplayer()\0"
    "pauseplayer()\0selectFile()\0"
    "updateResolutions()\0probeResolutions()\0"
    "displayBin()\0setLocation(QString)\0"
    "index\0resolutionBoxChanged(int)\0"
};

const QMetaObject Mainwin::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Mainwin,
      qt_meta_data_Mainwin, 0 }
};

const QMetaObject *Mainwin::metaObject() const
{
    return &staticMetaObject;
}

void *Mainwin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Mainwin))
        return static_cast<void*>(const_cast< Mainwin*>(this));
    return QWidget::qt_metacast(_clname);
}

int Mainwin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileSelected((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: displayHandle(); break;
        case 2: startplayer(); break;
        case 3: pauseplayer(); break;
        case 4: selectFile(); break;
        case 5: updateResolutions(); break;
        case 6: probeResolutions(); break;
        case 7: displayBin(); break;
        case 8: setLocation((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: resolutionBoxChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Mainwin::fileSelected(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
