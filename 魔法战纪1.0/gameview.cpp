#include "gameview.h"
#include "gamescene.h"
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include "player.h"
#include "boss.h"
#include "bossphase2.h"  // 新增

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

void HpBarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), m_bgColor);

    double ratio = static_cast<double>(m_hp) / m_maxHp;
    int fgWidth = static_cast<int>(width() * ratio);
    painter.fillRect(0, 0, fgWidth, height(), m_fgColor);
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

    m_bossHpBar = new HpBarWidget(this);
    m_bossHpBar->setColors(Qt::yellow, Qt::red);
    m_bossHpBar->setFixedSize(400, 12);

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameView::gameLoop);
    m_gameTimer->start(16);
}

GameView::~GameView()
{
}

HpBarWidget* GameView::playerHpBar() const { return m_playerHpBar; }
HpBarWidget* GameView::bossHpBar() const { return m_bossHpBar; }

void GameView::gameLoop()
{
    GameScene *gameScene = (GameScene*)(scene());
    if (!gameScene) return;
    Player *player = gameScene->getPlayer();
    if (!player) return;

    player->updatePotionState();
    player->updateInvincibleState();  // 更新无敌状态（新增）

    // 检查阶段切换
    gameScene->checkPhaseTransition();

    // 一阶段技能状态机更新（新增：控制随机释放技能）
    gameScene->updateBossSkillState();

    // 一阶段技能更新
    gameScene->updateBossDash();
    gameScene->updateBossSkillState();
    gameScene->updateBossOrbitingBullets();
    gameScene->updateBossHomingBullets();
    gameScene->updateBossSpread();
    gameScene->updateFissionBullets();

    // 二阶段更新（新增）
    gameScene->updateBossPhase2();
    gameScene->updateBossPhase2Finale();
    gameScene->updateBossClones();  // 更新分身弹幕

    gameScene->updateLockTarget();
    gameScene->updateBullets();
    gameScene->checkBulletCollisions();
    gameScene->cleanupBullets();


    m_playerHpBar->setHp(player->getHp(), player->getMaxHp());

    // 血量显示
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

    // Boss移动
    Boss *boss1 = gameScene->getBoss();
    if (boss1) {
        boss1->moveTowardsPlayer(
            player->getCenterX(),
            player->getCenterY(),
            gameScene->getValidArea()
            );
    }

    // 二阶段Boss随机直线移动（技能完成后高速随机移动，碰墙反弹）
    BossPhase2 *boss2 = gameScene->getBossPhase2();
    if (boss2) {
        boss2->updateRandomMove(gameScene->getValidArea());
    }

    centerOn(player);
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    m_pressedKeys.insert(event->key());

    GameScene *gameScene = (GameScene*)(scene());
    if (!gameScene) return;

    if (event->key() == Qt::Key_J) {
        gameScene->processShoot();
    }
    else if (event->key() == Qt::Key_Shift) {
        gameScene->getPlayer()->setLock(true);
    }
    else if (event->key() == Qt::Key_R) {
        gameScene->processPotion();
    }
}

void GameView::keyReleaseEvent(QKeyEvent *event)
{
    m_pressedKeys.remove(event->key());

    GameScene *gameScene = (GameScene*)(scene());
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
}
