#ifndef BOSSCLONE_H
#define BOSSCLONE_H

#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QList>

class Bullet;

// BossClone: Boss分身
// 用途：二阶段Boss放完终结技后，在地图四个角落出现的固定分身
// 特性：不会移动、无法被攻击、以固定频率向周围环形发射弹幕
class BossClone : public QGraphicsPixmapItem
{
public:
    explicit BossClone(qreal x, qreal y, QGraphicsItem *parent = nullptr);

    // 更新分身状态，返回新发射的子弹
    // 参数 validArea: 有效区域（用于边界检测，分身本身不移动）
    QList<Bullet*> updateShoot();

    qreal getCenterX() const;
    qreal getCenterY() const;

private:
    // 环形发射计时器
    QElapsedTimer m_shootTimer;

    // 分身弹幕参数
    static constexpr int CLONE_SHOOT_INTERVAL = 3500;  // 每3.5秒发射一次（比本体稍慢）
    static constexpr int CLONE_BULLET_COUNT = 20;          // 每次20颗子弹（更密集的弹幕）
    static constexpr int CLONE_BULLET_SPEED = 2;       // 子弹速度更慢（与本体一致，增加压迫感）
    static constexpr int CLONE_BULLET_SIZE = 18;       // 子弹尺寸增大（更容易被看到和躲避）
};

#endif // BOSSCLONE_H
