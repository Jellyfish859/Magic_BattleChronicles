#include "warningrect.h"
#include <QtMath>
#include <QPen>

WarningRect::WarningRect(QGraphicsItem *parent) : QGraphicsRectItem(parent)
{
    setVisible(false);
    setZValue(7);  // 在Boss下方，子弹上方
}

void WarningRect::activate(qreal startX, qreal startY, qreal angle, qreal length)
{
    m_active = true;
    m_opacity = 0.0;
    m_phase = 0;
    m_timer.restart();

    // 设置矩形：起点在(0,0)，沿x轴正方向延伸
    // 然后通过setPos和setRotation定位
    setRect(0, -WIDTH / 2, length, WIDTH);
    setPos(startX, startY);
    setRotation(qRadiansToDegrees(angle));

    // 浅透明红色
    setBrush(QColor(255, 80, 80, 0));  // 初始完全透明
    setPen(Qt::NoPen);

    setVisible(true);
}

bool WarningRect::updateState()
{
    if (!m_active) return false;

    int elapsed = m_timer.elapsed();

    // 阶段0：淡入
    if (m_phase == 0) {
        if (elapsed < FADE_IN_DURATION) {
            // 淡入：0 -> MAX_OPACITY
            m_opacity = MAX_OPACITY * static_cast<qreal>(elapsed) / FADE_IN_DURATION;
            setBrush(QColor(255, 80, 80, static_cast<int>(m_opacity * 255)));
            return true;
        } else {
            // 淡入完成，进入闪烁阶段
            m_phase = 1;
            m_timer.restart();
            elapsed = 0;
        }
    }

    // 阶段1：快速闪烁
    if (m_phase == 1) {
        if (elapsed < BLINK_DURATION) {
            // 快速切换透明度：高 -> 低 -> 高 -> 低...
            int blinkCount = elapsed / BLINK_INTERVAL;
            if (blinkCount % 2 == 0) {
                // 亮
                m_opacity = MAX_OPACITY;
            } else {
                // 暗
                m_opacity = MAX_OPACITY * 0.3;  // 暗时透明度更低
            }
            setBrush(QColor(255, 80, 80, static_cast<int>(m_opacity * 255)));
            return true;
        } else {
            // 闪烁结束
            m_phase = 2;
        }
    }

    // 阶段2：消失
    m_active = false;
    setVisible(false);
    return false;
}

bool WarningRect::isFadeInComplete() const
{
    return m_phase >= 1;
}
