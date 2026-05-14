/****************************************************************************
** Meta object code from reading C++ file 'gameview.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../gameview.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gameview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12DeathOverlayE_t {};
} // unnamed namespace

template <> constexpr inline auto DeathOverlay::qt_create_metaobjectdata<qt_meta_tag_ZN12DeathOverlayE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DeathOverlay"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DeathOverlay, qt_meta_tag_ZN12DeathOverlayE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DeathOverlay::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DeathOverlayE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DeathOverlayE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12DeathOverlayE_t>.metaTypes,
    nullptr
} };

void DeathOverlay::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DeathOverlay *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *DeathOverlay::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeathOverlay::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12DeathOverlayE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DeathOverlay::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN14VictoryOverlayE_t {};
} // unnamed namespace

template <> constexpr inline auto VictoryOverlay::qt_create_metaobjectdata<qt_meta_tag_ZN14VictoryOverlayE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "VictoryOverlay"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<VictoryOverlay, qt_meta_tag_ZN14VictoryOverlayE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject VictoryOverlay::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14VictoryOverlayE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14VictoryOverlayE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14VictoryOverlayE_t>.metaTypes,
    nullptr
} };

void VictoryOverlay::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<VictoryOverlay *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *VictoryOverlay::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VictoryOverlay::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14VictoryOverlayE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int VictoryOverlay::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN8GameViewE_t {};
} // unnamed namespace

template <> constexpr inline auto GameView::qt_create_metaobjectdata<qt_meta_tag_ZN8GameViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "GameView",
        "gameLoop",
        "",
        "onIntroStageChanged",
        "stage",
        "onIntroBossAppearing",
        "onIntroBossHpBarShow",
        "onBossDeathStageChanged",
        "onBossDeathCameraMove",
        "targetX",
        "targetY",
        "durationMs",
        "onBossDeathFadeOut",
        "onBossDeathFadeIn",
        "onBossDeathFadeToBlack",
        "onBossDeathCameraToPlayer",
        "onBossDeathComplete",
        "onBossPhase2DeathStageChanged",
        "onBossPhase2DeathCameraMove",
        "onBossPhase2DeathFadeOut",
        "onBossPhase2DeathCleanup",
        "onBossPhase2DeathComplete",
        "onPlayerDeathStageChanged",
        "onPlayerDeathPlayerFadeOut",
        "progress",
        "onPlayerDeathImageFadeIn",
        "onPlayerDeathScreenFadeToBlack",
        "onPlayerDeathComplete",
        "onVictoryImageFadeIn",
        "onVictoryScreenFadeToBlack",
        "onVictoryComplete"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'gameLoop'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onIntroStageChanged'
        QtMocHelpers::SlotData<void(int)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onIntroBossAppearing'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onIntroBossHpBarShow'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBossDeathStageChanged'
        QtMocHelpers::SlotData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onBossDeathCameraMove'
        QtMocHelpers::SlotData<void(qreal, qreal, int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 9 }, { QMetaType::QReal, 10 }, { QMetaType::Int, 11 },
        }}),
        // Slot 'onBossDeathFadeOut'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBossDeathFadeIn'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBossDeathFadeToBlack'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBossDeathCameraToPlayer'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBossDeathComplete'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBossPhase2DeathStageChanged'
        QtMocHelpers::SlotData<void(int)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onBossPhase2DeathCameraMove'
        QtMocHelpers::SlotData<void(qreal, qreal, int)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 9 }, { QMetaType::QReal, 10 }, { QMetaType::Int, 11 },
        }}),
        // Slot 'onBossPhase2DeathFadeOut'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBossPhase2DeathCleanup'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBossPhase2DeathComplete'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onPlayerDeathStageChanged'
        QtMocHelpers::SlotData<void(int)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onPlayerDeathPlayerFadeOut'
        QtMocHelpers::SlotData<void(qreal)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 24 },
        }}),
        // Slot 'onPlayerDeathImageFadeIn'
        QtMocHelpers::SlotData<void(qreal)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 24 },
        }}),
        // Slot 'onPlayerDeathScreenFadeToBlack'
        QtMocHelpers::SlotData<void(qreal)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 24 },
        }}),
        // Slot 'onPlayerDeathComplete'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onVictoryImageFadeIn'
        QtMocHelpers::SlotData<void(qreal)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 24 },
        }}),
        // Slot 'onVictoryScreenFadeToBlack'
        QtMocHelpers::SlotData<void(qreal)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 24 },
        }}),
        // Slot 'onVictoryComplete'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GameView, qt_meta_tag_ZN8GameViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject GameView::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8GameViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8GameViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8GameViewE_t>.metaTypes,
    nullptr
} };

void GameView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GameView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->gameLoop(); break;
        case 1: _t->onIntroStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onIntroBossAppearing(); break;
        case 3: _t->onIntroBossHpBarShow(); break;
        case 4: _t->onBossDeathStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onBossDeathCameraMove((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 6: _t->onBossDeathFadeOut(); break;
        case 7: _t->onBossDeathFadeIn(); break;
        case 8: _t->onBossDeathFadeToBlack(); break;
        case 9: _t->onBossDeathCameraToPlayer(); break;
        case 10: _t->onBossDeathComplete(); break;
        case 11: _t->onBossPhase2DeathStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->onBossPhase2DeathCameraMove((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 13: _t->onBossPhase2DeathFadeOut(); break;
        case 14: _t->onBossPhase2DeathCleanup(); break;
        case 15: _t->onBossPhase2DeathComplete(); break;
        case 16: _t->onPlayerDeathStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->onPlayerDeathPlayerFadeOut((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 18: _t->onPlayerDeathImageFadeIn((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 19: _t->onPlayerDeathScreenFadeToBlack((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 20: _t->onPlayerDeathComplete(); break;
        case 21: _t->onVictoryImageFadeIn((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 22: _t->onVictoryScreenFadeToBlack((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 23: _t->onVictoryComplete(); break;
        default: ;
        }
    }
}

const QMetaObject *GameView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GameView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8GameViewE_t>.strings))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int GameView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    return _id;
}
QT_WARNING_POP
