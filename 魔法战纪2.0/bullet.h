#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsEllipseItem>
#include <QElapsedTimer>

class Bullet : public QGraphicsEllipseItem
{
public:
    // 创建玩家子弹
    static Bullet* createPlayerBullet(qreal x, qreal y, qreal dx, qreal dy, int speed, QGraphicsItem *parent = nullptr);

    // 创建Boss环绕子弹
    static Bullet* createBossOrbitingBullet(qreal centerX, qreal centerY, qreal angle, qreal radius, int size, QGraphicsItem *parent = nullptr);

    // 创建Boss追踪子弹
    static Bullet* createBossHomingBullet(qreal x, qreal y, qreal dx, qreal dy, int speed, int size, QGraphicsItem *parent = nullptr);

    // 创建Boss扇形散射子弹（新增）
    static Bullet* createBossSpreadBullet(qreal x, qreal y, qreal angle, int speed, int size, QGraphicsItem *parent = nullptr);

    // 创建裂变子弹（新增）
    // 用途：环绕子弹裂变时生成的一圈扩散子弹
    static Bullet* createFissionBullet(qreal x, qreal y, qreal angle, int speed, int size, QGraphicsItem *parent = nullptr);

    static Bullet* createFissionableSpreadBullet(qreal x, qreal y, qreal angle, int speed, int size,qreal fissionDist, int fissionCount, int fissionSpeed, int fissionSize, QGraphicsItem *parent = nullptr);

    void update();
    bool isOutOfBounds(qreal sceneWidth, qreal sceneHeight) const;

    bool isBossBullet() const;

    void launchTowards(qreal targetX, qreal targetY, int speed);

    void updateHoming(qreal targetX, qreal targetY, qreal maxTurn);

    bool isFlying() const;

    bool isHoming() const;

    void setOrbitCenter(qreal x, qreal y);

    int elapsedTime() const;

    // 裂变相关（新增）
    bool isFissionCapable() const;      // 是否具备裂变能力（仅环绕子弹）
    bool shouldFission() const;         // 是否满足裂变条件
    void markFissioned();               // 标记已裂变（防止重复裂变）
    bool hasFissioned() const;          // 是否已经裂变过
    qreal getFlightDistance() const;    // 获取已飞行距离
    void setFissionParams(qreal maxDist, int fissionCount, int fissionSpeed, int fissionSize);  // 设置裂变参数

private:
    explicit Bullet(QGraphicsItem *parent = nullptr);

    void initPlayer(qreal x, qreal y, qreal dx, qreal dy, int speed);
    void initBossOrbiting(qreal centerX, qreal centerY, qreal angle, qreal radius, int size);
    void initBossHoming(qreal x, qreal y, qreal dx, qreal dy, int speed, int size);
    void initBossSpread(qreal x, qreal y, qreal angle, int speed, int size);
    void initFissionBullet(qreal x, qreal y, qreal angle, int speed, int size);  // 新增

    qreal m_dx = 0;
    qreal m_dy = 0;
    int m_speed = 0;

    bool m_isBossBullet = false;
    bool m_isFlying = true;
    bool m_isHoming = false;

    // 环绕参数
    qreal m_orbitCenterX = 0;
    qreal m_orbitCenterY = 0;
    qreal m_orbitAngle = 0;
    qreal m_orbitRadius = 0;
    static constexpr qreal ORBIT_ANGULAR_SPEED = 0.025;

    // 追踪参数
    QElapsedTimer m_lifeTimer;

    // 裂变参数（新增）
    bool m_canFission = false;          // 是否可裂变
    bool m_hasFissioned = false;        // 是否已裂变
    qreal m_flightDistance = 0;         // 已飞行距离
    qreal m_flightStartX = 0;           // 飞行起点X
    qreal m_flightStartY = 0;           // 飞行起点Y
    qreal m_maxFlightDistance = 0;      // 最大飞行距离（超过则裂变）
    int m_fissionCount = 0;             // 裂变子弹数量
    int m_fissionSpeed = 0;             // 裂变子弹速度
    int m_fissionSize = 0;              // 裂变子弹尺寸
};

#endif // BULLET_H
