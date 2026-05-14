#ifndef WARNINGRECT_H
#define WARNINGRECT_H

#include <QGraphicsRectItem>
#include <QElapsedTimer>

class WarningRect : public QGraphicsRectItem
{
public:
    explicit WarningRect(QGraphicsItem *parent = nullptr);

    void activate(qreal startX, qreal startY, qreal angle, qreal length);

    // 更新闪烁状态
    // 返回：是否还在显示中（false表示预警结束，可以删除）
    bool updateState();

    // 是否已完成淡入阶段
    bool isFadeInComplete() const;

private:
    bool m_active = false;
    qreal m_opacity = 0.0;
    QElapsedTimer m_timer;

    // 阶段：0=淡入, 1=闪烁, 2=结束
    int m_phase = 0;

    static constexpr int FADE_IN_DURATION = 400;      // 淡入时间：400ms
    static constexpr int BLINK_DURATION = 800;        // 闪烁总时间：800ms
    static constexpr int BLINK_INTERVAL = 80;         // 闪烁切换间隔：80ms
    static constexpr qreal WIDTH = 50;                // 预警矩形宽度
    static constexpr qreal MAX_OPACITY = 0.4;           // 最大透明度：40%
};

#endif // WARNINGRECT_H
