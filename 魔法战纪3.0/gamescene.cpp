#include "gamescene.h"
#include "player.h"
#include "boss.h"
#include "bossphase2.h"
#include "bullet.h"
#include <QtMath>
#include "warningrect.h"
#include "bossclone.h"

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 1280, 1280);
    setupBackground();

    m_player = new Player();
    addItem(m_player);

    m_boss = nullptr;

    m_validArea = QRectF(100, 100, 1080, 1080);

    m_introTimer.start();
}

QRectF GameScene::getValidArea() const
{
    return m_validArea;
}

GameScene::GameState GameScene::getGameState() const
{
    return m_gameState;
}

bool GameScene::isPlaying() const
{
    return m_gameState == GameState::Playing;
}

bool GameScene::isIntro() const
{
    return m_gameState == GameState::Intro;
}

bool GameScene::isPhaseTransition() const
{
    return m_gameState == GameState::PhaseTransition;
}

bool GameScene::isPhase2Transition() const
{
    return m_gameState == GameState::Phase2Transition;
}

// isPlayerDying: 是否处于玩家死亡演出中
bool GameScene::isPlayerDying() const
{
    return m_gameState == GameState::PlayerDying;
}

void GameScene::startIntro()
{
    m_gameState = GameState::Intro;
    m_introStage = 0;
    m_introTimer.restart();
}

void GameScene::updateIntro()
{
    if (m_gameState != GameState::Intro) return;

    int elapsed = m_introTimer.elapsed();

    if (m_introStage == 0) {
        m_introStage = 1;
        m_introTimer.restart();
        emit introStageChanged(1);
        return;
    }

    if (m_introStage == 1) {
        if (elapsed >= INTRO_CAMERA_TO_CENTER) {
            m_introStage = 2;
            m_introTimer.restart();
            emit introStageChanged(2);
            emit introBossAppearing();
        }
        return;
    }

    if (m_introStage == 2) {
        if (!m_boss) {
            m_boss = new Boss();
            addItem(m_boss);
        }

        if (m_boss) {
            qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / INTRO_BOSS_APPEAR, 1.0);
            m_boss->setIntroProgress(progress);
        }

        if (elapsed >= INTRO_BOSS_APPEAR) {
            if (m_boss) {
                m_boss->finishIntro();
            }
            m_introStage = 3;
            m_introTimer.restart();
            emit introStageChanged(3);
            emit introBossHpBarShow();
        }
        return;
    }

    if (m_introStage == 3) {
        if (elapsed >= INTRO_HP_BAR_FADE) {
            m_introStage = 4;
            m_introTimer.restart();
            emit introStageChanged(4);
        }
        return;
    }

    if (m_introStage == 4) {
        if (elapsed >= INTRO_CAMERA_BACK) {
            m_introStage = 5;
            m_introTimer.restart();
            emit introStageChanged(5);
        }
        return;
    }

    if (m_introStage == 5) {
        if (elapsed >= INTRO_BATTLE_DELAY) {
            m_introStage = 0;
            m_gameState = GameState::Playing;
            emit introStageChanged(0);
        }
        return;
    }
}

bool GameScene::isIntroComplete() const
{
    return m_gameState != GameState::Intro;
}

// ========== 一阶段死亡演出系统 ==========

void GameScene::startBossDeathSequence()
{
    if (m_bossDeathState != BossDeathState::NotStarted) return;

    m_gameState = GameState::PhaseTransition;
    m_bossDeathState = BossDeathState::CameraToBoss;
    m_bossDeathTimer.restart();
    m_bossDeathBlinkCount = 0;
    m_bossDeathBlinkVisible = true;

    if (m_boss) {
        emit bossDeathCameraMove(m_boss->getCenterX(), m_boss->getCenterY(), DEATH_CAMERA_TO_BOSS);
    }
    emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
}

