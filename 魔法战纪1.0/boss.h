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

    // 技能状态系统（新增）
    // 用途：实现三个技能（环绕、追踪、扇形）轮流随机释放，每次只放一个，放完才能放下一个
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

    // 缓慢移动（必须传3个参数，避免歧义）
    void moveTowardsPlayer(qreal playerX, qreal playerY, const QRectF &validArea);

private:
    int m_hp = 500;
    int m_maxHp = 500;

    // 环绕子弹
    QElapsedTimer m_orbSkillTimer;
    static constexpr int ORB_SKILL_INTERVAL = 8000;  // 缩短2秒，更频繁触发
    static constexpr int ORB_BULLET_COUNT = 4;
    static constexpr int ORB_BULLET_SIZE = 32;
    static constexpr qreal ORB_RADIUS = 130;
    static constexpr int LAUNCH_INTERVAL = 500;  // 缩短200ms，更快发射环绕子弹
    static constexpr int ORB_BULLET_SPEED = 6;

    // 追踪子弹
    QElapsedTimer m_homingSkillTimer;
    static constexpr int HOMING_SKILL_INTERVAL = 6000;  // 缩短2秒，更频繁触发
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

    static constexpr int SPREAD_INTERVAL = 7000;  // 缩短2秒，更频繁触发
    static constexpr int SPREAD_SHOT_COUNT = 16;  // 从10增加到16，更多子弹
    static constexpr int SPREAD_SHOT_INTERVAL = 50;  // 从80→60→50，更快散射，持续时间更长
    static constexpr qreal SPREAD_ANGLE_RANGE = M_PI / 2.0;  // 从60°增加到90°，更宽扇形
    static constexpr int SPREAD_BULLET_SPEED = 8;  // 从7增加到8，更快
    static constexpr int SPREAD_BULLET_SIZE = 14;  // 从10增加到14，更大子弹

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
    static constexpr int DASH_BULLET_SIZE = 24;     // 加大：从14改为20
    static constexpr int DASH_BULLET_SPEED = 8;

    // 环绕子弹裂变参数（新增）
    static constexpr qreal ORB_FISSION_DISTANCE = 450.0;  // 飞行超过300像素触发裂变
    static constexpr int ORB_FISSION_COUNT = 8;           // 裂变8颗子弹
    static constexpr int ORB_FISSION_SPEED = 5;           // 裂变子弹速度
    static constexpr int ORB_FISSION_SIZE = 16;           // 裂变子弹尺寸

    // 技能状态（新增）
    // 用途：实现三个技能轮流随机释放，每次只放一个，放完才能放下一个
    SkillState m_currentSkill = SkillState::Idle;
    QElapsedTimer m_skillCooldownTimer;
    static constexpr int SKILL_COOLDOWN_MS = 1000;  // 技能间隔1秒（缩短，出招更密集）

    // 技能执行计时器（新增）
    // 用途：记录技能实际执行了多久，用于判断技能是否完成
    // 为什么：spawn 函数会 restart 技能间隔计时器，导致无法判断技能是否执行完毕
    //        所以用独立的计时器，只在技能开始时启动一次，不被 restart 干扰
    // 注意：必须在构造函数中 start()，否则 restart() 在 invalid 计时器上是未定义行为
    QElapsedTimer m_orbExecTimer;
    QElapsedTimer m_homingExecTimer;
    QElapsedTimer m_spreadExecTimer;

    // 技能触发标志（新增）
    // 用途：当 pickNextSkill 选择了一个技能后，设置对应标志为 true
    //       shouldSpawn 函数检查此标志，如果为 true 则触发技能并清除标志
    bool m_orbTrigger = false;
    bool m_homingTrigger = false;
    bool m_spreadTrigger = false;

    // 移动
    static constexpr qreal MOVE_SPEED = 0.3;

    QList<Bullet*> spawnRingBullets(qreal centerX, qreal centerY, int count, int speed, int size);
};

#endif // BOSS_H
