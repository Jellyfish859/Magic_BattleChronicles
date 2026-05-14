#include "gameview.h"
#include "gamescene.h"
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QScrollBar>
#include "player.h"
#include "boss.h"
#include "bossphase2.h"

// ========== HpBarWidget ==========

HpBarWidget::HpBarWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
}

void HpBarWidget::setHp(int hp, int maxHp)
{
    m_hp = hp;
    m_maxHp = maxHp;
    update();
}

void HpBarWidget::setColors(QColor fg, QColor bg)
{
    m_fgColor = fg;
    m_bgColor = bg;
    update();
}

void HpBarWidget::startFadeIn(int durationMs)
{
    m_isFadingIn = true;
    m_fadeInDuration = durationMs;
    m_opacity = 0.0;
    m_fadeTimer.restart();
    setVisible(true);
    update();
}

void HpBarWidget::updateFadeIn()
{
    if (!m_isFadingIn) return;
    int elapsed = m_fadeTimer.elapsed();
    if (elapsed >= m_fadeInDuration) {
        m_opacity = 1.0;
        m_isFadingIn = false;
    } else {
        m_opacity = static_cast<qreal>(elapsed) / m_fadeInDuration;
    }
    update();
}

bool HpBarWidget::isFadeInComplete() const
{
    return !m_isFadingIn && m_opacity >= 1.0;
}

void HpBarWidget::setOpacity(qreal opacity)
{
    m_opacity = qBound(0.0, opacity, 1.0);
    update();
}

void HpBarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (m_opacity <= 0.0) return;
    QPainter painter(this);
    painter.setOpacity(m_opacity);
    painter.fillRect(rect(), m_bgColor);
    double ratio = static_cast<double>(m_hp) / m_maxHp;
    int fgWidth = static_cast<int>(width() * ratio);
    if (fgWidth > 0) {
        painter.fillRect(0, 0, fgWidth, height(), m_fgColor);
    }
}

// ========== DeathOverlay ==========

DeathOverlay::DeathOverlay(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_NoSystemBackground);
    setVisible(false);
    setFocusPolicy(Qt::NoFocus);
}

void DeathOverlay::startSequence()
{
    m_isActive = true;
    setVisible(true);
    raise();  // 置顶
}

void DeathOverlay::reset()
{
    m_isActive = false;
    m_playerOpacity = 1.0;
    m_imageOpacity = 0.0;
    m_blackOpacity = 0.0;
    setVisible(false);
}

void DeathOverlay::setPlayerOpacity(qreal opacity)
{
    m_playerOpacity = opacity;
}

void DeathOverlay::setImageOpacity(qreal opacity)
{
    m_imageOpacity = opacity;
    update();
}

void DeathOverlay::setBlackOpacity(qreal opacity)
{
    m_blackOpacity = opacity;
    update();
}

// ========== VictoryOverlay ==========

VictoryOverlay::VictoryOverlay(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_NoSystemBackground);
    setVisible(false);
    setFocusPolicy(Qt::NoFocus);
}

void VictoryOverlay::startSequence()
{
    m_isActive = true;
    setVisible(true);
    raise();
}

void VictoryOverlay::reset()
{
    m_isActive = false;
    m_imageOpacity = 0.0;
    m_blackOpacity = 0.0;
    setVisible(false);
}

void VictoryOverlay::setImageOpacity(qreal opacity)
{
    m_imageOpacity = opacity;
    update();
}

void VictoryOverlay::setBlackOpacity(qreal opacity)
{
    m_blackOpacity = opacity;
    update();
}

void VictoryOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!m_isActive) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect r = rect();

    // 绘制 "VICTORY" 图片（淡入）
    if (m_imageOpacity > 0.0) {
        painter.setOpacity(m_imageOpacity);
        if (m_victoryImage.isNull()) {
            m_victoryImage = QPixmap(400, 100);
            m_victoryImage.fill(Qt::transparent);
            QPainter imgPainter(&m_victoryImage);
            imgPainter.setRenderHint(QPainter::Antialiasing);
            QFont font("Arial", 48, QFont::Bold);
            imgPainter.setFont(font);
            imgPainter.setPen(QColor(255, 215, 0));  // 金色
            imgPainter.drawText(m_victoryImage.rect(), Qt::AlignCenter, "VICTORY");
            imgPainter.end();
        }
        int imgX = (r.width() - m_victoryImage.width()) / 2;
        int imgY = (r.height() - m_victoryImage.height()) / 2;
        painter.drawPixmap(imgX, imgY, m_victoryImage);
    }

    // 全屏黑屏
    if (m_blackOpacity > 0.0) {
        painter.setOpacity(m_blackOpacity);
        painter.fillRect(r, Qt::black);
    }
}

