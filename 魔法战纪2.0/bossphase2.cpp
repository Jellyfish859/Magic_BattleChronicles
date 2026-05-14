#include "bossphase2.h"
#include "bullet.h"
#include "warningrect.h"
#include "warningcircle.h"
#include <QtMath>
#include <QRandomGenerator>
#include <QGraphicsScene>

BossPhase2::BossPhase2(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    QPixmap pixmap(":/images/images/test_boss.png");
    setPixmap(pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setPos(640 - 60, 640 - 60);
    setZValue(0);

    m_opacity = 0.0;
    applyOpacity();

    m_warningRect = new WarningRect();
    m_warningRect->setVisible(false);

    m_warningCircle = new WarningCircle();
    m_warningCircle->setVisible(false);

    // 启动环形发射计时器（新增）
    m_ringShootTimer.start();
}

BossPhase2::~BossPhase2()
{
    if (m_warningRect) {
        if (m_warningRect->scene()) {
            m_warningRect->scene()->removeItem(m_warningRect);
        }
        delete m_warningRect;
        m_warningRect = nullptr;
    }

    if (m_warningCircle) {
        if (m_warningCircle->scene()) {
            m_warningCircle->scene()->removeItem(m_warningCircle);
        }
        delete m_warningCircle;
        m_warningCircle = nullptr;
    }
}

WarningRect* BossPhase2::getWarningRect() const
{
    return m_warningRect;
}

WarningCircle* BossPhase2::getWarningCircle() const
{
    return m_warningCircle;
}

bool BossPhase2::updateWarningRect()
{
    if (!m_warningRect || !m_warningRect->isVisible()) return false;
    return m_warningRect->updateState();
}

void BossPhase2::setOpacity(qreal opacity)
{
    m_opacity = qBound(0.0, opacity, 1.0);
    applyOpacity();
}

qreal BossPhase2::getOpacity() const
{
    return m_opacity;
}

// setDeathOpacity: 设置死亡演出透明度
void BossPhase2::setDeathOpacity(qreal opacity)
{
    m_deathOpacity = qBound(0.0, opacity, 1.0);
    // 应用死亡透明度（覆盖正常透明度）
    QGraphicsItem::setOpacity(m_deathOpacity);
}

qreal BossPhase2::getDeathOpacity() const
{
    return m_deathOpacity;
}

void BossPhase2::applyOpacity()

{
    QGraphicsItem::setOpacity(m_opacity);
}

bool BossPhase2::isFadingIn() const
{
    return m_dashState == Warning && m_dashTimer.elapsed() < FADE_DURATION;
}

bool BossPhase2::isFadingOut() const
{
    return m_dashState == FadingOut;
}

// updateRandomMove: 二阶段Boss随机方向直线移动
void BossPhase2::updateRandomMove(const QRectF &validArea)
{
    // 只有在非冲刺状态且技能完成后才执行随机移动
    if (m_dashState != Idle || !isSkillComplete()) return;

    qreal halfW = pixmap().width() / 2;
    qreal halfH = pixmap().height() / 2;

    // 初始化随机方向（首次调用或长时间未改变方向时）
    if (m_moveDirX == 0 && m_moveDirY == 0) {
        qreal angle = QRandomGenerator::global()->generateDouble() * 2.0 * M_PI;
        m_moveDirX = qCos(angle);
        m_moveDirY = qSin(angle);
        m_moveDirTimer.start();
    }

    // 计算新位置
    qreal newX = pos().x() + m_moveDirX * MOVE_SPEED;
    qreal newY = pos().y() + m_moveDirY * MOVE_SPEED;

    qreal bossX = newX + halfW;
    qreal bossY = newY + halfH;

    bool hitBoundary = false;

    // 检测左右边界碰撞
    if (bossX < validArea.left() + halfW) {
        newX = validArea.left();
        hitBoundary = true;
    } else if (bossX > validArea.right() - halfW) {
        newX = validArea.right() - pixmap().width();
        hitBoundary = true;
    }

    // 检测上下边界碰撞
    if (bossY < validArea.top() + halfH) {
        newY = validArea.top();
        hitBoundary = true;
    } else if (bossY > validArea.bottom() - halfH) {
        newY = validArea.bottom() - pixmap().height();
        hitBoundary = true;
    }

    // 如果碰到边界，随机选择一个新方向（确保不会直接弹回墙里）
    if (hitBoundary) {
        qreal minAngle, maxAngle;

        if (bossX <= validArea.left() + halfW) {
            minAngle = -M_PI / 2 + 0.2;
            maxAngle = M_PI / 2 - 0.2;
        } else if (bossX >= validArea.right() - halfW) {
            minAngle = M_PI / 2 + 0.2;
            maxAngle = 3 * M_PI / 2 - 0.2;
        } else if (bossY <= validArea.top() + halfH) {
            minAngle = 0.2;
            maxAngle = M_PI - 0.2;
        } else {
            minAngle = -M_PI + 0.2;
            maxAngle = -0.2;
        }

        qreal angle = minAngle + QRandomGenerator::global()->generateDouble() * (maxAngle - minAngle);
        m_moveDirX = qCos(angle);
        m_moveDirY = qSin(angle);
        m_moveDirTimer.restart();
    }

    // 定期随机改变方向（即使没有碰墙）
    if (m_moveDirTimer.elapsed() >= MOVE_DIRECTION_CHANGE_INTERVAL) {
        qreal angle = QRandomGenerator::global()->generateDouble() * 2.0 * M_PI;
        m_moveDirX = qCos(angle);
        m_moveDirY = qSin(angle);
        m_moveDirTimer.restart();
    }

    setPos(newX, newY);
}

bool BossPhase2::checkDashCollision(qreal playerX, qreal playerY, qreal playerRadius) const
{
    if (m_dashState != Dashing) return false;

    qreal bossX = getCenterX();
    qreal bossY = getCenterY();
    qreal bossRadius = pixmap().width() / 2;

    qreal dx = bossX - playerX;
    qreal dy = bossY - playerY;
    qreal dist = qSqrt(dx * dx + dy * dy);

    return dist <= bossRadius + playerRadius;
}

bool BossPhase2::isBoundaryDashing() const
{
    return m_dashState == Dashing;
}

QPointF BossPhase2::randomBoundaryPoint(const QRectF &validArea)
{
    int side = QRandomGenerator::global()->bounded(4);
    qreal halfW = pixmap().width() / 2;
    qreal halfH = pixmap().height() / 2;

    qreal x, y;

    switch (side) {
    case 0:
        x = validArea.left() + halfW + QRandomGenerator::global()->bounded(validArea.width() - pixmap().width());
        y = validArea.top() + halfH;
        break;
    case 1:
        x = validArea.right() - halfW;
        y = validArea.top() + halfH + QRandomGenerator::global()->bounded(validArea.height() - pixmap().height());
        break;
    case 2:
        x = validArea.left() + halfW + QRandomGenerator::global()->bounded(validArea.width() - pixmap().width());
        y = validArea.bottom() - halfH;
        break;
    case 3:
        x = validArea.left() + halfW;
        y = validArea.top() + halfH + QRandomGenerator::global()->bounded(validArea.height() - pixmap().height());
        break;
    }

    return QPointF(x, y);
}

QList<Bullet*> BossPhase2::spawnSideBullets(qreal centerX, qreal centerY)
{
    QList<Bullet*> bullets;

    qreal perpX = -m_dashDirY;
    qreal perpY = m_dashDirX;

    for (int i = 0; i < SIDE_BULLET_COUNT; ++i) {
        qreal leftAngle = qAtan2(perpY, perpX) +
                          (QRandomGenerator::global()->generateDouble() - 0.5) * SIDE_ANGLE_RANGE;
        bullets.append(Bullet::createBossSpreadBullet(
            centerX - SIDE_BULLET_SIZE / 2.0,
            centerY - SIDE_BULLET_SIZE / 2.0,
            leftAngle,
            SIDE_BULLET_SPEED,
            SIDE_BULLET_SIZE
            ));

        qreal rightAngle = qAtan2(-perpY, -perpX) +
                           (QRandomGenerator::global()->generateDouble() - 0.5) * SIDE_ANGLE_RANGE;
        bullets.append(Bullet::createBossSpreadBullet(
            centerX - SIDE_BULLET_SIZE / 2.0,
            centerY - SIDE_BULLET_SIZE / 2.0,
            rightAngle,
            SIDE_BULLET_SPEED,
            SIDE_BULLET_SIZE
            ));
    }

    return bullets;
}

QList<Bullet*> BossPhase2::spawnRingBullets(qreal centerX, qreal centerY, int count, int speed, int size)
{
    QList<Bullet*> bullets;
    for (int i = 0; i < count; ++i) {
        qreal angle = i * (2.0 * M_PI / count);
        bullets.append(Bullet::createBossSpreadBullet(
            centerX - size / 2.0,
            centerY - size / 2.0,
            angle,
            speed,
            size
            ));
    }
    return bullets;
}

QList<Bullet*> BossPhase2::updateBoundaryDash(qreal playerX, qreal playerY, const QRectF &validArea)
{
    QList<Bullet*> newBullets;

    if (m_dashCount >= MAX_DASH_COUNT && m_dashState == Idle) return newBullets;

    switch (m_dashState) {
    case Idle: {
        m_dashState = Warning;
        m_dashTimer.restart();

        QPointF spawnPoint = randomBoundaryPoint(validArea);
        setPos(spawnPoint.x() - pixmap().width() / 2, spawnPoint.y() - pixmap().height() / 2);

        m_dashStartX = spawnPoint.x();
        m_dashStartY = spawnPoint.y();

        qreal dx = playerX - m_dashStartX;
        qreal dy = playerY - m_dashStartY;
        qreal len = qSqrt(dx * dx + dy * dy);

        if (len < 0.001) {
            m_dashDirX = 1;
            m_dashDirY = 0;
        } else {
            m_dashDirX = dx / len;
            m_dashDirY = dy / len;
        }

        qreal angle = qAtan2(m_dashDirY, m_dashDirX);
        qreal halfW = pixmap().width() / 2;
        qreal halfH = pixmap().height() / 2;

        qreal tMax = 99999;
        if (m_dashDirX > 0.001) {
            qreal t = (validArea.right() - halfW - m_dashStartX) / m_dashDirX;
            if (t > 0 && t < tMax) tMax = t;
        } else if (m_dashDirX < -0.001) {
            qreal t = (validArea.left() + halfW - m_dashStartX) / m_dashDirX;
            if (t > 0 && t < tMax) tMax = t;
        }
        if (m_dashDirY > 0.001) {
            qreal t = (validArea.bottom() - halfH - m_dashStartY) / m_dashDirY;
            if (t > 0 && t < tMax) tMax = t;
        } else if (m_dashDirY < -0.001) {
            qreal t = (validArea.top() + halfH - m_dashStartY) / m_dashDirY;
            if (t > 0 && t < tMax) tMax = t;
        }

        if (m_warningRect) {
            m_warningRect->activate(m_dashStartX, m_dashStartY, angle, tMax);
        }

        m_opacity = 0.0;
        applyOpacity();

        break;
    }

    case Warning: {
        if (m_warningRect) {
            m_warningRect->updateState();
        }

        int elapsed = m_dashTimer.elapsed();
        if (elapsed < FADE_DURATION) {
            m_opacity = static_cast<qreal>(elapsed) / FADE_DURATION;
            applyOpacity();
        } else {
            m_opacity = 1.0;
            applyOpacity();
        }

        if (elapsed >= WARNING_DURATION) {
            m_dashState = Dashing;
            m_dashTimer.restart();
            m_shootTimer.restart();
        }
        break;
    }

    case Dashing: {
        if (m_warningRect && m_warningRect->isVisible()) {
            m_warningRect->setVisible(false);
        }

        qreal bossX = getCenterX();
        qreal bossY = getCenterY();

        qreal newX = pos().x() + m_dashDirX * DASH_SPEED;
        qreal newY = pos().y() + m_dashDirY * DASH_SPEED;
        setPos(newX, newY);

        qreal halfW = pixmap().width() / 2;
        qreal halfH = pixmap().height() / 2;
        bool outOfBounds = (bossX < validArea.left() + halfW ||
                            bossX > validArea.right() - halfW ||
                            bossY < validArea.top() + halfH ||
                            bossY > validArea.bottom() - halfH);

        if (m_shootTimer.elapsed() >= SHOOT_INTERVAL) {
            newBullets.append(spawnSideBullets(bossX, bossY));
            m_shootTimer.restart();
        }

        if (outOfBounds) {
            qreal clampedX = qBound(validArea.left() + halfW, bossX, validArea.right() - halfW);
            qreal clampedY = qBound(validArea.top() + halfH, bossY, validArea.bottom() - halfH);
            setPos(clampedX - halfW, clampedY - halfH);

            m_dashState = FadingOut;
            m_dashTimer.restart();
        }
        break;
    }

    case FadingOut: {
        int elapsed = m_dashTimer.elapsed();
        if (elapsed < FADE_DURATION) {
            m_opacity = 1.0 - static_cast<qreal>(elapsed) / FADE_DURATION;
            applyOpacity();
        } else {
            m_opacity = 0.0;
            applyOpacity();

            m_dashState = Pausing;
            m_dashTimer.restart();
        }
        break;
    }

    case Pausing: {
        if (m_dashTimer.elapsed() >= PAUSE_DURATION) {
            m_dashCount++;

            if (m_dashCount >= MAX_DASH_COUNT) {
                m_opacity = 1.0;
                applyOpacity();
                m_dashState = Idle;
            } else {
                m_dashState = Cooldown;
                m_dashTimer.restart();
            }
        }
        break;
    }

    case Cooldown: {
        if (m_dashTimer.elapsed() >= 300) {
            m_dashState = Idle;
        }
        break;
    }
    }

    return newBullets;
}


QList<Bullet*> BossPhase2::updateFinaleSkill(qreal playerX, qreal playerY, const QRectF &validArea)
{
    QList<Bullet*> newBullets;

    if (m_finaleState == FinaleDone) return newBullets;

    if (m_dashCount < MAX_DASH_COUNT || m_dashState != Idle) return newBullets;

    switch (m_finaleState) {
    case NotStarted: {
        m_finaleState = CircleWarning;
        m_finaleTimer.restart();

        m_opacity = 0.0;
        applyOpacity();
        setPos(-200, -200);

        if (m_warningCircle) {
            qreal centerX = validArea.center().x();
            qreal centerY = validArea.center().y();
            m_warningCircle->activate(centerX, centerY, FINALE_RADIUS);
        }

        break;
    }

    case CircleWarning: {
        if (m_warningCircle) {
            bool stillActive = m_warningCircle->updateState();
            if (!stillActive) {
                m_finaleState = BossAppearing;
                m_finaleTimer.restart();

                qreal centerX = validArea.center().x();
                qreal centerY = validArea.center().y();
                setPos(centerX - pixmap().width() / 2, centerY - pixmap().height() / 2);
                m_opacity = 1.0;
                applyOpacity();

                for (int i = 0; i < FINALE_BULLET_COUNT; ++i) {
                    qreal angle = i * (2.0 * M_PI / FINALE_BULLET_COUNT);
                    Bullet *bullet = Bullet::createBossOrbitingBullet(
                        getCenterX(), getCenterY(), angle, 0, FINALE_BULLET_SIZE
                        );
                    bullet->setFissionParams(FINALE_FISSION_DIST, FINALE_FISSION_COUNT,
                                             FINALE_FISSION_SPEED, FINALE_FISSION_SIZE);
                    bullet->launchTowards(getCenterX() + qCos(angle) * 100,
                                          getCenterY() + qSin(angle) * 100,
                                          FINALE_BULLET_SPEED);
                    newBullets.append(bullet);
                }
            }
        }
        break;
    }

    case BossAppearing: {
        if (m_finaleTimer.elapsed() >= 500) {
            m_finaleState = FinaleDone;
        }
        break;
    }

    case FinaleDone:
        break;
    }

    return newBullets;
}

bool BossPhase2::isFinaleComplete() const
{
    return m_finaleState == FinaleDone;
}


// updateRingShoot: 二阶段Boss环形发射子弹
QList<Bullet*> BossPhase2::updateRingShoot()
{
    QList<Bullet*> newBullets;

    // 只有在技能完成且不在冲刺状态时才发射
    if (!isSkillComplete() || m_dashState != Idle) return newBullets;

    // 检查发射间隔
    if (m_ringShootTimer.elapsed() < RING_SHOOT_INTERVAL) return newBullets;

    // 重置计时器
    m_ringShootTimer.restart();

    qreal centerX = getCenterX();
    qreal centerY = getCenterY();

    // 环形发射普通散射子弹（不裂变，数量多速度慢）
    for (int i = 0; i < RING_BULLET_COUNT; ++i) {
        qreal angle = i * (2.0 * M_PI / RING_BULLET_COUNT);
        Bullet *bullet = Bullet::createBossSpreadBullet(
            centerX - RING_BULLET_SIZE / 2.0,
            centerY - RING_BULLET_SIZE / 2.0,
            angle,
            RING_BULLET_SPEED,
            RING_BULLET_SIZE
            );
        newBullets.append(bullet);
    }

    return newBullets;
}

bool BossPhase2::isSkillComplete() const
{
    return m_dashCount >= MAX_DASH_COUNT && m_dashState == Idle && m_finaleState == FinaleDone;
}

qreal BossPhase2::getCenterX() const
{
    return pos().x() + pixmap().width() / 2;
}

qreal BossPhase2::getCenterY() const
{
    return pos().y() + pixmap().height() / 2;
}

int BossPhase2::getHp() const { return m_hp; }
int BossPhase2::getMaxHp() const { return m_maxHp; }

void BossPhase2::setHp(int hp)
{
    m_hp = qBound(0, hp, m_maxHp);
}

void BossPhase2::takeDamage(int damage)
{
    setHp(m_hp - damage);
}
