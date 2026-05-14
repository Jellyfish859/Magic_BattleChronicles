#ifndef BOSS_H
#define BOSS_H

#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QList>
#include <QRectF>

class Bullet;

class Boss : public QGraphicsPixmapItem
{
public:
    explicit Boss(QGraphicsItem *parent = nullptr);

    int getHp() const;
    int getMaxHp() const;
    void setHp(int hp);
    void takeDamage(int damage);

    qreal getCenterX() const;
    qreal getCenterY() const;

    // 技能状态系统
    enum class SkillState { Idle, Orbiting, Homing, Spread };
    SkillState getCurrentSkillState() const;
    bool isAnySkillActive() const;
    void updateSkillState();
    void pickNextSkill();

    // 环绕子弹系统
    bool shouldSpawnOrbitSkill() const;
    QList<Bullet*> spawnOrbitingBullets();
    static constexpr int getLaunchInterval() { return LAUNCH_INTERVAL; }
    static constexpr int getOrbBulletSpeed() { return ORB_BULLET_SPEED; }

    // 追踪子弹系统
    bool shouldSpawnHomingSkill() const;
    QList<Bullet*> spawnHomingBullets(qreal playerX, qreal playerY);
    static constexpr int getHomingBulletSpeed() { return HOMING_BULLET_SPEED; }
    static constexpr qreal getHomingMaxTurn() { return HOMING_MAX_TURN; }
    static constexpr int getHomingLifeTime() { return HOMING_LIFE_TIME; }

    // 扇形散射系统
    bool shouldSpawnSpreadSkill() const;
    Bullet* getNextSpreadBullet(qreal playerX, qreal playerY);
    bool isSpreadActive() const;
    static constexpr int getSpreadBulletSpeed() { return SPREAD_BULLET_SPEED; }
    static constexpr int getSpreadBulletSize() { return SPREAD_BULLET_SIZE; }

    // 三段式冲刺扩散技能
    bool shouldSpawnDashSkill() const;
    QList<Bullet*> updateDashState(qreal playerX, qreal playerY, const QRectF &validArea);
    bool isDashing() const;

    // 缓慢移动
    void moveTowardsPlayer(qreal playerX, qreal playerY, const QRectF &validArea);

    // 开场动画系统
    void setIntroProgress(qreal progress);
    void setIntroOffset(qreal offsetY);
    qreal getIntroOpacity() const;
    qreal getIntroOffset() const;
    bool isIntroComplete() const;
    void finishIntro();

    // 死亡演出系统（新增）
    // 用途：死亡演出期间需要直接控制Boss透明度（闪烁、淡出、淡入）
    // 为什么：死亡演出的闪烁和淡出需要频繁修改透明度，复用开场动画的参数
    void setIntroOpacity(qreal opacity);

private:
    int m_hp = 500;
    int m_maxHp = 500;

    // 环绕子弹
    QElapsedTimer m_orbSkillTimer;
    static constexpr int ORB_SKILL_INTERVAL = 8000;
    static constexpr int ORB_BULLET_COUNT = 4;
    static constexpr int ORB_BULLET_SIZE = 32;
    static constexpr qreal ORB_RADIUS = 130;
    static constexpr int LAUNCH_INTERVAL = 500;
    static constexpr int ORB_BULLET_SPEED = 6;

    // 追踪子弹
    QElapsedTimer m_homingSkillTimer;
    static constexpr int HOMING_SKILL_INTERVAL = 6000;
    static constexpr int HOMING_BULLET_COUNT = 6;
    static constexpr int HOMING_BULLET_SIZE = 16;
    static constexpr int HOMING_BULLET_SPEED = 4;
    static constexpr qreal HOMING_MAX_TURN = 0.08;
    static constexpr int HOMING_LIFE_TIME = 4000;

    // 扇形散射
    QElapsedTimer m_spreadTimer;
    QElapsedTimer m_spreadShotTimer;
    bool m_spreadActive = false;
    int m_spreadShotCount = 0;

    static constexpr int SPREAD_INTERVAL = 7000;
    static constexpr int SPREAD_SHOT_COUNT = 16;
    static constexpr int SPREAD_SHOT_INTERVAL = 50;
    static constexpr qreal SPREAD_ANGLE_RANGE = M_PI / 2.0;
    static constexpr int SPREAD_BULLET_SPEED = 8;
    static constexpr int SPREAD_BULLET_SIZE = 14;

    // 三段式冲刺扩散
    QElapsedTimer m_dashSkillTimer;
    QElapsedTimer m_dashPhaseTimer;
    bool m_isDashing = false;
    qreal m_dashDirX = 0;
    qreal m_dashDirY = 0;
    qreal m_dashStartX = 0;
    qreal m_dashStartY = 0;
    bool m_dashMidFired = false;
    bool m_dashEndFired = false;

    static constexpr int DASH_SKILL_INTERVAL = 12000;
    static constexpr int DASH_DURATION = 1500;
    static constexpr qreal DASH_SPEED = 5.5;
    static constexpr int DASH_BULLET_COUNT = 12;
    static constexpr int DASH_BULLET_SIZE = 24;
    static constexpr int DASH_BULLET_SPEED = 8;

    // 环绕子弹裂变参数
    static constexpr qreal ORB_FISSION_DISTANCE = 450.0;
    static constexpr int ORB_FISSION_COUNT = 8;
    static constexpr int ORB_FISSION_SPEED = 5;
    static constexpr int ORB_FISSION_SIZE = 16;

    // 技能状态
    SkillState m_currentSkill = SkillState::Idle;
    QElapsedTimer m_skillCooldownTimer;
    static constexpr int SKILL_COOLDOWN_MS = 1000;

    // 技能执行计时器
    QElapsedTimer m_orbExecTimer;
    QElapsedTimer m_homingExecTimer;
    QElapsedTimer m_spreadExecTimer;

    // 技能触发标志
    bool m_orbTrigger = false;
    bool m_homingTrigger = false;
    bool m_spreadTrigger = false;

    // 移动
    static constexpr qreal MOVE_SPEED = 0.3;

    // 开场动画参数
    qreal m_introOpacity = 0.0;
    qreal m_introOffsetY = 100.0;
    bool m_introComplete = false;

    QList<Bullet*> spawnRingBullets(qreal centerX, qreal centerY, int count, int speed, int size);
};

#endif // BOSS_H