// ========== DeathOverlay ==========

void DeathOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!m_isActive) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect r = rect();

    // 阶段2-3：绘制 "YOU DIED" 图片（淡入）
    if (m_imageOpacity > 0.0) {
        painter.setOpacity(m_imageOpacity);

        // 懒加载死亡图片
        if (m_deathImage.isNull()) {
            m_deathImage = QPixmap(400, 100);
            m_deathImage.fill(Qt::transparent);
            QPainter imgPainter(&m_deathImage);
            imgPainter.setRenderHint(QPainter::Antialiasing);
            QFont font("Arial", 48, QFont::Bold);
            imgPainter.setFont(font);
            imgPainter.setPen(QColor(180, 20, 20));
            imgPainter.drawText(m_deathImage.rect(), Qt::AlignCenter, "YOU DIED");
            imgPainter.end();
        }

        int imgX = (r.width() - m_deathImage.width()) / 2;
        int imgY = (r.height() - m_deathImage.height()) / 2;
        painter.drawPixmap(imgX, imgY, m_deathImage);
    }

    // 阶段4：全屏黑屏（覆盖在最上层）
    if (m_blackOpacity > 0.0) {
        painter.setOpacity(m_blackOpacity);
        painter.fillRect(r, Qt::black);
    }
}

// ========== GameView ==========

GameView::GameView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);
    setRenderHint(QPainter::Antialiasing);

    m_playerHpBar = new HpBarWidget(this);
    m_playerHpBar->setColors(Qt::green, Qt::red);
    m_playerHpBar->setFixedSize(250, 10);
    m_playerHpBar->move(20, 20);
    m_playerHpBar->setOpacity(1.0);

    m_bossHpBar = new HpBarWidget(this);
    m_bossHpBar->setColors(Qt::yellow, Qt::red);
    m_bossHpBar->setFixedSize(400, 12);
    m_bossHpBar->setOpacity(0.0);
    m_bossHpBar->setVisible(false);

    // 死亡演出覆盖层（新增）
    // 用途：在GameView上方绘制死亡演出效果
    // 为什么：独立Widget不受场景坐标变换影响，更可靠
    m_deathOverlay = new DeathOverlay(this);
    m_victoryOverlay = new VictoryOverlay(this);
    m_victoryOverlay->setGeometry(0, 0, width(), height());
    m_victoryOverlay->setVisible(false);
    m_victoryOverlay->raise();
    m_deathOverlay->setGeometry(0, 0, width(), height());
    m_deathOverlay->setVisible(false);
    m_deathOverlay->raise();

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameView::gameLoop);
    m_gameTimer->start(16);
}

GameView::~GameView()
{
}

HpBarWidget* GameView::playerHpBar() const { return m_playerHpBar; }
HpBarWidget* GameView::bossHpBar() const { return m_bossHpBar; }

void GameView::startIntro()
{
    m_isIntroActive = true;
    GameScene *gameScene = qobject_cast<GameScene*>(scene());
    if (gameScene) {
        connect(gameScene, &GameScene::introStageChanged, this, &GameView::onIntroStageChanged);
        connect(gameScene, &GameScene::introBossAppearing, this, &GameView::onIntroBossAppearing);
        connect(gameScene, &GameScene::introBossHpBarShow, this, &GameView::onIntroBossHpBarShow);
        gameScene->startIntro();
    }
    Player *player = gameScene ? gameScene->getPlayer() : nullptr;
    if (player) {
        m_introCameraStart = QPointF(player->getCenterX(), player->getCenterY());
    } else {
        m_introCameraStart = QPointF(SCENE_CENTER_X, SCENE_CENTER_Y);
    }
    m_introCameraTarget = QPointF(SCENE_CENTER_X, SCENE_CENTER_Y);
    m_introCameraDuration = 1500;
    m_introCameraProgress = 0.0;
    m_introCameraTimer.restart();
}

