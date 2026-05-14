#include "boss.h"
#include "bullet.h"
#include <QtMath>
#include <QRandomGenerator>

Boss::Boss(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    QPixmap pixmap(":/images/images/test_boss.png");
    setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 开场动画初始状态：完全透明，位置在目标位置下方
    setPos(640, 640 + m_introOffsetY);
    setZValue(0);
    QGraphicsItem::setOpacity(m_introOpacity);

    m_orbSkillTimer.start();
    m_homingSkillTimer.start();
    m_spreadTimer.start();
    m_dashSkillTimer.start();

    m_skillCooldownTimer.start();

    m_orbExecTimer.start();
    m_homingExecTimer.start();
    m_spreadExecTimer.start();
}

// setIntroProgress: 设置开场动画进度
void Boss::setIntroProgress(qreal progress)
{
    progress = qBound(0.0, progress, 1.0);

    // 透明度：ease-out缓动
    m_introOpacity = progress * progress;
    QGraphicsItem::setOpacity(m_introOpacity);

    // 垂直偏移：ease-out缓动
    qreal offsetFactor = (1.0 - progress) * (1.0 - progress);
    m_introOffsetY = 100.0 * offsetFactor;

    setPos(640, 640 + m_introOffsetY);
}

// setIntroOpacity: 直接设置透明度
// 用途：死亡演出期间直接控制Boss透明度（闪烁、淡出、淡入）
// 为什么：死亡演出需要频繁修改透明度，不需要同时修改位置
void Boss::setIntroOpacity(qreal opacity)
{
    m_introOpacity = qBound(0.0, opacity, 1.0);
    QGraphicsItem::setOpacity(m_introOpacity);
}

void Boss::setIntroOffset(qreal offsetY)
{
    m_introOffsetY = offsetY;
    setPos(640, 640 + m_introOffsetY);
}

qreal Boss::getIntroOpacity() const
{
    return m_introOpacity;
}

qreal Boss::getIntroOffset() const
{
    return m_introOffsetY;
}

bool Boss::isIntroComplete() const
{
    return m_introComplete;
}

void Boss::finishIntro()
{
    m_introComplete = true;
    m_introOpacity = 1.0;
    m_introOffsetY = 0.0;
    QGraphicsItem::setOpacity(1.0);
    setPos(640, 640);
}

void Boss::moveTowardsPlayer(qreal playerX, qreal playerY, const QRectF &validArea)
{
    if (!m_introComplete) return;
    if (m_isDashing) return;

    qreal bossX = getCenterX();
    qreal bossY = getCenterY();

    qreal dx = playerX - bossX;
    qreal dy = playerY - bossY;
    qreal len = qSqrt(dx * dx + dy * dy);

    if (len < 0.001) return;

    dx = dx / len * MOVE_SPEED;
    dy = dy / len * MOVE_SPEED;

    qreal newX = pos().x() + dx;
    qreal newY = pos().y() + dy;

    qreal halfW = pixmap().width() / 2;
    qreal halfH = pixmap().height() / 2;

    newX = qBound(validArea.left() + halfW, newX, validArea.right() - halfW);
    newY = qBound(validArea.top() + halfH, newY, validArea.bottom() - halfH);

    setPos(newX, newY);
}

bool Boss::shouldSpawnDashSkill() const
{
    if (!m_introComplete) return false;
    return !m_isDashing && m_dashSkillTimer.elapsed() >= DASH_SKILL_INTERVAL;
}

