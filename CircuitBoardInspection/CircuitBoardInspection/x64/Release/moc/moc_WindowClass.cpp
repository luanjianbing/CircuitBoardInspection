/****************************************************************************
** Meta object code from reading C++ file 'WindowClass.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../WindowClass.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WindowClass.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WindowClass_t {
    QByteArrayData data[13];
    char stringdata0[214];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WindowClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WindowClass_t qt_meta_stringdata_WindowClass = {
    {
QT_MOC_LITERAL(0, 0, 11), // "WindowClass"
QT_MOC_LITERAL(1, 12, 13), // "onActionClose"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 15), // "onActionEnlarge"
QT_MOC_LITERAL(4, 43, 11), // "onActionRun"
QT_MOC_LITERAL(5, 55, 20), // "onActionImageSetting"
QT_MOC_LITERAL(6, 76, 19), // "dealSignalLoadModel"
QT_MOC_LITERAL(7, 96, 23), // "dealSignalGrabFrameOver"
QT_MOC_LITERAL(8, 120, 27), // "dealSignalGetExternalSignal"
QT_MOC_LITERAL(9, 148, 11), // "onButtonRun"
QT_MOC_LITERAL(10, 160, 18), // "onActionStatistics"
QT_MOC_LITERAL(11, 179, 13), // "portReadyRead"
QT_MOC_LITERAL(12, 193, 20) // "onPortSendSingleChar"

    },
    "WindowClass\0onActionClose\0\0onActionEnlarge\0"
    "onActionRun\0onActionImageSetting\0"
    "dealSignalLoadModel\0dealSignalGrabFrameOver\0"
    "dealSignalGetExternalSignal\0onButtonRun\0"
    "onActionStatistics\0portReadyRead\0"
    "onPortSendSingleChar"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WindowClass[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08 /* Private */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    0,   71,    2, 0x08 /* Private */,
       5,    0,   72,    2, 0x08 /* Private */,
       6,    1,   73,    2, 0x08 /* Private */,
       7,    0,   76,    2, 0x08 /* Private */,
       8,    0,   77,    2, 0x08 /* Private */,
       9,    0,   78,    2, 0x08 /* Private */,
      10,    0,   79,    2, 0x08 /* Private */,
      11,    0,   80,    2, 0x08 /* Private */,
      12,    0,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void WindowClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WindowClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onActionClose(); break;
        case 1: _t->onActionEnlarge(); break;
        case 2: _t->onActionRun(); break;
        case 3: _t->onActionImageSetting(); break;
        case 4: _t->dealSignalLoadModel((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->dealSignalGrabFrameOver(); break;
        case 6: _t->dealSignalGetExternalSignal(); break;
        case 7: _t->onButtonRun(); break;
        case 8: _t->onActionStatistics(); break;
        case 9: _t->portReadyRead(); break;
        case 10: _t->onPortSendSingleChar(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject WindowClass::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_WindowClass.data,
    qt_meta_data_WindowClass,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WindowClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WindowClass::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WindowClass.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int WindowClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
