#include "bullet.h"
#include <QPen>
#include <QtMath>

Bullet::Bullet(QGraphicsItem *parent) : QGraphicsEllipseItem(parent)
{
}

// ========== 玩家子弹 ==========
Bullet* Bullet::createPlayerBullet(qreal x, qreal y, qreal dx, qreal dy, int speed, QGraphicsItem *parent)
{
    Bullet *bullet = new Bullet(parent);
    bullet->initPlayer(x, y, dx, dy, speed);
    return bullet;
}

void Bullet::initPlayer(qreal x, qreal y, qreal dx, qreal dy, int speed)
{
    m_isBossBullet = false;
    m_isFlying = true;
    m_isHoming = false;
    m_canFission = false;
    m_dx = dx;
    m_dy = dy;
    m_speed = speed;

    setRect(0, 0, 8, 8);
    setPos(x, y);
    setBrush(Qt::cyan);
    setPen(Qt::NoPen);
    setZValue(5);
}

// ========== Boss环绕子弹 ==========
Bullet* Bullet::createBossOrbitingBullet(qreal centerX, qreal centerY, qreal angle, qreal radius, int size, QGraphicsItem *parent)
{
    Bullet *bullet = new Bullet(parent);
    bullet->initBossOrbiting(centerX, centerY, angle, radius, size);
    return bullet;
}

void Bullet::initBossOrbiting(qreal centerX, qreal centerY, qreal angle, qreal radius, int size)
{
    m_isBossBullet = true;
    m_isFlying = false;  // 初始为环绕状态，不是飞行状态
    m_isHoming = false;
    m_canFission = true;  // 环绕子弹具备裂变能力
    m_hasFissioned = false;

    m_orbitCenterX = centerX;
    m_orbitCenterY = centerY;
    m_orbitAngle = angle;
    m_orbitRadius = radius;

    setRect(0, 0, size, size);
    setBrush(QColor(255, 80, 0));
    setPen(QPen(QColor(200, 30, 0), 2));
    setZValue(5);

    qreal x = centerX + qCos(angle) * radius - size / 2.0;
    qreal y = centerY + qSin(angle) * radius - size / 2.0;
    setPos(x, y);
}

// ========== Boss追踪子弹（新增）==========
Bullet* Bullet::createBossHomingBullet(qreal x, qreal y, qreal dx, qreal dy, int speed, int size, QGraphicsItem *parent)
{
    Bullet *bullet = new Bullet(parent);
    bullet->initBossHoming(x, y, dx, dy, speed, size);
    return bullet;
}

void Bullet::initBossHoming(qreal x, qreal y, qreal dx, qreal dy, int speed, int size)
{
    m_isBossBullet = true;
    m_isFlying = true;
    m_isHoming = true;
    m_canFission = false;
    m_dx = dx;
    m_dy = dy;
    m_speed = speed;

    setRect(0, 0, size, size);
    // 紫色填充，白色边框，突出追踪弹的魔法感
    setBrush(QColor(160, 32, 240));
    setPen(QPen(QColor(220, 180, 255), 2));
    setZValue(5);

    setPos(x, y);
    m_lifeTimer.start();
}

// 工厂方法：创建Boss扇形散射子弹
Bullet* Bullet::createBossSpreadBullet(qreal x, qreal y, qreal angle, int speed, int size, QGraphicsItem *parent)
{
    Bullet *bullet = new Bullet(parent);
    bullet->initBossSpread(x, y, angle, speed, size);
    return bullet;
}

// 初始化扇形散射子弹：深红色，直接飞行
void Bullet::initBossSpread(qreal x, qreal y, qreal angle, int speed, int size)
{
    m_isBossBullet = true;
    m_isFlying = true;
    m_isHoming = false;
    m_canFission = false;
    m_dx = qCos(angle);
    m_dy = qSin(angle);
    m_speed = speed;

    setRect(0, 0, size, size);
    // 深红色填充，暗红边框
    setBrush(QColor(180, 20, 20));
    setPen(QPen(QColor(120, 10, 10), 1));
    setZValue(5);

    setPos(x, y);
    m_lifeTimer.start();
}

// ========== 裂变子弹（新增）==========
// 用途：环绕子弹裂变时生成的一圈扩散子弹
Bullet* Bullet::createFissionBullet(qreal x, qreal y, qreal angle, int speed, int size, QGraphicsItem *parent)
{
    Bullet *bullet = new Bullet(parent);
    bullet->initFissionBullet(x, y, angle, speed, size);
    return bullet;
}

// 初始化裂变子弹：橙红色（与母弹区分），无裂变能力
void Bullet::initFissionBullet(qreal x, qreal y, qreal angle, int speed, int size)
{
    m_isBossBullet = true;
    m_isFlying = true;
    m_isHoming = false;
    m_canFission = false;  // 裂变子弹不能再裂变
    m_dx = qCos(angle);
    m_dy = qSin(angle);
    m_speed = speed;

    setRect(0, 0, size, size);
    // 橙红色填充，亮橙边框（与环绕子弹母弹区分）
    setBrush(QColor(255, 120, 30));
    setPen(QPen(QColor(255, 200, 100), 2));
    setZValue(5);

    setPos(x, y);
    m_lifeTimer.start();
}