void GameScene::updateBossDeathSequence()
{
    if (m_gameState != GameState::PhaseTransition) return;
    if (!m_boss) return;

    int elapsed = m_bossDeathTimer.elapsed();

    switch (m_bossDeathState) {
    case BossDeathState::CameraToBoss: {
        if (elapsed >= DEATH_CAMERA_TO_BOSS) {
            m_bossDeathState = BossDeathState::Blinking;
            m_bossDeathTimer.restart();
            m_bossDeathBlinkCount = 0;
            m_bossDeathBlinkVisible = true;
            emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
        }
        break;
    }

    case BossDeathState::Blinking: {
        int blinkPhase = elapsed / DEATH_BLINK_INTERVAL;
        if (blinkPhase != m_bossDeathBlinkCount) {
            m_bossDeathBlinkCount = blinkPhase;
            m_bossDeathBlinkVisible = !m_bossDeathBlinkVisible;
            m_boss->setIntroOpacity(m_bossDeathBlinkVisible ? 1.0 : 0.2);
        }

        if (m_bossDeathBlinkCount >= 4) {
            m_bossDeathState = BossDeathState::FadeOut;
            m_bossDeathTimer.restart();
            emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
            emit bossDeathFadeOut();
        }
        break;
    }

    case BossDeathState::FadeOut: {
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / DEATH_FADE_OUT_DURATION, 1.0);
        qreal opacity = 1.0 - progress;
        m_boss->setIntroOpacity(opacity);

        if (elapsed >= DEATH_FADE_OUT_DURATION) {
            m_boss->setIntroOpacity(0.0);
            // 移动到场景中心（透明状态）
            m_boss->setPos(640 - m_boss->pixmap().width() / 2, 640 - m_boss->pixmap().height() / 2);

            m_bossDeathState = BossDeathState::AppearAtCenter;
            m_bossDeathTimer.restart();
            emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
            emit bossDeathFadeIn();
        }
        break;
    }

    case BossDeathState::AppearAtCenter: {
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / DEATH_APPEAR_DURATION, 1.0);
        qreal opacity = progress * progress;
        m_boss->setIntroOpacity(opacity);

        if (elapsed >= DEATH_APPEAR_DURATION) {
            m_boss->setIntroOpacity(1.0);
            m_bossDeathState = BossDeathState::CameraToCenter;
            m_bossDeathTimer.restart();

            emit bossDeathCameraMove(640.0, 640.0, DEATH_CAMERA_TO_CENTER);
            emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
        }
        break;
    }

    case BossDeathState::CameraToCenter: {
        if (elapsed >= DEATH_CAMERA_TO_CENTER) {
            m_bossDeathState = BossDeathState::WaitBeforeFade;
            m_bossDeathTimer.restart();
            emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
        }
        break;
    }

    case BossDeathState::WaitBeforeFade: {
        // 停留期间Boss逐渐淡出（新增）
        qreal fadeProgress = qBound(0.0, static_cast<qreal>(elapsed) / DEATH_WAIT_DURATION, 1.0);
        m_boss->setIntroOpacity(1.0 - fadeProgress);  // 1.0→0.0

        if (elapsed >= DEATH_WAIT_DURATION) {
            m_boss->setIntroOpacity(0.0);
            m_bossDeathState = BossDeathState::FadeToBlack;
            m_bossDeathTimer.restart();
            emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
            emit bossDeathFadeToBlack();  // 屏幕开始淡出
        }
        break;
    }

    case BossDeathState::FadeToBlack: {
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / DEATH_FADE_TO_BLACK, 1.0);

        if (elapsed >= DEATH_FADE_TO_BLACK) {
            // 屏幕完全变黑后，镜头回主角
            m_bossDeathState = BossDeathState::CameraToPlayer;
            m_bossDeathTimer.restart();
            emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
            emit bossDeathCameraToPlayer();  // 通知镜头回主角
        }
        break;
    }

    case BossDeathState::CameraToPlayer: {
        // 镜头回主角位置
        if (elapsed >= DEATH_CAMERA_TO_PLAYER) {
            m_bossDeathState = BossDeathState::Complete;
            emit bossDeathStageChanged(static_cast<int>(m_bossDeathState));
            emit bossDeathComplete();
        }
        break;
    }

    case BossDeathState::Complete: {
        performPhaseTransition();
        break;
    }

    case BossDeathState::NotStarted:
        break;
    }
}

GameScene::BossDeathState GameScene::getBossDeathState() const
{
    return m_bossDeathState;
}

bool GameScene::isBossDeathComplete() const
{
    return m_bossDeathState == BossDeathState::Complete;
}

qreal GameScene::getBossDeathProgress() const
{
    if (m_bossDeathState == BossDeathState::NotStarted ||
        m_bossDeathState == BossDeathState::Complete) {
        return 0.0;
    }

    int elapsed = m_bossDeathTimer.elapsed();
    switch (m_bossDeathState) {
    case BossDeathState::CameraToBoss:
        return qBound(0.0, static_cast<qreal>(elapsed) / DEATH_CAMERA_TO_BOSS, 1.0);
    case BossDeathState::Blinking:
        return qBound(0.0, static_cast<qreal>(elapsed) / (DEATH_BLINK_INTERVAL * 4), 1.0);
    case BossDeathState::FadeOut:
        return qBound(0.0, static_cast<qreal>(elapsed) / DEATH_FADE_OUT_DURATION, 1.0);
    case BossDeathState::AppearAtCenter:
        return qBound(0.0, static_cast<qreal>(elapsed) / DEATH_APPEAR_DURATION, 1.0);
    case BossDeathState::CameraToCenter:
        return qBound(0.0, static_cast<qreal>(elapsed) / DEATH_CAMERA_TO_CENTER, 1.0);
    case BossDeathState::WaitBeforeFade:
        return qBound(0.0, static_cast<qreal>(elapsed) / DEATH_WAIT_DURATION, 1.0);
    case BossDeathState::FadeToBlack:
        return qBound(0.0, static_cast<qreal>(elapsed) / DEATH_FADE_TO_BLACK, 1.0);
    case BossDeathState::CameraToPlayer:
        return qBound(0.0, static_cast<qreal>(elapsed) / DEATH_CAMERA_TO_PLAYER, 1.0);
    default:
        return 0.0;
    }
}

