#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QPixmap>
#include <QList>
#include <QElapsedTimer>
#include <QRectF>
#include "warningcircle.h"

class Player;
class Boss;
class BossPhase2;
class BossClone;
class Bullet;

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GameScene(QObject *parent = nullptr);
    Player* getPlayer() const;
    Boss* getBoss() const;
    BossPhase2* getBossPhase2() const;

    QRectF getValidArea() const;

    void processShoot();
    void processPotion();
    void updateLockTarget();

    void updateBullets();
    void checkBulletCollisions();
    void cleanupBullets();

    void updateBossOrbitingBullets();
    void updateBossHomingBullets();
    void updateBossSpread();
    void updateBossDash();
    void updateBossSkillState();  // 新增：更新一阶段Boss技能状态
    void updateBossClones();  // 新增：更新Boss分身弹幕
    void updateFissionBullets();
    void updateBossPhase2();  // 新增：更新二阶段Boss
    void updateBossPhase2Finale();

    void checkPhaseTransition();

signals:
    void playerHpChanged(int hp, int maxHp);
    void bossHpChanged(int hp, int maxHp);
    void phaseChanged(int phase);

private:
    void setupBackground();
    void spawnBossClones();  // 新增：在终结技后生成Boss分身

    QList<Bullet*> spawnFissionBullets(Bullet *sourceBullet);

    QPixmap m_background;
    Player *m_player;
    Boss *m_boss;
    BossPhase2 *m_bossPhase2 = nullptr;

    QList<Bullet*> m_bullets;
    QList<Bullet*> m_bossOrbitingBullets;
    QElapsedTimer m_orbLaunchTimer;
    bool m_orbitingActive = false;

    QList<Bullet*> m_bossHomingBullets;
    QList<Bullet*> m_bossSpreadBullets;
    QList<Bullet*> m_bossDashBullets;
    QList<Bullet*> m_fissionBullets;
    QList<Bullet*> m_phase2Bullets;  // 新增：二阶段子弹
    QList<BossClone*> m_bossClones;  // 新增：Boss分身列表
    bool m_clonesSpawned = false;  // 新增：分身是否已生成

    QRectF m_validArea;

    int m_currentPhase = 1;
    void updateBossPhase2WarningRect();
    bool m_phaseTransitionDone = false;
};

#endif // GAMESCENE_H