void Bullet::update()
{
    if (m_isBossBullet && !m_isFlying) {
        // 环绕状态
        m_orbitAngle += ORBIT_ANGULAR_SPEED;

        qreal x = m_orbitCenterX + qCos(m_orbitAngle) * m_orbitRadius - rect().width() / 2.0;
        qreal y = m_orbitCenterY + qSin(m_orbitAngle) * m_orbitRadius - rect().height() / 2.0;
        setPos(x, y);
    } else {
        // 飞行状态（普通子弹或追踪子弹或裂变子弹）
        qreal oldX = pos().x();
        qreal oldY = pos().y();
        setPos(pos().x() + m_dx * m_speed, pos().y() + m_dy * m_speed);

        // 累积飞行距离（用于裂变判断）
        if (m_canFission && !m_hasFissioned) {
            qreal dx = pos().x() - oldX;
            qreal dy = pos().y() - oldY;
            m_flightDistance += qSqrt(dx * dx + dy * dy);
        }
    }
}

// 更新追踪方向：每帧转向玩家，但限制最大转角
void Bullet::updateHoming(qreal targetX, qreal targetY, qreal maxTurn)
{
    if (!m_isHoming) return;

    qreal bulletCenterX = pos().x() + rect().width() / 2.0;
    qreal bulletCenterY = pos().y() + rect().height() / 2.0;

    // 计算朝向玩家的目标角度
    qreal targetDx = targetX - bulletCenterX;
    qreal targetDy = targetY - bulletCenterY;
    qreal targetAngle = qAtan2(targetDy, targetDx);

    // 当前飞行角度
    qreal currentAngle = qAtan2(m_dy, m_dx);

    // 计算角度差，限制在 [-π, π]
    qreal angleDiff = targetAngle - currentAngle;
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;

    // 限制最大转向角度
    if (angleDiff > maxTurn) angleDiff = maxTurn;
    if (angleDiff < -maxTurn) angleDiff = -maxTurn;

    // 应用转向
    qreal newAngle = currentAngle + angleDiff;
    m_dx = qCos(newAngle);
    m_dy = qSin(newAngle);
}

Bullet* Bullet::createFissionableSpreadBullet(qreal x, qreal y, qreal angle, int speed, int size,qreal fissionDist, int fissionCount, int fissionSpeed, int fissionSize,QGraphicsItem *parent)
{
    Bullet *bullet = new Bullet(parent);
    bullet->initBossSpread(x, y, angle, speed, size);
    bullet->m_canFission = true;  // 启用裂变
    bullet->setFissionParams(fissionDist, fissionCount, fissionSpeed, fissionSize);
    return bullet;
}

bool Bullet::isOutOfBounds(qreal sceneWidth, qreal sceneHeight) const
{
    qreal x = pos().x();
    qreal y = pos().y();
    return x < -50 || x > sceneWidth + 50 || y < -50 || y > sceneHeight + 50;
}

bool Bullet::isBossBullet() const
{
    return m_isBossBullet;
}

void Bullet::launchTowards(qreal targetX, qreal targetY, int speed)
{
    if (!m_isBossBullet || m_isFlying) return;

    m_isFlying = true;

    qreal bulletCenterX = pos().x() + rect().width() / 2.0;
    qreal bulletCenterY = pos().y() + rect().height() / 2.0;

    qreal dx = targetX - bulletCenterX;
    qreal dy = targetY - bulletCenterY;
    qreal len = qSqrt(dx * dx + dy * dy);

    if (len < 0.001) {
        m_dx = 1;
        m_dy = 0;
    } else {
        m_dx = dx / len;
        m_dy = dy / len;
    }

    m_speed = speed;

    // 记录飞行起点，用于计算飞行距离
    m_flightStartX = bulletCenterX;
    m_flightStartY = bulletCenterY;
    m_flightDistance = 0;
}

bool Bullet::isFlying() const
{
    return m_isFlying;
}

bool Bullet::isHoming() const
{
    return m_isHoming;
}

void Bullet::setOrbitCenter(qreal x, qreal y)
{
    m_orbitCenterX = x;
    m_orbitCenterY = y;
}

int Bullet::elapsedTime() const
{
    return m_lifeTimer.elapsed();
}

// ========== 裂变相关方法实现（新增）==========

bool Bullet::isFissionCapable() const
{
    return m_canFission;
}

bool Bullet::shouldFission() const
{
    if (!m_canFission || m_hasFissioned || !m_isFlying) return false;

    // 条件1：飞行距离超过阈值
    if (m_maxFlightDistance > 0 && m_flightDistance >= m_maxFlightDistance) {
        return true;
    }

    return false;
}

void Bullet::markFissioned()
{
    m_hasFissioned = true;
}

bool Bullet::hasFissioned() const
{
    return m_hasFissioned;
}

qreal Bullet::getFlightDistance() const
{
    return m_flightDistance;
}

void Bullet::setFissionParams(qreal maxDist, int fissionCount, int fissionSpeed, int fissionSize)
{
    m_maxFlightDistance = maxDist;
    m_fissionCount = fissionCount;
    m_fissionSpeed = fissionSpeed;
    m_fissionSize = fissionSize;
}