void GameScene::performPhaseTransition()
{
    if (m_phaseTransitionDone) return;
    m_phaseTransitionDone = true;
    m_currentPhase = 2;

    if (m_boss) {
        removeItem(m_boss);
        delete m_boss;
        m_boss = nullptr;
    }

    for (Bullet *bullet : m_bossOrbitingBullets) {
        if (bullet->scene() == this) removeItem(bullet);
        delete bullet;
    }
    m_bossOrbitingBullets.clear();

    for (Bullet *bullet : m_bossHomingBullets) {
        if (bullet->scene() == this) removeItem(bullet);
        delete bullet;
    }
    m_bossHomingBullets.clear();

    for (Bullet *bullet : m_bossSpreadBullets) {
        if (bullet->scene() == this) removeItem(bullet);
        delete bullet;
    }
    m_bossSpreadBullets.clear();

    for (Bullet *bullet : m_bossDashBullets) {
        if (bullet->scene() == this) removeItem(bullet);
        delete bullet;
    }
    m_bossDashBullets.clear();

    for (Bullet *bullet : m_fissionBullets) {
        if (bullet->scene() == this) removeItem(bullet);
        delete bullet;
    }
    m_fissionBullets.clear();

    m_orbitingActive = false;
    m_bossDeathState = BossDeathState::NotStarted;

    m_bossPhase2 = new BossPhase2();
    addItem(m_bossPhase2);

    if (m_bossPhase2->getWarningRect()) {
        addItem(m_bossPhase2->getWarningRect());
    }
    if (m_bossPhase2->getWarningCircle()) {
        addItem(m_bossPhase2->getWarningCircle());
    }

    m_gameState = GameState::Playing;

    emit phaseChanged(2);
    emit bossHpChanged(m_bossPhase2->getHp(), m_bossPhase2->getMaxHp());
}

void GameScene::checkPhaseTransition()
{
    // 检查一阶段死亡
    if (!m_phaseTransitionDone && m_currentPhase == 1 && m_boss && m_boss->getHp() <= 0) {
        if (m_bossDeathState == BossDeathState::NotStarted) {
            startBossDeathSequence();
        }
        return;
    }

    // 检查二阶段死亡
    if (m_currentPhase == 2 && m_bossPhase2 && m_bossPhase2->getHp() <= 0) {
        if (m_phase2DeathState == Phase2DeathState::NotStarted) {
            startPhase2DeathSequence();
        }
    }
}

// checkPlayerDeath: 检查玩家是否死亡并触发死亡演出
void GameScene::checkPlayerDeath()
{
    if (m_gameState != GameState::Playing && m_gameState != GameState::Intro) return;
    if (!m_player) return;
    if (m_player->getHp() > 0) return;
    if (m_playerDeathState != PlayerDeathState::NotStarted) return;

    startPlayerDeathSequence();
}

// ========== 二阶段死亡演出系统 ==========

void GameScene::startPhase2DeathSequence()
{
    if (m_phase2DeathState != Phase2DeathState::NotStarted) return;

    m_gameState = GameState::Phase2Transition;
    m_phase2DeathState = Phase2DeathState::CameraToBoss;
    m_phase2DeathTimer.restart();
    m_phase2DeathBlinkCount = 0;
    m_phase2DeathBlinkVisible = true;

    // 发出信号：镜头移向二阶段Boss
    if (m_bossPhase2) {
        emit phase2DeathCameraMove(m_bossPhase2->getCenterX(), m_bossPhase2->getCenterY(), P2_DEATH_CAMERA_TO_BOSS);
    }
    emit phase2DeathStageChanged(static_cast<int>(m_phase2DeathState));
}

void GameScene::updatePhase2DeathSequence()
{
    if (m_gameState != GameState::Phase2Transition) return;
    if (!m_bossPhase2) return;

    int elapsed = m_phase2DeathTimer.elapsed();

    switch (m_phase2DeathState) {
    case Phase2DeathState::CameraToBoss: {
        // 镜头移向Boss（由GameView处理摄像机移动）
        if (elapsed >= P2_DEATH_CAMERA_TO_BOSS) {
            m_phase2DeathState = Phase2DeathState::Blinking;
            m_phase2DeathTimer.restart();
            m_phase2DeathBlinkCount = 0;
            m_phase2DeathBlinkVisible = true;
            emit phase2DeathStageChanged(static_cast<int>(m_phase2DeathState));
        }
        break;
    }

    case Phase2DeathState::Blinking: {
        // Boss闪烁
        int blinkPhase = elapsed / P2_DEATH_BLINK_INTERVAL;
        if (blinkPhase != m_phase2DeathBlinkCount) {
            m_phase2DeathBlinkCount = blinkPhase;
            m_phase2DeathBlinkVisible = !m_phase2DeathBlinkVisible;
            // 二阶段Boss使用 setOpacity 控制闪烁
            m_bossPhase2->setOpacity(m_phase2DeathBlinkVisible ? 1.0 : 0.2);
        }

        if (m_phase2DeathBlinkCount >= 4) {
            m_phase2DeathState = Phase2DeathState::FadeOut;
            m_phase2DeathTimer.restart();
            emit phase2DeathStageChanged(static_cast<int>(m_phase2DeathState));
            emit phase2DeathFadeOut();
        }
        break;
    }

    case Phase2DeathState::FadeOut: {
        // Boss淡出死亡
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / P2_DEATH_FADE_OUT_DURATION, 1.0);
        qreal opacity = 1.0 - progress;
        m_bossPhase2->setOpacity(opacity);

        if (elapsed >= P2_DEATH_FADE_OUT_DURATION) {
            m_bossPhase2->setOpacity(0.0);
            m_phase2DeathState = Phase2DeathState::Cleanup;
            m_phase2DeathTimer.restart();
            emit phase2DeathStageChanged(static_cast<int>(m_phase2DeathState));
            emit phase2DeathCleanup();
        }
        break;
    }

    case Phase2DeathState::Cleanup: {
        // 清理血条、分身、子弹
        if (elapsed >= P2_DEATH_CLEANUP_DURATION) {
            performPhase2Cleanup();
            m_phase2DeathState = Phase2DeathState::Complete;
            emit phase2DeathStageChanged(static_cast<int>(m_phase2DeathState));
            emit phase2DeathComplete();
        }
        break;
    }

    case Phase2DeathState::Complete: {
        // 演出结束，由performPhase2Cleanup启动胜利演出
        break;
    }

    case Phase2DeathState::NotStarted:
        break;
    }
}

