/****************************************************************************
** Meta object code from reading C++ file 'qspectrumwidget.h'
**
** Created: Fri Oct 2 20:02:07 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qspectrumwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qspectrumwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QSpectrumWidget[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QSpectrumWidget[] = {
    "QSpectrumWidget\0\0printSpectrum()\0"
};

const QMetaObject QSpectrumWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QSpectrumWidget,
      qt_meta_data_QSpectrumWidget, 0 }
};

const QMetaObject *QSpectrumWidget::metaObject() const
{
    return &staticMetaObject;
}

void *QSpectrumWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSpectrumWidget))
        return static_cast<void*>(const_cast< QSpectrumWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QSpectrumWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: printSpectrum(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
