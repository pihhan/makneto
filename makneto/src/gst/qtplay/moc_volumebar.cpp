/****************************************************************************
** Meta object code from reading C++ file 'volumebar.h'
**
** Created: Mon Mar 22 10:15:24 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "volumebar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'volumebar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VolumeBar[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_VolumeBar[] = {
    "VolumeBar\0\0volumeChanged()\0"
};

const QMetaObject VolumeBar::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_VolumeBar,
      qt_meta_data_VolumeBar, 0 }
};

const QMetaObject *VolumeBar::metaObject() const
{
    return &staticMetaObject;
}

void *VolumeBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VolumeBar))
        return static_cast<void*>(const_cast< VolumeBar*>(this));
    if (!strcmp(_clname, "GstAudioWatcher"))
        return static_cast< GstAudioWatcher*>(const_cast< VolumeBar*>(this));
    return QFrame::qt_metacast(_clname);
}

int VolumeBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: volumeChanged(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void VolumeBar::volumeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