GameScene::Phase2DeathState GameScene::getPhase2DeathState() const
{
    return m_phase2DeathState;
}

bool GameScene::isPhase2DeathComplete() const
{
    return m_phase2DeathState == Phase2DeathState::Complete;
}

qreal GameScene::getPhase2DeathProgress() const
{
    if (m_phase2DeathState == Phase2DeathState::NotStarted ||
        m_phase2DeathState == Phase2DeathState::Complete) {
        return 0.0;
    }

    int elapsed = m_phase2DeathTimer.elapsed();
    switch (m_phase2DeathState) {
    case Phase2DeathState::CameraToBoss:
        return qBound(0.0, static_cast<qreal>(elapsed) / P2_DEATH_CAMERA_TO_BOSS, 1.0);
    case Phase2DeathState::Blinking:
        return qBound(0.0, static_cast<qreal>(elapsed) / (P2_DEATH_BLINK_INTERVAL * 4), 1.0);
    case Phase2DeathState::FadeOut:
        return qBound(0.0, static_cast<qreal>(elapsed) / P2_DEATH_FADE_OUT_DURATION, 1.0);
    case Phase2DeathState::Cleanup:
        return qBound(0.0, static_cast<qreal>(elapsed) / P2_DEATH_CLEANUP_DURATION, 1.0);
    default:
        return 0.0;
    }
}

// performPhase2Cleanup: 二阶段死亡后的清理
void GameScene::performPhase2Cleanup()
{
    // 删除二阶段Boss
    if (m_bossPhase2) {
        // 先删除预警矩形和圆形
        if (m_bossPhase2->getWarningRect()) {
            if (m_bossPhase2->getWarningRect()->scene()) {
                removeItem(m_bossPhase2->getWarningRect());
            }
        }
        if (m_bossPhase2->getWarningCircle()) {
            if (m_bossPhase2->getWarningCircle()->scene()) {
                removeItem(m_bossPhase2->getWarningCircle());
            }
        }
        removeItem(m_bossPhase2);
        delete m_bossPhase2;
        m_bossPhase2 = nullptr;
    }

    // 删除所有分身
    for (BossClone *clone : m_bossClones) {
        if (clone->scene()) {
            removeItem(clone);
        }
        delete clone;
    }
    m_bossClones.clear();
    m_clonesSpawned = false;

    // 清理二阶段子弹
    for (Bullet *bullet : m_phase2Bullets) {
        if (bullet->scene()) {
            removeItem(bullet);
        }
        delete bullet;
    }
    m_phase2Bullets.clear();

    // 清理裂变子弹
    for (Bullet *bullet : m_fissionBullets) {
        if (bullet->scene()) {
            removeItem(bullet);
        }
        delete bullet;
    }
    m_fissionBullets.clear();

    m_phase2DeathDone = true;

    // 启动胜利演出（新增）
    startVictorySequence();
}

// startVictorySequence: 启动胜利演出
void GameScene::startVictorySequence()
{
    m_gameState = GameState::Victory;
    m_victoryTimer.start();  // 使用start()确保计时器有效
    m_victoryState = 0;
    emit victoryStageChanged(0);
}

// updateVictorySequence: 更新胜利演出
void GameScene::updateVictorySequence()
{
    if (m_gameState != GameState::Victory) return;

    int elapsed = m_victoryTimer.elapsed();

    // 阶段0：VICTORY图片淡入 (1000ms)
    if (m_victoryState == 0) {
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / 1000, 1.0);
        emit victoryImageFadeIn(progress);
        if (elapsed >= 1000) {
            m_victoryState = 1;
            m_victoryTimer.restart();
            emit victoryStageChanged(1);
        }
        return;
    }

    // 阶段1：等待 (1000ms)
    if (m_victoryState == 1) {
        if (elapsed >= 1000) {
            m_victoryState = 2;
            m_victoryTimer.restart();
            emit victoryStageChanged(2);
        }
        return;
    }

    // 阶段2：黑屏淡出 (1500ms)
    if (m_victoryState == 2) {
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / 1500, 1.0);
        emit victoryScreenFadeToBlack(progress);
        if (elapsed >= 1500) {
            m_victoryState = 3;
            emit victoryStageChanged(3);
            emit victoryComplete();
        }
        return;
    }
}

BossPhase2* GameScene::getBossPhase2() const
{
    return m_bossPhase2;
}

void GameScene::setupBackground()
{
    m_background.load(":/images/images/text.jpg");
    setBackgroundBrush(m_background);
}

void GameScene::updateLockTarget()
{
    if (!m_player->isLocked()) {
        return;
    }
    if (m_boss) {
        m_player->setLockTarget(m_boss->getCenterX(), m_boss->getCenterY());
    } else if (m_bossPhase2) {
        m_player->setLockTarget(m_bossPhase2->getCenterX(), m_bossPhase2->getCenterY());
    }
}

void GameScene::processShoot()
{
    if (!isPlaying()) return;

    if (!m_player->canShoot()) {
        return;
    }

    updateLockTarget();

    qreal dirX = m_player->getShootDirX();
    qreal dirY = m_player->getShootDirY();

    Bullet *bullet = Bullet::createPlayerBullet(m_player->getShootX(), m_player->getShootY(), dirX, dirY, 10);
    m_bullets.append(bullet);
    addItem(bullet);

    m_player->onShoot();
}

