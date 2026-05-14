#ifndef WARNINGCIRCLE_H
#define WARNINGCIRCLE_H

#include <QGraphicsEllipseItem>
#include <QElapsedTimer>

// WarningCircle: 圆形预警区域
// 用途：二阶段终结技能，中央出现大圆形预警，先淡入后闪烁
class WarningCircle : public QGraphicsEllipseItem
{
public:
    explicit WarningCircle(QGraphicsItem *parent = nullptr);

    // 激活圆形预警
    // 参数 centerX, centerY: 圆心位置
    // 参数 radius: 预警半径
    void activate(qreal centerX, qreal centerY, qreal radius);

    // 更新闪烁状态
    // 返回：是否还在显示中
    bool updateState();

private:
    bool m_active = false;
    qreal m_opacity = 0.0;
    QElapsedTimer m_timer;
    int m_phase = 0;  // 0=淡入, 1=闪烁, 2=结束

    static constexpr int FADE_IN_DURATION = 600;    // 淡入时间：600ms
    static constexpr int BLINK_DURATION = 1000;       // 闪烁总时间：1000ms
    static constexpr int BLINK_INTERVAL = 100;      // 闪烁切换间隔：100ms
    static constexpr qreal MAX_OPACITY = 0.5;       // 最大透明度：50%
};

#endif // WARNINGCIRCLE_H
