#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include <QWidget>
#include <QPaintEvent>
#include <QSet>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>

#include "gamescene.h"

class QScrollBar;

class HpBarWidget : public QWidget
{
public:
    explicit HpBarWidget(QWidget *parent = nullptr);
    void setHp(int hp, int maxHp);
    void setColors(QColor fg, QColor bg);
    void startFadeIn(int durationMs);
    void updateFadeIn();
    bool isFadeInComplete() const;
    void setOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_hp = 0;
    int m_maxHp = 100;
    QColor m_fgColor = Qt::green;
    QColor m_bgColor = Qt::red;
    qreal m_opacity = 1.0;
    bool m_isFadingIn = false;
    int m_fadeInDuration = 0;
    QElapsedTimer m_fadeTimer;
};

// 死亡演出覆盖层（新增）
// 用途：在GameView上方绘制玩家死亡演出效果
// 为什么：使用独立Widget比drawForeground更可靠，不受坐标变换影响
class DeathOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit DeathOverlay(QWidget *parent = nullptr);

    void startSequence();
    void reset();

    void setPlayerOpacity(qreal opacity);
    void setImageOpacity(qreal opacity);
    void setBlackOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal m_playerOpacity = 1.0;
    qreal m_imageOpacity = 0.0;
    qreal m_blackOpacity = 0.0;
    bool m_isActive = false;
    QPixmap m_deathImage;
};

class VictoryOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit VictoryOverlay(QWidget *parent = nullptr);
    void startSequence();
    void reset();
    void setImageOpacity(qreal opacity);
    void setBlackOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal m_imageOpacity = 0.0;
    qreal m_blackOpacity = 0.0;
    bool m_isActive = false;
    QPixmap m_victoryImage;
};

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~GameView();

    HpBarWidget* playerHpBar() const;
    HpBarWidget* bossHpBar() const;

    void startIntro();
    void updateIntroCamera();
    bool isIntroActive() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void gameLoop();

    void onIntroStageChanged(int stage);
    void onIntroBossAppearing();
    void onIntroBossHpBarShow();

    void onBossDeathStageChanged(int stage);
    void onBossDeathCameraMove(qreal targetX, qreal targetY, int durationMs);
    void onBossDeathFadeOut();
    void onBossDeathFadeIn();
    void onBossDeathFadeToBlack();
    void onBossDeathCameraToPlayer();
    void onBossDeathComplete();

    void onBossPhase2DeathStageChanged(int stage);
    void onBossPhase2DeathCameraMove(qreal targetX, qreal targetY, int durationMs);
    void onBossPhase2DeathFadeOut();
    void onBossPhase2DeathCleanup();
    void onBossPhase2DeathComplete();

    // 玩家死亡演出槽函数（修正后）
    void onPlayerDeathStageChanged(int stage);
    void onPlayerDeathPlayerFadeOut(qreal progress);
    void onPlayerDeathImageFadeIn(qreal progress);
    void onPlayerDeathScreenFadeToBlack(qreal progress);
    void onPlayerDeathComplete();

    // 胜利演出槽函数（新增）
    void onVictoryImageFadeIn(qreal progress);
    void onVictoryScreenFadeToBlack(qreal progress);
    void onVictoryComplete();

private:
    void updateDeathCamera();

    HpBarWidget *m_playerHpBar;
    HpBarWidget *m_bossHpBar;
    DeathOverlay *m_deathOverlay;
    VictoryOverlay *m_victoryOverlay;  // 胜利演出覆盖层（新增）

    QSet<int> m_pressedKeys;
    QTimer *m_gameTimer;

    bool m_isIntroActive = false;
    QPointF m_introCameraTarget;
    QPointF m_introCameraStart;
    qreal m_introCameraProgress = 0.0;
    int m_introCameraDuration = 0;
    QElapsedTimer m_introCameraTimer;

    static constexpr qreal SCENE_CENTER_X = 640.0;
    static constexpr qreal SCENE_CENTER_Y = 640.0;

    bool m_isDeathCameraMoving = false;
    QPointF m_deathCameraTarget;
    QPointF m_deathCameraStart;
    int m_deathCameraDuration = 0;
    QElapsedTimer m_deathCameraTimer;

    qreal m_fadeToBlackOpacity = 0.0;
    bool m_isFadingToBlack = false;
};

#endif // GAMEVIEW_H