void GameScene::processPotion()
{
    if (!isPlaying()) return;
    if (!m_player->usePotion()) {
        return;
    }
}

void GameScene::updateBossSkillState()
{
    if (!isPlaying()) return;
    if (!m_boss) return;

    m_boss->updateSkillState();

    if (!m_boss->isAnySkillActive()) {
        m_boss->pickNextSkill();
    }
}

void GameScene::updateBossDash()
{
    if (!isPlaying()) return;
    if (!m_boss) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();

    QList<Bullet*> newBullets;
    if (m_boss->getCurrentSkillState() == Boss::SkillState::Idle) {
        newBullets = m_boss->updateDashState(playerX, playerY, m_validArea);
    }

    for (Bullet *bullet : newBullets) {
        m_bossDashBullets.append(bullet);
        addItem(bullet);
    }

    if (m_bossDashBullets.isEmpty()) {
        return;
    }

    qreal playerRadius = m_player->pixmap().width() / 2;

    for (int i = m_bossDashBullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_bossDashBullets[i];

        bullet->update();

        qreal bulletCenterX = bullet->pos().x() + bullet->rect().width() / 2;
        qreal bulletCenterY = bullet->pos().y() + bullet->rect().height() / 2;
        qreal dx = bulletCenterX - playerX;
        qreal dy = bulletCenterY - playerY;
        qreal dist = qSqrt(dx * dx + dy * dy);

        if (dist <= playerRadius + bullet->rect().width() / 2) {
            m_player->takeDamage(8);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());

            removeItem(bullet);
            m_bossDashBullets.removeAt(i);
            delete bullet;
            continue;
        }

        if (bullet->isOutOfBounds(1280, 1280)) {
            removeItem(bullet);
            m_bossDashBullets.removeAt(i);
            delete bullet;
        }
    }
}

QList<Bullet*> GameScene::spawnFissionBullets(Bullet *sourceBullet)
{
    QList<Bullet*> bullets;

    qreal centerX = sourceBullet->pos().x() + sourceBullet->rect().width() / 2;
    qreal centerY = sourceBullet->pos().y() + sourceBullet->rect().height() / 2;

    int count = 8;
    int speed = 5;
    int size = 16;

    for (int i = 0; i < count; ++i) {
        qreal angle = i * (2.0 * M_PI / count);
        Bullet *bullet = Bullet::createFissionBullet(centerX, centerY, angle, speed, size);
        bullets.append(bullet);
    }

    return bullets;
}

void GameScene::updateBossOrbitingBullets()
{
    if (!isPlaying()) return;
    if (!m_boss) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();

    if (m_boss->getCurrentSkillState() == Boss::SkillState::Orbiting) {
        if (!m_orbitingActive && m_boss->shouldSpawnOrbitSkill()) {
            QList<Bullet*> newBullets = m_boss->spawnOrbitingBullets();
            for (Bullet *bullet : newBullets) {
                m_bossOrbitingBullets.append(bullet);
                addItem(bullet);
            }

            m_orbitingActive = true;
            m_orbLaunchTimer.restart();
        }
    }

    if (m_bossOrbitingBullets.isEmpty()) {
        m_orbitingActive = false;
        return;
    }

    qreal bossX = m_boss->getCenterX();
    qreal bossY = m_boss->getCenterY();

    for (Bullet *bullet : m_bossOrbitingBullets) {
        if (!bullet->isFlying()) {
            bullet->setOrbitCenter(bossX, bossY);
        }
    }

    if (m_orbitingActive) {
        Bullet *nextBullet = nullptr;
        for (Bullet *bullet : m_bossOrbitingBullets) {
            if (!bullet->isFlying()) {
                nextBullet = bullet;
                break;
            }
        }

        if (nextBullet && m_orbLaunchTimer.elapsed() >= Boss::getLaunchInterval()) {
            nextBullet->launchTowards(playerX, playerY, Boss::getOrbBulletSpeed());
            m_orbLaunchTimer.restart();
        }
    }

    for (Bullet *bullet : m_bossOrbitingBullets) {
        bullet->update();
    }

    qreal playerRadius = m_player->pixmap().width() / 2;

    for (int i = m_bossOrbitingBullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_bossOrbitingBullets[i];

        if (!bullet->isFlying()) continue;

        qreal bulletCenterX = bullet->pos().x() + bullet->rect().width() / 2;
        qreal bulletCenterY = bullet->pos().y() + bullet->rect().height() / 2;
        qreal dx = bulletCenterX - playerX;
        qreal dy = bulletCenterY - playerY;
        qreal dist = qSqrt(dx * dx + dy * dy);

        if (dist <= playerRadius + bullet->rect().width() / 2) {
            m_player->takeDamage(15);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());

            if (bullet->isFissionCapable() && !bullet->hasFissioned()) {
                bullet->markFissioned();
                QList<Bullet*> fissionBullets = spawnFissionBullets(bullet);
                for (Bullet *fb : fissionBullets) {
                    m_fissionBullets.append(fb);
                    addItem(fb);
                }
            }

            removeItem(bullet);
            m_bossOrbitingBullets.removeAt(i);
            delete bullet;
            continue;
        }

        if (bullet->isOutOfBounds(1280, 1280)) {
            if (bullet->isFissionCapable() && !bullet->hasFissioned()) {
                bullet->markFissioned();
                QList<Bullet*> fissionBullets = spawnFissionBullets(bullet);
                for (Bullet *fb : fissionBullets) {
                    m_fissionBullets.append(fb);
                    addItem(fb);
                }
            }

            removeItem(bullet);
            m_bossOrbitingBullets.removeAt(i);
            delete bullet;
            continue;
        }

        if (bullet->isFissionCapable() && bullet->shouldFission() && !bullet->hasFissioned()) {
            bullet->markFissioned();
            QList<Bullet*> fissionBullets = spawnFissionBullets(bullet);
            for (Bullet *fb : fissionBullets) {
                m_fissionBullets.append(fb);
                addItem(fb);
            }

            removeItem(bullet);
            m_bossOrbitingBullets.removeAt(i);
            delete bullet;
            continue;
        }
    }

    if (m_orbitingActive && !m_bossOrbitingBullets.isEmpty()) {
        bool allDone = true;
        for (Bullet *bullet : m_bossOrbitingBullets) {
            if (!bullet->isFlying() || !bullet->isOutOfBounds(1280, 1280)) {
                allDone = false;
                break;
            }
        }
        if (allDone) {
            m_orbitingActive = false;
            for (Bullet *bullet : m_bossOrbitingBullets) {
                removeItem(bullet);
                delete bullet;
            }
            m_bossOrbitingBullets.clear();
        }
    }
}

