#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QRectF>

class Player : public QGraphicsPixmapItem
{
public:
    explicit Player(QGraphicsItem *parent = nullptr);

    // 移动（必须传3个参数）
    void move(int dx, int dy, const QRectF &validArea);

    bool isFacingRight() const;

    bool canShoot() const;
    void onShoot();

    qreal getShootX() const;
    qreal getShootY() const;

    // 锁定相关（修改）
    bool isLocked() const;
    void setLock(bool locked);           // 新增：设置锁定状态
    void setLockTarget(qreal x, qreal y);
    qreal getLockTargetX() const;
    qreal getLockTargetY() const;

    qreal getShootDirX() const;
    qreal getShootDirY() const;

    bool usePotion();
    bool isUsingPotion() const;
    int getPotionCount() const;
    int getMaxPotions() const;
    void updatePotionState();

    qreal getCenterX() const;
    qreal getCenterY() const;

    int getHp() const;
    int getMaxHp() const;
    void setHp(int hp);
    void takeDamage(int damage);

    // 无敌系统公共接口（新增）
    // 用途：外部查询玩家是否处于无敌状态，用于碰撞检测时跳过
    bool isInvincible() const;
    void updateInvincibleState();

    // 新增：获取当前移速（考虑锁定和血瓶状态）
    int getCurrentSpeed() const;

private:
    int m_speed = 5;
    int m_hp = 100;
    int m_maxHp = 100;
    bool m_facingRight = true;

    int m_lastDx = 1;
    int m_lastDy = 0;

    QElapsedTimer m_shootCooldown;
    static constexpr int SHOOT_COOLDOWN_MS = 200;

    bool m_isLocked = false;
    qreal m_lockTargetX = 0;
    qreal m_lockTargetY = 0;

    int m_potionCount = 5;
    static constexpr int MAX_POTIONS = 5;
    bool m_isUsingPotion = false;
    QElapsedTimer m_potionTimer;
    static constexpr int POTION_DURATION_MS = 1500;
    static constexpr int POTION_HEAL_AMOUNT = 50;

    // 新增：锁定时的移速倍率
    static constexpr qreal LOCK_SPEED_RATIO = 0.6;  // 锁定移速为正常的60%

    // 受伤无敌系统（新增）
    // 用途：玩家受伤后半秒内无敌，防止连续受击秒杀
    // 为什么：弹幕密集时玩家可能瞬间受到多次伤害，无敌时间提供反应和躲避空间
    bool m_isInvincible = false;
    QElapsedTimer m_invincibleTimer;
    static constexpr int INVINCIBLE_DURATION_MS = 500;  // 无敌持续时间：500ms（半秒）

    void flip(bool facingRight);
};

#endif // PLAYER_H
