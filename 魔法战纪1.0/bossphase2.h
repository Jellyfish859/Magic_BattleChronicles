#ifndef BOSSPHASE2_H
#define BOSSPHASE2_H

#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QList>
#include <QRectF>

class Bullet;
class WarningRect;
class WarningCircle;

class BossPhase2 : public QGraphicsPixmapItem
{
public:
    explicit BossPhase2(QGraphicsItem *parent = nullptr);
    ~BossPhase2();

    int getHp() const;
    int getMaxHp() const;
    void setHp(int hp);
    void takeDamage(int damage);

    qreal getCenterX() const;
    qreal getCenterY() const;

    QList<Bullet*> updateBoundaryDash(qreal playerX, qreal playerY, const QRectF &validArea);
    QList<Bullet*> updateFinaleSkill(qreal playerX, qreal playerY, const QRectF &validArea);
    QList<Bullet*> updateRingShoot();

    bool isFinaleComplete() const;

    bool updateWarningRect();
    WarningRect* getWarningRect() const;
    WarningCircle* getWarningCircle() const;

    bool checkDashCollision(qreal playerX, qreal playerY, qreal playerRadius) const;

    bool isBoundaryDashing() const;
    bool isSkillComplete() const;

    void updateRandomMove(const QRectF &validArea);

    void setOpacity(qreal opacity);
    qreal getOpacity() const;
    bool isFadingIn() const;
    bool isFadingOut() const;

private:
    int m_hp = 800;
    int m_maxHp = 800;

    enum DashState { Idle, Warning, Dashing, Pausing, Cooldown, FadingOut } m_dashState = Idle;
    QElapsedTimer m_dashTimer;
    int m_dashCount = 0;
    qreal m_dashDirX = 0;
    qreal m_dashDirY = 0;
    qreal m_dashStartX = 0;
    qreal m_dashStartY = 0;
    QElapsedTimer m_shootTimer;


    WarningRect *m_warningRect = nullptr;
    WarningCircle *m_warningCircle = nullptr;

    enum FinaleState { NotStarted, CircleWarning, BossAppearing, FinaleDone } m_finaleState = NotStarted;
    QElapsedTimer m_finaleTimer;

    qreal m_opacity = 1.0;
    static constexpr int FADE_DURATION = 500;

    static constexpr int MAX_DASH_COUNT = 3;
    static constexpr int WARNING_DURATION = 1200;
    static constexpr qreal DASH_SPEED = 12.0;
    static constexpr int SHOOT_INTERVAL = 100;
    static constexpr int SIDE_BULLET_COUNT = 2;
    static constexpr int SIDE_BULLET_SPEED = 9;
    static constexpr int SIDE_BULLET_SIZE = 12;
    static constexpr qreal SIDE_ANGLE_RANGE = M_PI / 2.5;
    static constexpr int PAUSE_DURATION = 800;
    static constexpr int DASH_COLLISION_DAMAGE = 35;


    static constexpr qreal FINALE_RADIUS = 200.0;
    static constexpr int FINALE_DAMAGE = 50;
    static constexpr int FINALE_BULLET_COUNT = 6;
    static constexpr int FINALE_BULLET_SIZE = 28;
    static constexpr int FINALE_BULLET_SPEED = 5;
    static constexpr qreal FINALE_FISSION_DIST = 350.0;
    static constexpr int FINALE_FISSION_COUNT = 8;
    static constexpr int FINALE_FISSION_SPEED = 5;
    static constexpr int FINALE_FISSION_SIZE = 16;

    static constexpr qreal MOVE_SPEED = 2.2;   // 二阶段移速（适中）
    static constexpr int MOVE_DIRECTION_CHANGE_INTERVAL = 2000;  // 每2秒可能换方向

    // 环形发射子弹系统（新增）
    // 用途：技能完成后，Boss定期向周围环形发射大量普通散射子弹，形成高密度弹幕
    QElapsedTimer m_ringShootTimer;
    static constexpr int RING_SHOOT_INTERVAL = 1800;   // 攻击频率加快：从2500ms缩短到1800ms（约1.8秒一次）
    static constexpr int RING_BULLET_COUNT = 24;         // 每次24颗子弹（高密度弹幕）
    static constexpr int RING_BULLET_SPEED = 2;        // 子弹速度更慢（大幅减缓，增加压迫感）
    static constexpr int RING_BULLET_SIZE = 20;        // 子弹尺寸增大（更容易被看到和躲避）

    qreal m_moveDirX = 0;
    qreal m_moveDirY = 0;
    QElapsedTimer m_moveDirTimer;

    QPointF randomBoundaryPoint(const QRectF &validArea);
    QList<Bullet*> spawnSideBullets(qreal centerX, qreal centerY);
    QList<Bullet*> spawnRingBullets(qreal centerX, qreal centerY, int count, int speed, int size);
    void applyOpacity();
};

#endif // BOSSPHASE2_H