void GameScene::updateFissionBullets()
{
    if (!isPlaying()) return;
    if (m_fissionBullets.isEmpty()) {
        return;
    }

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();
    qreal playerRadius = m_player->pixmap().width() / 2;

    for (int i = m_fissionBullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_fissionBullets[i];

        bullet->update();

        qreal bulletCenterX = bullet->pos().x() + bullet->rect().width() / 2;
        qreal bulletCenterY = bullet->pos().y() + bullet->rect().height() / 2;
        qreal dx = bulletCenterX - playerX;
        qreal dy = bulletCenterY - playerY;
        qreal dist = qSqrt(dx * dx + dy * dy);

        if (dist <= playerRadius + bullet->rect().width() / 2) {
            m_player->takeDamage(10);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());

            removeItem(bullet);
            m_fissionBullets.removeAt(i);
            delete bullet;
            continue;
        }

        if (bullet->isOutOfBounds(1280, 1280)) {
            removeItem(bullet);
            m_fissionBullets.removeAt(i);
            delete bullet;
        }
    }
}

void GameScene::updateBossHomingBullets()
{
    if (!isPlaying()) return;
    if (!m_boss) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();

    if (m_boss->getCurrentSkillState() == Boss::SkillState::Homing) {
        if (m_boss->shouldSpawnHomingSkill()) {
            QList<Bullet*> newBullets = m_boss->spawnHomingBullets(playerX, playerY);
            for (Bullet *bullet : newBullets) {
                m_bossHomingBullets.append(bullet);
                addItem(bullet);
            }
        }
    }

    if (m_bossHomingBullets.isEmpty()) {
        return;
    }

    qreal playerRadius = m_player->pixmap().width() / 2;
    int lifeTime = Boss::getHomingLifeTime();
    qreal maxTurn = Boss::getHomingMaxTurn();

    for (int i = m_bossHomingBullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_bossHomingBullets[i];

        bullet->updateHoming(playerX, playerY, maxTurn);
        bullet->update();

        qreal bulletCenterX = bullet->pos().x() + bullet->rect().width() / 2;
        qreal bulletCenterY = bullet->pos().y() + bullet->rect().height() / 2;
        qreal dx = bulletCenterX - playerX;
        qreal dy = bulletCenterY - playerY;
        qreal dist = qSqrt(dx * dx + dy * dy);

        if (dist <= playerRadius + bullet->rect().width() / 2) {
            m_player->takeDamage(10);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());

            removeItem(bullet);
            m_bossHomingBullets.removeAt(i);
            delete bullet;
            continue;
        }

        if (bullet->elapsedTime() >= lifeTime || bullet->isOutOfBounds(1280, 1280)) {
            removeItem(bullet);
            m_bossHomingBullets.removeAt(i);
            delete bullet;
        }
    }
}

void GameScene::updateBossSpread()
{
    if (!isPlaying()) return;
    if (!m_boss) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();

    if (m_boss->getCurrentSkillState() == Boss::SkillState::Spread) {
        Bullet *bullet = m_boss->getNextSpreadBullet(playerX, playerY);

        if (bullet) {
            m_bossSpreadBullets.append(bullet);
            addItem(bullet);
        }
    }

    if (m_bossSpreadBullets.isEmpty()) {
        return;
    }

    qreal playerRadius = m_player->pixmap().width() / 2;

    for (int i = m_bossSpreadBullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_bossSpreadBullets[i];

        bullet->update();

        qreal bulletCenterX = bullet->pos().x() + bullet->rect().width() / 2;
        qreal bulletCenterY = bullet->pos().y() + bullet->rect().height() / 2;
        qreal dx = bulletCenterX - playerX;
        qreal dy = bulletCenterY - playerY;
        qreal dist = qSqrt(dx * dx + dy * dy);

        if (dist <= playerRadius + bullet->rect().width() / 2) {
            m_player->takeDamage(8);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());

            removeItem(bullet);
            m_bossSpreadBullets.removeAt(i);
            delete bullet;
            continue;
        }

        if (bullet->isOutOfBounds(1280, 1280)) {
            removeItem(bullet);
            m_bossSpreadBullets.removeAt(i);
            delete bullet;
        }
    }
}