void GameView::updateIntroCamera()
{
    if (!m_isIntroActive) return;
    qreal elapsed = m_introCameraTimer.elapsed();
    qreal progress = qBound(0.0, elapsed / m_introCameraDuration, 1.0);
    qreal smoothProgress = progress * progress * (3.0 - 2.0 * progress);
    qreal currentX = m_introCameraStart.x() + (m_introCameraTarget.x() - m_introCameraStart.x()) * smoothProgress;
    qreal currentY = m_introCameraStart.y() + (m_introCameraTarget.y() - m_introCameraStart.y()) * smoothProgress;
    centerOn(currentX, currentY);
}

bool GameView::isIntroActive() const
{
    return m_isIntroActive;
}

void GameView::onIntroStageChanged(int stage)
{
    GameScene *gameScene = qobject_cast<GameScene*>(scene());
    if (!gameScene) return;
    switch (stage) {
    case 4: {
        Player *player = gameScene->getPlayer();
        if (player) {
            m_introCameraStart = QPointF(SCENE_CENTER_X, SCENE_CENTER_Y);
            m_introCameraTarget = QPointF(player->getCenterX(), player->getCenterY());
            m_introCameraDuration = 1500;
            m_introCameraProgress = 0.0;
            m_introCameraTimer.restart();
        }
        break;
    }
    case 0:
        m_isIntroActive = false;
        disconnect(gameScene, &GameScene::introStageChanged, this, &GameView::onIntroStageChanged);
        disconnect(gameScene, &GameScene::introBossAppearing, this, &GameView::onIntroBossAppearing);
        disconnect(gameScene, &GameScene::introBossHpBarShow, this, &GameView::onIntroBossHpBarShow);
        break;
    default:
        break;
    }
}

void GameView::onIntroBossAppearing()
{
    m_bossHpBar->setVisible(true);
    m_bossHpBar->setOpacity(0.0);
}

void GameView::onIntroBossHpBarShow()
{
    m_bossHpBar->startFadeIn(800);
}

// ========== 一阶段死亡演出 ==========

void GameView::onBossDeathStageChanged(int stage) { Q_UNUSED(stage) }

void GameView::onBossDeathCameraMove(qreal targetX, qreal targetY, int durationMs)
{
    m_isDeathCameraMoving = true;
    qreal currentCenterX = horizontalScrollBar()->value() + viewport()->width() / 2.0;
    qreal currentCenterY = verticalScrollBar()->value() + viewport()->height() / 2.0;
    m_deathCameraStart = QPointF(currentCenterX, currentCenterY);
    m_deathCameraTarget = QPointF(targetX, targetY);
    m_deathCameraDuration = durationMs;
    m_deathCameraTimer.restart();
}

void GameView::onBossDeathFadeOut() {}
void GameView::onBossDeathFadeIn() {}

void GameView::onBossDeathFadeToBlack()
{
    m_isFadingToBlack = true;
    m_fadeToBlackOpacity = 0.0;
}

void GameView::onBossDeathCameraToPlayer()
{
    m_isFadingToBlack = false;
    m_fadeToBlackOpacity = 0.0;
    GameScene *gameScene = qobject_cast<GameScene*>(scene());
    if (!gameScene) return;
    Player *player = gameScene->getPlayer();
    if (player) {
        m_isDeathCameraMoving = true;
        qreal currentCenterX = horizontalScrollBar()->value() + viewport()->width() / 2.0;
        qreal currentCenterY = verticalScrollBar()->value() + viewport()->height() / 2.0;
        m_deathCameraStart = QPointF(currentCenterX, currentCenterY);
        m_deathCameraTarget = QPointF(player->getCenterX(), player->getCenterY());
        m_deathCameraDuration = 1500;
        m_deathCameraTimer.restart();
    }
}

void GameView::onBossDeathComplete()
{
    m_isDeathCameraMoving = false;
    m_isFadingToBlack = false;
    m_fadeToBlackOpacity = 0.0;
    GameScene *gameScene = qobject_cast<GameScene*>(scene());
    if (gameScene && gameScene->getBossPhase2()) {
        m_bossHpBar->setHp(gameScene->getBossPhase2()->getHp(), gameScene->getBossPhase2()->getMaxHp());
        m_bossHpBar->startFadeIn(800);
    }
}

// ========== 二阶段死亡演出 ==========

void GameView::onBossPhase2DeathStageChanged(int stage) { Q_UNUSED(stage) }

