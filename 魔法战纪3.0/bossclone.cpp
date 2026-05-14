#include "bossclone.h"
#include "bullet.h"
#include <QtMath>

// BossClone: Boss分身实现

BossClone::BossClone(qreal x, qreal y, QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    // 加载Boss图片但使用半透明效果以区分本体
    QPixmap pixmap(":/images/images/test_boss.png");
    setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setPos(x - 40, y - 40);  // 居中放置
    setZValue(-1);  // 在Boss本体下方

    // 设置半透明效果，让玩家能区分分身和本体
    QGraphicsItem::setOpacity(0.6);

    // 禁用选择和移动，确保无法被攻击和交互
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);

    // 初始化发射计时器
    m_shootTimer.start();
}

// updateShoot: 更新分身发射状态
QList<Bullet*> BossClone::updateShoot()
{
    QList<Bullet*> newBullets;

    // 检查发射间隔
    if (m_shootTimer.elapsed() < CLONE_SHOOT_INTERVAL) return newBullets;

    // 重置计时器
    m_shootTimer.restart();

    qreal centerX = getCenterX();
    qreal centerY = getCenterY();

    // 环形发射普通散射子弹
    for (int i = 0; i < CLONE_BULLET_COUNT; ++i) {
        qreal angle = i * (2.0 * M_PI / CLONE_BULLET_COUNT);
        Bullet *bullet = Bullet::createBossSpreadBullet(
            centerX - CLONE_BULLET_SIZE / 2.0,
            centerY - CLONE_BULLET_SIZE / 2.0,
            angle,
            CLONE_BULLET_SPEED,
            CLONE_BULLET_SIZE
            );
        newBullets.append(bullet);
    }

    return newBullets;
}

qreal BossClone::getCenterX() const
{
    return pos().x() + pixmap().width() / 2;
}

qreal BossClone::getCenterY() const
{
    return pos().y() + pixmap().height() / 2;
}
