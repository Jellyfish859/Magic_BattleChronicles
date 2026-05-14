/****************************************************************************
** Meta object code from reading C++ file 'gamescene.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../gamescene.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gamescene.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9GameSceneE_t {};
} // unnamed namespace

template <> constexpr inline auto GameScene::qt_create_metaobjectdata<qt_meta_tag_ZN9GameSceneE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "GameScene",
        "playerHpChanged",
        "",
        "hp",
        "maxHp",
        "bossHpChanged",
        "phaseChanged",
        "phase",
        "introStageChanged",
        "stage",
        "introBossAppearing",
        "introBossHpBarShow",
        "bossDeathStageChanged",
        "bossDeathCameraMove",
        "targetX",
        "targetY",
        "durationMs",
        "bossDeathFadeOut",
        "bossDeathFadeIn",
        "bossDeathFadeToBlack",
        "bossDeathCameraToPlayer",
        "bossDeathComplete",
        "phase2DeathStageChanged",
        "phase2DeathCameraMove",
        "phase2DeathFadeOut",
        "phase2DeathCleanup",
        "phase2DeathComplete",
        "victoryStageChanged",
        "victoryImageFadeIn",
        "progress",
        "victoryScreenFadeToBlack",
        "victoryComplete",
        "playerDeathStageChanged",
        "playerDeathPlayerFadeOut",
        "playerDeathImageFadeIn",
        "playerDeathScreenFadeToBlack",
        "playerDeathComplete"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'playerHpChanged'
        QtMocHelpers::SignalData<void(int, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 4 },
        }}),
        // Signal 'bossHpChanged'
        QtMocHelpers::SignalData<void(int, int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 4 },
        }}),
        // Signal 'phaseChanged'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Signal 'introStageChanged'
        QtMocHelpers::SignalData<void(int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Signal 'introBossAppearing'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'introBossHpBarShow'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'bossDeathStageChanged'
        QtMocHelpers::SignalData<void(int)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Signal 'bossDeathCameraMove'
        QtMocHelpers::SignalData<void(qreal, qreal, int)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 14 }, { QMetaType::QReal, 15 }, { QMetaType::Int, 16 },
        }}),
        // Signal 'bossDeathFadeOut'
        QtMocHelpers::SignalData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'bossDeathFadeIn'
        QtMocHelpers::SignalData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'bossDeathFadeToBlack'
        QtMocHelpers::SignalData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'bossDeathCameraToPlayer'
        QtMocHelpers::SignalData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'bossDeathComplete'
        QtMocHelpers::SignalData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'phase2DeathStageChanged'
        QtMocHelpers::SignalData<void(int)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Signal 'phase2DeathCameraMove'
        QtMocHelpers::SignalData<void(qreal, qreal, int)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 14 }, { QMetaType::QReal, 15 }, { QMetaType::Int, 16 },
        }}),
        // Signal 'phase2DeathFadeOut'
        QtMocHelpers::SignalData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'phase2DeathCleanup'
        QtMocHelpers::SignalData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'phase2DeathComplete'
        QtMocHelpers::SignalData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'victoryStageChanged'
        QtMocHelpers::SignalData<void(int)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Signal 'victoryImageFadeIn'
        QtMocHelpers::SignalData<void(qreal)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 29 },
        }}),
        // Signal 'victoryScreenFadeToBlack'
        QtMocHelpers::SignalData<void(qreal)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 29 },
        }}),
        // Signal 'victoryComplete'
        QtMocHelpers::SignalData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'playerDeathStageChanged'
        QtMocHelpers::SignalData<void(int)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Signal 'playerDeathPlayerFadeOut'
        QtMocHelpers::SignalData<void(qreal)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 29 },
        }}),
        // Signal 'playerDeathImageFadeIn'
        QtMocHelpers::SignalData<void(qreal)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 29 },
        }}),
        // Signal 'playerDeathScreenFadeToBlack'
        QtMocHelpers::SignalData<void(qreal)>(35, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 29 },
        }}),
        // Signal 'playerDeathComplete'
        QtMocHelpers::SignalData<void()>(36, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GameScene, qt_meta_tag_ZN9GameSceneE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject GameScene::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsScene::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9GameSceneE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9GameSceneE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9GameSceneE_t>.metaTypes,
    nullptr
} };

void GameScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GameScene *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->playerHpChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->bossHpChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 2: _t->phaseChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->introStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->introBossAppearing(); break;
        case 5: _t->introBossHpBarShow(); break;
        case 6: _t->bossDeathStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->bossDeathCameraMove((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 8: _t->bossDeathFadeOut(); break;
        case 9: _t->bossDeathFadeIn(); break;
        case 10: _t->bossDeathFadeToBlack(); break;
        case 11: _t->bossDeathCameraToPlayer(); break;
        case 12: _t->bossDeathComplete(); break;
        case 13: _t->phase2DeathStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->phase2DeathCameraMove((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 15: _t->phase2DeathFadeOut(); break;
        case 16: _t->phase2DeathCleanup(); break;
        case 17: _t->phase2DeathComplete(); break;
        case 18: _t->victoryStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 19: _t->victoryImageFadeIn((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 20: _t->victoryScreenFadeToBlack((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 21: _t->victoryComplete(); break;
        case 22: _t->playerDeathStageChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 23: _t->playerDeathPlayerFadeOut((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 24: _t->playerDeathImageFadeIn((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 25: _t->playerDeathScreenFadeToBlack((*reinterpret_cast<std::add_pointer_t<qreal>>(_a[1]))); break;
        case 26: _t->playerDeathComplete(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(int , int )>(_a, &GameScene::playerHpChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(int , int )>(_a, &GameScene::bossHpChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(int )>(_a, &GameScene::phaseChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(int )>(_a, &GameScene::introStageChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::introBossAppearing, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::introBossHpBarShow, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(int )>(_a, &GameScene::bossDeathStageChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(qreal , qreal , int )>(_a, &GameScene::bossDeathCameraMove, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::bossDeathFadeOut, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::bossDeathFadeIn, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::bossDeathFadeToBlack, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::bossDeathCameraToPlayer, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::bossDeathComplete, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(int )>(_a, &GameScene::phase2DeathStageChanged, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(qreal , qreal , int )>(_a, &GameScene::phase2DeathCameraMove, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::phase2DeathFadeOut, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::phase2DeathCleanup, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::phase2DeathComplete, 17))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(int )>(_a, &GameScene::victoryStageChanged, 18))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(qreal )>(_a, &GameScene::victoryImageFadeIn, 19))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(qreal )>(_a, &GameScene::victoryScreenFadeToBlack, 20))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::victoryComplete, 21))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(int )>(_a, &GameScene::playerDeathStageChanged, 22))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(qreal )>(_a, &GameScene::playerDeathPlayerFadeOut, 23))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(qreal )>(_a, &GameScene::playerDeathImageFadeIn, 24))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)(qreal )>(_a, &GameScene::playerDeathScreenFadeToBlack, 25))
            return;
        if (QtMocHelpers::indexOfMethod<void (GameScene::*)()>(_a, &GameScene::playerDeathComplete, 26))
            return;
    }
}

const QMetaObject *GameScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GameScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9GameSceneE_t>.strings))
        return static_cast<void*>(this);
    return QGraphicsScene::qt_metacast(_clname);
}

int GameScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void GameScene::playerHpChanged(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void GameScene::bossHpChanged(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void GameScene::phaseChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void GameScene::introStageChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void GameScene::introBossAppearing()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void GameScene::introBossHpBarShow()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void GameScene::bossDeathStageChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void GameScene::bossDeathCameraMove(qreal _t1, qreal _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2, _t3);
}

// SIGNAL 8
void GameScene::bossDeathFadeOut()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void GameScene::bossDeathFadeIn()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void GameScene::bossDeathFadeToBlack()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void GameScene::bossDeathCameraToPlayer()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void GameScene::bossDeathComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void GameScene::phase2DeathStageChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void GameScene::phase2DeathCameraMove(qreal _t1, qreal _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1, _t2, _t3);
}

// SIGNAL 15
void GameScene::phase2DeathFadeOut()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void GameScene::phase2DeathCleanup()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void GameScene::phase2DeathComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void GameScene::victoryStageChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 18, nullptr, _t1);
}

// SIGNAL 19
void GameScene::victoryImageFadeIn(qreal _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 19, nullptr, _t1);
}

// SIGNAL 20
void GameScene::victoryScreenFadeToBlack(qreal _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 20, nullptr, _t1);
}

// SIGNAL 21
void GameScene::victoryComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 21, nullptr);
}

// SIGNAL 22
void GameScene::playerDeathStageChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 22, nullptr, _t1);
}

// SIGNAL 23
void GameScene::playerDeathPlayerFadeOut(qreal _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 23, nullptr, _t1);
}

// SIGNAL 24
void GameScene::playerDeathImageFadeIn(qreal _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 24, nullptr, _t1);
}

// SIGNAL 25
void GameScene::playerDeathScreenFadeToBlack(qreal _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 25, nullptr, _t1);
}

// SIGNAL 26
void GameScene::playerDeathComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 26, nullptr);
}
QT_WARNING_POP
