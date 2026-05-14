#include "warningcircle.h"
#include <QtMath>
#include <QPen>

WarningCircle::WarningCircle(QGraphicsItem *parent) : QGraphicsEllipseItem(parent)
{
    setVisible(false);
    setZValue(7);
}

void WarningCircle::activate(qreal centerX, qreal centerY, qreal radius)
{
    m_active = true;
    m_opacity = 0.0;
    m_phase = 0;
    m_timer.restart();

    // 设置圆形：以圆心为中心
    setRect(-radius, -radius, radius * 2, radius * 2);
    setPos(centerX, centerY);

    // 浅透明红色，无填充，红色边框
    setBrush(Qt::NoBrush);
    setPen(QPen(QColor(255, 60, 60, 0), 4));  // 初始透明边框

    setVisible(true);
}

bool WarningCircle::updateState()
{
    if (!m_active) return false;

    int elapsed = m_timer.elapsed();

    // 阶段0：淡入
    if (m_phase == 0) {
        if (elapsed < FADE_IN_DURATION) {
            m_opacity = MAX_OPACITY * static_cast<qreal>(elapsed) / FADE_IN_DURATION;
            setPen(QPen(QColor(255, 60, 60, static_cast<int>(m_opacity * 255)), 4));
            return true;
        } else {
            m_phase = 1;
            m_timer.restart();
            elapsed = 0;
        }
    }

    // 阶段1：快速闪烁
    if (m_phase == 1) {
        if (elapsed < BLINK_DURATION) {
            int blinkCount = elapsed / BLINK_INTERVAL;
            if (blinkCount % 2 == 0) {
                m_opacity = MAX_OPACITY;
            } else {
                m_opacity = MAX_OPACITY * 0.2;
            }
            setPen(QPen(QColor(255, 60, 60, static_cast<int>(m_opacity * 255)), 4));
            return true;
        } else {
            m_phase = 2;
        }
    }

    // 阶段2：消失
    m_active = false;
    setVisible(false);
    return false;
}