QList<Bullet*> Boss::updateDashState(qreal playerX, qreal playerY, const QRectF &validArea)
{
    QList<Bullet*> newBullets;

    if (!m_isDashing && m_dashSkillTimer.elapsed() >= DASH_SKILL_INTERVAL) {
        m_isDashing = true;
        m_dashPhaseTimer.restart();
        m_dashSkillTimer.restart();
        m_dashMidFired = false;
        m_dashEndFired = false;

        qreal bossX = getCenterX();
        qreal bossY = getCenterY();

        m_dashStartX = bossX;
        m_dashStartY = bossY;

        qreal dx = playerX - bossX;
        qreal dy = playerY - bossY;
        qreal len = qSqrt(dx * dx + dy * dy);

        if (len < 0.001) {
            m_dashDirX = 1;
            m_dashDirY = 0;
        } else {
            m_dashDirX = dx / len;
            m_dashDirY = dy / len;
        }

        newBullets.append(spawnRingBullets(bossX, bossY, DASH_BULLET_COUNT, DASH_BULLET_SPEED, DASH_BULLET_SIZE));
    }

    if (!m_isDashing) return newBullets;

    qreal elapsed = m_dashPhaseTimer.elapsed();
    qreal bossX = getCenterX();
    qreal bossY = getCenterY();

    qreal moveX = m_dashDirX * DASH_SPEED;
    qreal moveY = m_dashDirY * DASH_SPEED;

    qreal newX = pos().x() + moveX;
    qreal newY = pos().y() + moveY;

    qreal halfW = pixmap().width() / 2;
    qreal halfH = pixmap().height() / 2;

    newX = qBound(validArea.left() + halfW, newX, validArea.right() - halfW);
    newY = qBound(validArea.top() + halfH, newY, validArea.bottom() - halfH);

    setPos(newX, newY);

    if (!m_dashMidFired && elapsed >= DASH_DURATION / 2) {
        m_dashMidFired = true;
        qreal midX = getCenterX();
        qreal midY = getCenterY();
        newBullets.append(spawnRingBullets(midX, midY, DASH_BULLET_COUNT, DASH_BULLET_SPEED, DASH_BULLET_SIZE));
    }

    if (elapsed >= DASH_DURATION) {
        m_isDashing = false;
        qreal endX = getCenterX();
        qreal endY = getCenterY();
        newBullets.append(spawnRingBullets(endX, endY, DASH_BULLET_COUNT, DASH_BULLET_SPEED, DASH_BULLET_SIZE));
    }

    return newBullets;
}

bool Boss::isDashing() const
{
    return m_isDashing;
}

QList<Bullet*> Boss::spawnRingBullets(qreal centerX, qreal centerY, int count, int speed, int size)
{
    QList<Bullet*> bullets;

    for (int i = 0; i < count; ++i) {
        qreal angle = i * (2.0 * M_PI / count);
        Bullet *bullet = Bullet::createBossSpreadBullet(
            centerX - size / 2.0,
            centerY - size / 2.0,
            angle,
            speed,
            size
            );
        bullets.append(bullet);
    }

    return bullets;
}

bool Boss::shouldSpawnOrbitSkill() const
{
    if (!m_introComplete) return false;
    if (m_orbTrigger) return true;
    return m_orbSkillTimer.elapsed() >= ORB_SKILL_INTERVAL;
}

QList<Bullet*> Boss::spawnOrbitingBullets()
{
    QList<Bullet*> bullets;
    qreal bossX = getCenterX();
    qreal bossY = getCenterY();

    m_orbTrigger = false;
    m_orbSkillTimer.restart();

    for (int i = 0; i < ORB_BULLET_COUNT; ++i) {
        qreal angle = i * (M_PI / 2.0);
        Bullet *bullet = Bullet::createBossOrbitingBullet(bossX, bossY, angle, ORB_RADIUS, ORB_BULLET_SIZE);
        bullet->setFissionParams(ORB_FISSION_DISTANCE, ORB_FISSION_COUNT, ORB_FISSION_SPEED, ORB_FISSION_SIZE);
        bullets.append(bullet);
    }

    return bullets;
}

bool Boss::shouldSpawnHomingSkill() const
{
    if (!m_introComplete) return false;
    if (m_homingTrigger) return true;
    return m_homingSkillTimer.elapsed() >= HOMING_SKILL_INTERVAL;
}

QList<Bullet*> Boss::spawnHomingBullets(qreal playerX, qreal playerY)
{
    Q_UNUSED(playerX);
    Q_UNUSED(playerY);

    QList<Bullet*> bullets;
    qreal bossX = getCenterX();
    qreal bossY = getCenterY();

    m_homingTrigger = false;
    m_homingSkillTimer.restart();

    for (int i = 0; i < HOMING_BULLET_COUNT; ++i) {
        qreal angle = i * (2.0 * M_PI / HOMING_BULLET_COUNT);
        qreal dx = qCos(angle);
        qreal dy = qSin(angle);

        qreal spawnX = bossX + dx * 60 - HOMING_BULLET_SIZE / 2.0;
        qreal spawnY = bossY + dy * 60 - HOMING_BULLET_SIZE / 2.0;

        Bullet *bullet = Bullet::createBossHomingBullet(spawnX, spawnY, dx, dy, HOMING_BULLET_SPEED, HOMING_BULLET_SIZE);
        bullets.append(bullet);
    }

    return bullets;
}