void GameView::onBossPhase2DeathCameraMove(qreal targetX, qreal targetY, int durationMs)
{
    m_isDeathCameraMoving = true;
    qreal currentCenterX = horizontalScrollBar()->value() + viewport()->width() / 2.0;
    qreal currentCenterY = verticalScrollBar()->value() + viewport()->height() / 2.0;
    m_deathCameraStart = QPointF(currentCenterX, currentCenterY);
    m_deathCameraTarget = QPointF(targetX, targetY);
    m_deathCameraDuration = durationMs;
    m_deathCameraTimer.restart();
}

void GameView::onBossPhase2DeathFadeOut() {}

void GameView::onBossPhase2DeathCleanup()
{
    m_bossHpBar->setOpacity(0.0);
    m_bossHpBar->setVisible(false);
}

void GameView::onBossPhase2DeathComplete()
{
    m_isDeathCameraMoving = false;
}

// ========== 玩家死亡演出（修正后）==========

void GameView::onPlayerDeathStageChanged(int stage)
{
    Q_UNUSED(stage)
    // 激活死亡演出覆盖层
    if (m_deathOverlay) {
        m_deathOverlay->startSequence();
    }
}

void GameView::onPlayerDeathPlayerFadeOut(qreal progress)
{
    GameScene *gameScene = qobject_cast<GameScene*>(scene());
    if (gameScene && gameScene->getPlayer()) {
        gameScene->getPlayer()->setOpacity(1.0 - progress);
    }
}

void GameView::onPlayerDeathImageFadeIn(qreal progress)
{
    if (m_deathOverlay) {
        m_deathOverlay->setImageOpacity(progress);
    }
}

void GameView::onPlayerDeathScreenFadeToBlack(qreal progress)
{
    if (m_deathOverlay) {
        m_deathOverlay->setBlackOpacity(progress);
    }
}

// 胜利演出槽函数（新增）
void GameView::onVictoryImageFadeIn(qreal progress)
{
    if (m_victoryOverlay) {
        // 第一次调用时激活覆盖层
        if (!m_victoryOverlay->isVisible() && progress > 0) {
            m_victoryOverlay->startSequence();
        }
        m_victoryOverlay->setImageOpacity(progress);
    }
}

void GameView::onVictoryScreenFadeToBlack(qreal progress)
{
    if (m_victoryOverlay) {
        m_victoryOverlay->setBlackOpacity(progress);
    }
}

void GameView::onVictoryComplete()
{
    if (m_victoryOverlay) {
        m_victoryOverlay->reset();
    }
}

void GameView::onPlayerDeathComplete()
{
    if (m_deathOverlay) {
        m_deathOverlay->reset();
    }
}

void GameView::updateDeathCamera()
{
    if (!m_isDeathCameraMoving) return;
    qreal elapsed = m_deathCameraTimer.elapsed();
    qreal progress = qBound(0.0, elapsed / m_deathCameraDuration, 1.0);
    qreal smoothProgress = progress * progress * (3.0 - 2.0 * progress);
    qreal currentX = m_deathCameraStart.x() + (m_deathCameraTarget.x() - m_deathCameraStart.x()) * smoothProgress;
    qreal currentY = m_deathCameraStart.y() + (m_deathCameraTarget.y() - m_deathCameraStart.y()) * smoothProgress;
    centerOn(currentX, currentY);
    if (progress >= 1.0) {
        m_isDeathCameraMoving = false;
    }
}

