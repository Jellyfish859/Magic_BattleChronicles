#include "player.h"

Player::Player(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    QPixmap pixmap(":/images/images/test_player.png");
    setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setPos(640, 1000);
    setZValue(0);

    m_shootCooldown.start();
    m_potionTimer.start();
}

void Player::move(int dx, int dy, const QRectF &validArea)
{
    // 计算当前实际移速
    int currentSpeed = getCurrentSpeed();

    qreal newX = pos().x() + dx * currentSpeed;
    qreal newY = pos().y() + dy * currentSpeed;

    qreal halfW = pixmap().width() / 2;
    qreal halfH = pixmap().height() / 2;

    newX = qBound(validArea.left() + halfW, newX, validArea.right() - halfW);
    newY = qBound(validArea.top() + halfH, newY, validArea.bottom() - halfH);

    setPos(newX, newY);

    if (dx < 0) {
        flip(false);
    } else if (dx > 0) {
        flip(true);
    }

    if (dx != 0 || dy != 0) {
        m_lastDx = dx;
        m_lastDy = dy;
    }

    // 无敌状态视觉反馈：闪烁效果
    if (m_isInvincible) {
        int phase = (m_invincibleTimer.elapsed() / 100) % 2;
        QGraphicsItem::setOpacity(phase == 0 ? 0.5 : 0.8);
    } else {
        QGraphicsItem::setOpacity(1.0);
    }
}

// 新增：获取当前移速（综合考虑血瓶和锁定状态）
int Player::getCurrentSpeed() const
{
    int currentSpeed = m_speed;

    // 血瓶状态：移速减半
    if (m_isUsingPotion) {
        currentSpeed = currentSpeed / 2;
    }

    // 锁定状态：移速降低
    if (m_isLocked) {
        currentSpeed = static_cast<int>(currentSpeed * LOCK_SPEED_RATIO);
    }

    // 确保至少为1
    return qMax(1, currentSpeed);
}

bool Player::canShoot() const
{
    return m_shootCooldown.elapsed() >= SHOOT_COOLDOWN_MS;
}

void Player::onShoot()
{
    m_shootCooldown.restart();
}

qreal Player::getShootX() const
{
    return getCenterX();
}

qreal Player::getShootY() const
{
    return getCenterY();
}

bool Player::isLocked() const
{
    return m_isLocked;
}

// 新增：设置锁定状态
void Player::setLock(bool locked)
{
    m_isLocked = locked;
    if (m_isLocked) {
        // 锁定开始时更新目标
        // 但目标位置由外部调用 setLockTarget 设置
    }
}

void Player::setLockTarget(qreal x, qreal y)
{
    m_lockTargetX = x;
    m_lockTargetY = y;
}

qreal Player::getLockTargetX() const
{
    return m_lockTargetX;
}

qreal Player::getLockTargetY() const
{
    return m_lockTargetY;
}

qreal Player::getShootDirX() const
{
    if (m_isLocked) {
        qreal dx = m_lockTargetX - getCenterX();
        qreal dy = m_lockTargetY - getCenterY();
        qreal len = qSqrt(dx * dx + dy * dy);
        if (len < 0.001) return m_lastDx;
        return dx / len;
    }
    qreal len = qSqrt(m_lastDx * m_lastDx + m_lastDy * m_lastDy);
    if (len < 0.001) return m_facingRight ? 1 : -1;
    return m_lastDx / len;
}

qreal Player::getShootDirY() const
{
    if (m_isLocked) {
        qreal dx = m_lockTargetX - getCenterX();
        qreal dy = m_lockTargetY - getCenterY();
        qreal len = qSqrt(dx * dx + dy * dy);
        if (len < 0.001) return 0;
        return dy / len;
    }
    qreal len = qSqrt(m_lastDx * m_lastDx + m_lastDy * m_lastDy);
    if (len < 0.001) return 0;
    return m_lastDy / len;
}

bool Player::usePotion()
{
    if (m_potionCount <= 0) return false;
    if (m_isUsingPotion) return false;
    if (m_hp >= m_maxHp) return false;

    m_isUsingPotion = true;
    m_potionTimer.restart();
    m_potionCount--;
    return true;
}

bool Player::isUsingPotion() const
{
    return m_isUsingPotion;
}

int Player::getPotionCount() const
{
    return m_potionCount;
}

int Player::getMaxPotions() const
{
    return MAX_POTIONS;
}

void Player::updatePotionState()
{
    if (!m_isUsingPotion) return;
    if (m_potionTimer.elapsed() >= POTION_DURATION_MS) {
        m_isUsingPotion = false;
        int newHp = m_hp + POTION_HEAL_AMOUNT;
        setHp(qMin(newHp, m_maxHp));
    }
}

qreal Player::getCenterX() const
{
    return pos().x() + pixmap().width() / 2;
}

qreal Player::getCenterY() const
{
    return pos().y() + pixmap().height() / 2;
}

bool Player::isFacingRight() const
{
    return m_facingRight;
}

void Player::flip(bool facingRight)
{
    if (m_facingRight == facingRight) return;
    m_facingRight = facingRight;

    QTransform transform;
    transform.scale(facingRight ? 1 : -1, 1);
    setTransform(transform);
}

// isInvincible: 查询玩家是否处于无敌状态
// 用途：GameScene 的碰撞检测中判断是否可以受伤
bool Player::isInvincible() const
{
    return m_isInvincible;
}

// updateInvincibleState: 更新无敌状态
// 用途：每帧检查无敌时间是否结束，结束后恢复正常状态
// 为什么：无敌是有时限的，需要定期更新状态
void Player::updateInvincibleState()
{
    if (!m_isInvincible) return;
    if (m_invincibleTimer.elapsed() >= INVINCIBLE_DURATION_MS) {
        m_isInvincible = false;
    }
}

int Player::getHp() const { return m_hp; }
int Player::getMaxHp() const { return m_maxHp; }

void Player::setHp(int hp)
{
    m_hp = qBound(0, hp, m_maxHp);
}

void Player::takeDamage(int damage)
{
    // 无敌状态下不受伤害
    if (m_isInvincible) return;

    setHp(m_hp - damage);

    // 触发无敌状态
    m_isInvincible = true;
    m_invincibleTimer.restart();
}