void GameScene::updateBossPhase2()
{
    if (!isPlaying()) return;
    if (!m_bossPhase2) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();
    qreal playerRadius = m_player->pixmap().width() / 2;

    if (!m_bossPhase2->isSkillComplete()) {
        QList<Bullet*> newBullets = m_bossPhase2->updateBoundaryDash(playerX, playerY, m_validArea);

        for (Bullet *bullet : newBullets) {
            m_phase2Bullets.append(bullet);
            addItem(bullet);
        }
    } else {
        QList<Bullet*> ringBullets = m_bossPhase2->updateRingShoot();
        for (Bullet *bullet : ringBullets) {
            m_phase2Bullets.append(bullet);
            addItem(bullet);
        }
    }

    m_bossPhase2->updateWarningRect();

    if (m_bossPhase2->isBoundaryDashing() && m_bossPhase2->getOpacity() > 0.5) {
        if (m_bossPhase2->checkDashCollision(playerX, playerY, playerRadius)) {
            m_player->takeDamage(35);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());
        }
    }

    for (int i = m_phase2Bullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_phase2Bullets[i];
        bullet->update();

        qreal bulletCenterX = bullet->pos().x() + bullet->rect().width() / 2;
        qreal bulletCenterY = bullet->pos().y() + bullet->rect().height() / 2;
        qreal dx = bulletCenterX - playerX;
        qreal dy = bulletCenterY - playerY;
        qreal dist = qSqrt(dx * dx + dy * dy);

        if (dist <= playerRadius + bullet->rect().width() / 2) {
            m_player->takeDamage(10);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());

            removeItem(bullet);
            m_phase2Bullets.removeAt(i);
            delete bullet;
            continue;
        }

        if (bullet->isOutOfBounds(1280, 1280)) {
            if (bullet->isFissionCapable() && !bullet->hasFissioned()) {
                bullet->markFissioned();
                QList<Bullet*> fissionBullets = spawnFissionBullets(bullet);
                for (Bullet *fb : fissionBullets) {
                    m_fissionBullets.append(fb);
                    addItem(fb);
                }
            }

            removeItem(bullet);
            m_phase2Bullets.removeAt(i);
            delete bullet;
            continue;
        }

        if (bullet->isFissionCapable() && bullet->shouldFission() && !bullet->hasFissioned()) {
            bullet->markFissioned();
            QList<Bullet*> fissionBullets = spawnFissionBullets(bullet);
            for (Bullet *fb : fissionBullets) {
                m_fissionBullets.append(fb);
                addItem(fb);
            }

            removeItem(bullet);
            m_phase2Bullets.removeAt(i);
            delete bullet;
            continue;
        }
    }
}

void GameScene::updateBossPhase2Finale()
{
    if (!isPlaying()) return;
    if (!m_bossPhase2) return;

    if (m_bossPhase2->isFinaleComplete()) {
        if (!m_clonesSpawned) {
            spawnBossClones();
        }
        return;
    }

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();
    qreal playerRadius = m_player->pixmap().width() / 2;

    QList<Bullet*> newBullets = m_bossPhase2->updateFinaleSkill(playerX, playerY, m_validArea);

    for (Bullet *bullet : newBullets) {
        m_phase2Bullets.append(bullet);
        addItem(bullet);
    }

    if (m_bossPhase2->getWarningCircle() &&
        !m_bossPhase2->getWarningCircle()->isVisible() &&
        m_bossPhase2->getOpacity() > 0.5) {
        qreal dx = m_bossPhase2->getCenterX() - playerX;
        qreal dy = m_bossPhase2->getCenterY() - playerY;
        qreal dist = qSqrt(dx * dx + dy * dy);
        if (dist <= 200 + playerRadius) {
            m_player->takeDamage(50);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());
        }
    }
}

void GameScene::updateBossClones()
{
    if (!isPlaying()) return;
    if (m_bossClones.isEmpty()) return;

    for (BossClone *clone : m_bossClones) {
        QList<Bullet*> newBullets = clone->updateShoot();
        for (Bullet *bullet : newBullets) {
            m_phase2Bullets.append(bullet);
            addItem(bullet);
        }
    }
}

void GameScene::spawnBossClones()
{
    if (m_clonesSpawned) return;
    m_clonesSpawned = true;

    qreal margin = 80;
    qreal left = m_validArea.left() + margin;
    qreal right = m_validArea.right() - margin;
    qreal top = m_validArea.top() + margin;
    qreal bottom = m_validArea.bottom() - margin;

    QPointF positions[4] = {
        QPointF(left, top),
        QPointF(right, top),
        QPointF(left, bottom),
        QPointF(right, bottom)
    };

    for (int i = 0; i < 4; ++i) {
        BossClone *clone = new BossClone(positions[i].x(), positions[i].y());
        m_bossClones.append(clone);
        addItem(clone);
    }
}

void GameScene::updateBullets()
{
    for (Bullet *bullet : m_bullets) {
        bullet->update();
    }
}