bool Boss::shouldSpawnSpreadSkill() const
{
    if (!m_introComplete) return false;
    if (m_spreadTrigger) return true;
    return m_spreadTimer.elapsed() >= SPREAD_INTERVAL;
}

Bullet* Boss::getNextSpreadBullet(qreal playerX, qreal playerY)
{
    if (m_spreadTrigger) {
        m_spreadTrigger = false;
    }

    if (!m_spreadActive && m_spreadTimer.elapsed() >= SPREAD_INTERVAL) {
        m_spreadActive = true;
        m_spreadShotCount = 0;
        m_spreadShotTimer.restart();
        m_spreadTimer.restart();
    }

    if (!m_spreadActive) return nullptr;

    if (m_spreadShotCount >= SPREAD_SHOT_COUNT) {
        m_spreadActive = false;
        return nullptr;
    }

    if (m_spreadShotTimer.elapsed() < SPREAD_SHOT_INTERVAL) {
        return nullptr;
    }

    m_spreadShotCount++;
    m_spreadShotTimer.restart();

    qreal bossX = getCenterX();
    qreal bossY = getCenterY();
    qreal dx = playerX - bossX;
    qreal dy = playerY - bossY;
    qreal baseAngle = qAtan2(dy, dx);

    qreal randomOffset = (QRandomGenerator::global()->generateDouble() - 0.5) * SPREAD_ANGLE_RANGE;
    qreal finalAngle = baseAngle + randomOffset;

    qreal spawnX = bossX - SPREAD_BULLET_SIZE / 2.0;
    qreal spawnY = bossY - SPREAD_BULLET_SIZE / 2.0;

    return Bullet::createBossSpreadBullet(spawnX, spawnY, finalAngle, SPREAD_BULLET_SPEED, SPREAD_BULLET_SIZE);
}

bool Boss::isSpreadActive() const
{
    return m_spreadActive;
}

qreal Boss::getCenterX() const
{
    return pos().x() + pixmap().width() / 2;
}

qreal Boss::getCenterY() const
{
    return pos().y() + pixmap().height() / 2;
}

int Boss::getHp() const { return m_hp; }
int Boss::getMaxHp() const { return m_maxHp; }

void Boss::setHp(int hp)
{
    m_hp = qBound(0, hp, m_maxHp);
}

void Boss::takeDamage(int damage)
{
    setHp(m_hp - damage);
}

Boss::SkillState Boss::getCurrentSkillState() const
{
    return m_currentSkill;
}

bool Boss::isAnySkillActive() const
{
    return m_currentSkill != SkillState::Idle;
}

void Boss::updateSkillState()
{
    switch (m_currentSkill) {
    case SkillState::Orbiting:
        if (m_orbExecTimer.elapsed() >= 3000) {
            m_currentSkill = SkillState::Idle;
            m_skillCooldownTimer.restart();
        }
        break;

    case SkillState::Homing:
        if (m_homingExecTimer.elapsed() >= HOMING_LIFE_TIME + 500) {
            m_currentSkill = SkillState::Idle;
            m_skillCooldownTimer.restart();
        }
        break;

    case SkillState::Spread:
        if (!m_spreadActive && m_spreadExecTimer.elapsed() >= 3500) {
            m_currentSkill = SkillState::Idle;
            m_skillCooldownTimer.restart();
        }
        break;

    case SkillState::Idle:
        break;
    }
}

void Boss::pickNextSkill()
{
    if (m_currentSkill != SkillState::Idle) return;
    if (m_skillCooldownTimer.elapsed() < SKILL_COOLDOWN_MS) return;

    int skillIndex = QRandomGenerator::global()->bounded(3);

    switch (skillIndex) {
    case 0:
        m_currentSkill = SkillState::Orbiting;
        m_orbTrigger = true;
        m_orbExecTimer.restart();
        break;
    case 1:
        m_currentSkill = SkillState::Homing;
        m_homingTrigger = true;
        m_homingExecTimer.restart();
        break;
    case 2:
        m_currentSkill = SkillState::Spread;
        m_spreadTrigger = true;
        m_spreadExecTimer.restart();
        break;
    }
}