void GameView::gameLoop()
{
    GameScene *gameScene = qobject_cast<GameScene*>(scene());
    if (!gameScene) return;

    Player *player = gameScene->getPlayer();
    if (!player) return;

    // 开场动画
    if (gameScene->isIntro()) {
        gameScene->updateIntro();
        updateIntroCamera();
        if (m_bossHpBar->isVisible()) {
            m_bossHpBar->updateFadeIn();
        }
        return;
    }

    // 一阶段死亡演出
    if (gameScene->isPhaseTransition()) {
        gameScene->updateBossDeathSequence();
        updateDeathCamera();
        if (m_isFadingToBlack) {
            GameScene::BossDeathState state = gameScene->getBossDeathState();
            if (state == GameScene::BossDeathState::FadeToBlack) {
                m_fadeToBlackOpacity = gameScene->getBossDeathProgress();
            }
        }
        return;
    }

    // 二阶段死亡演出
    if (gameScene->isPhase2Transition()) {
        gameScene->updatePhase2DeathSequence();
        updateDeathCamera();
        return;
    }

    // 玩家死亡演出（修正后）
    if (gameScene->isPlayerDying()) {
        gameScene->updatePlayerDeathSequence();
        if (player) {
            centerOn(player);
        }
        return;
    }

    // 胜利演出（新增）
    if (gameScene->getGameState() == GameScene::GameState::Victory) {
        gameScene->updateVictorySequence();
        return;
    }

    // 正常游戏循环
    player->updatePotionState();
    player->updateInvincibleState();

    gameScene->checkPhaseTransition();
    gameScene->checkPlayerDeath();
    gameScene->updateBossSkillState();
    gameScene->updateBossDash();
    gameScene->updateBossSkillState();
    gameScene->updateBossOrbitingBullets();
    gameScene->updateBossHomingBullets();
    gameScene->updateBossSpread();
    gameScene->updateFissionBullets();
    gameScene->updateBossPhase2();
    gameScene->updateBossPhase2Finale();
    gameScene->updateBossClones();
    gameScene->updateLockTarget();
    gameScene->updateBullets();
    gameScene->checkBulletCollisions();
    gameScene->cleanupBullets();

    m_playerHpBar->setHp(player->getHp(), player->getMaxHp());

    if (gameScene->getBossPhase2()) {
        m_bossHpBar->setHp(gameScene->getBossPhase2()->getHp(), gameScene->getBossPhase2()->getMaxHp());
    } else if (gameScene->getBoss()) {
        m_bossHpBar->setHp(gameScene->getBoss()->getHp(), gameScene->getBoss()->getMaxHp());
    }

    int dx = 0;
    int dy = 0;
    if (m_pressedKeys.contains(Qt::Key_W)) dy -= 1;
    if (m_pressedKeys.contains(Qt::Key_S)) dy += 1;
    if (m_pressedKeys.contains(Qt::Key_A)) dx -= 1;
    if (m_pressedKeys.contains(Qt::Key_D)) dx += 1;

    if (dx != 0 || dy != 0) {
        player->move(dx, dy, gameScene->getValidArea());
    }

    Boss *boss1 = gameScene->getBoss();
    if (boss1) {
        boss1->moveTowardsPlayer(player->getCenterX(), player->getCenterY(), gameScene->getValidArea());
    }

    BossPhase2 *boss2 = gameScene->getBossPhase2();
    if (boss2) {
        boss2->updateRandomMove(gameScene->getValidArea());
    }

    centerOn(player);
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    GameScene *gameScene = qobject_cast<GameScene*>(scene());
    if (gameScene && (gameScene->isIntro() || gameScene->isPhaseTransition() ||
                      gameScene->isPhase2Transition() || gameScene->isPlayerDying() ||
                      gameScene->getGameState() == GameScene::GameState::Victory)) {
        if (event->key() == Qt::Key_F11) {
            QWidget::keyPressEvent(event);
        }
        return;
    }

    m_pressedKeys.insert(event->key());
    if (!gameScene) return;

    if (event->key() == Qt::Key_J) {
        gameScene->processShoot();
    } else if (event->key() == Qt::Key_Shift) {
        gameScene->getPlayer()->setLock(true);
    } else if (event->key() == Qt::Key_R) {
        gameScene->processPotion();
    }
}

void GameView::keyReleaseEvent(QKeyEvent *event)
{
    GameScene *gameScene = qobject_cast<GameScene*>(scene());
    if (gameScene && (gameScene->isIntro() || gameScene->isPhaseTransition() ||
                      gameScene->isPhase2Transition() || gameScene->isPlayerDying() ||
                      gameScene->getGameState() == GameScene::GameState::Victory)) {
        return;
    }

    m_pressedKeys.remove(event->key());
    if (!gameScene) return;

    if (event->key() == Qt::Key_Shift) {
        gameScene->getPlayer()->setLock(false);
    }
}

void GameView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    int bossX = (width() - m_bossHpBar->width()) / 2;
    int bossY = height() - 60;
    m_bossHpBar->move(bossX, bossY);

    // 死亡覆盖层跟随视口大小变化（新增）
    if (m_deathOverlay) {
        m_deathOverlay->setGeometry(rect());
    }
    if (m_victoryOverlay) {
        m_victoryOverlay->setGeometry(rect());
    }
}