void GameScene::checkBulletCollisions()
{
    if (!isPlaying()) return;

    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_bullets[i];
        const QList<QGraphicsItem*> collisions = bullet->collidingItems();
        bool hit = false;
        for (QGraphicsItem *item : collisions) {
            if (m_boss && item == m_boss) {
                m_boss->takeDamage(10);
                emit bossHpChanged(m_boss->getHp(), m_boss->getMaxHp());
                hit = true;
                break;
            }
            if (m_bossPhase2 && item == m_bossPhase2) {
                m_bossPhase2->takeDamage(10);
                emit bossHpChanged(m_bossPhase2->getHp(), m_bossPhase2->getMaxHp());
                hit = true;
                break;
            }
        }
        if (hit) {
            removeItem(bullet);
            m_bullets.removeAt(i);
            delete bullet;
        }
    }
}

void GameScene::cleanupBullets()
{
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_bullets[i];
        if (bullet->isOutOfBounds(1280, 1280)) {
            removeItem(bullet);
            m_bullets.removeAt(i);
            delete bullet;
        }
    }
}

Player* GameScene::getPlayer() const { return m_player; }
Boss* GameScene::getBoss() const { return m_boss; }
// ========== 玩家死亡演出系统（新增）==========

// startPlayerDeathSequence: 启动玩家死亡演出
// 用途：玩家血量归零时调用，开始死亡演出流程
// 为什么：需要连贯的淡出→图片→黑屏→返回开始界面
void GameScene::startPlayerDeathSequence()
{
    if (m_playerDeathState != PlayerDeathState::NotStarted) return;

    m_gameState = GameState::PlayerDying;
    m_playerDeathState = PlayerDeathState::PlayerFadeOut;
    m_playerDeathTimer.restart();

    emit playerDeathStageChanged(static_cast<int>(m_playerDeathState));
}

// updatePlayerDeathSequence: 更新玩家死亡演出时间轴
// 用途：每帧调用，推进死亡演出各阶段
// 为什么：死亡演出是多阶段的，需要按时间推进
void GameScene::updatePlayerDeathSequence()
{
    if (m_gameState != GameState::PlayerDying) return;

    int elapsed = m_playerDeathTimer.elapsed();

    switch (m_playerDeathState) {
    case PlayerDeathState::PlayerFadeOut: {
        // 阶段1：玩家淡出
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / PLAYER_DEATH_FADE_OUT, 1.0);
        emit playerDeathPlayerFadeOut(progress);

        if (elapsed >= PLAYER_DEATH_FADE_OUT) {
            m_playerDeathState = PlayerDeathState::ImageFadeIn;
            m_playerDeathTimer.restart();
            emit playerDeathStageChanged(static_cast<int>(m_playerDeathState));
        }
        break;
    }

    case PlayerDeathState::ImageFadeIn: {
        // 阶段2："YOU DIED"图片淡入
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / PLAYER_DEATH_IMAGE_FADE_IN, 1.0);
        emit playerDeathImageFadeIn(progress);

        if (elapsed >= PLAYER_DEATH_IMAGE_FADE_IN) {
            m_playerDeathState = PlayerDeathState::WaitBeforeBlack;
            m_playerDeathTimer.restart();
            emit playerDeathStageChanged(static_cast<int>(m_playerDeathState));
        }
        break;
    }

    case PlayerDeathState::WaitBeforeBlack: {
        // 阶段3：等待一段时间
        if (elapsed >= PLAYER_DEATH_WAIT) {
            m_playerDeathState = PlayerDeathState::ScreenFadeToBlack;
            m_playerDeathTimer.restart();
            emit playerDeathStageChanged(static_cast<int>(m_playerDeathState));
        }
        break;
    }

    case PlayerDeathState::ScreenFadeToBlack: {
        // 阶段4：屏幕慢慢变黑
        qreal progress = qBound(0.0, static_cast<qreal>(elapsed) / PLAYER_DEATH_BLACK_FADE, 1.0);
        emit playerDeathScreenFadeToBlack(progress);

        if (elapsed >= PLAYER_DEATH_BLACK_FADE) {
            m_playerDeathState = PlayerDeathState::Complete;
            emit playerDeathStageChanged(static_cast<int>(m_playerDeathState));
            emit playerDeathComplete();
        }
        break;
    }

    case PlayerDeathState::Complete: {
        // 阶段5：演出结束，进入GameOver状态
        m_gameState = GameState::GameOver;
        break;
    }

    case PlayerDeathState::NotStarted:
        break;
    }
}

GameScene::PlayerDeathState GameScene::getPlayerDeathState() const
{
    return m_playerDeathState;
}

bool GameScene::isPlayerDeathComplete() const
{
    return m_playerDeathState == PlayerDeathState::Complete;
}

qreal GameScene::getPlayerDeathProgress() const
{
    if (m_playerDeathState == PlayerDeathState::NotStarted ||
        m_playerDeathState == PlayerDeathState::Complete) {
        return 0.0;
    }

    int elapsed = m_playerDeathTimer.elapsed();
    switch (m_playerDeathState) {
    case PlayerDeathState::PlayerFadeOut:
        return qBound(0.0, static_cast<qreal>(elapsed) / PLAYER_DEATH_FADE_OUT, 1.0);
    case PlayerDeathState::ImageFadeIn:
        return qBound(0.0, static_cast<qreal>(elapsed) / PLAYER_DEATH_IMAGE_FADE_IN, 1.0);
    case PlayerDeathState::WaitBeforeBlack:
        return qBound(0.0, static_cast<qreal>(elapsed) / PLAYER_DEATH_WAIT, 1.0);
    case PlayerDeathState::ScreenFadeToBlack:
        return qBound(0.0, static_cast<qreal>(elapsed) / PLAYER_DEATH_BLACK_FADE, 1.0);
    default:
        return 0.0;
    }
}
