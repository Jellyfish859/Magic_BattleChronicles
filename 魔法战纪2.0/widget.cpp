#include "widget.h"

// ========== StartPage 实现 ==========

StartPage::StartPage(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void StartPage::setBackground(const QPixmap &pixmap)
{
    m_bgPixmap = pixmap;
    update();
}

void StartPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 背景图拉伸填充整个窗口
    if (!m_bgPixmap.isNull()) {
        painter.drawPixmap(rect(), m_bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(rect(), Qt::black);
    }
}

void StartPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}


#include "gamescene.h"
#include "gameview.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "player.h"
#include "boss.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    resize(800, 600);
    setWindowTitle("MyGame");

    m_stack = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_stack);
    setLayout(mainLayout);

    // ========== 第0页：开始界面 ==========
    // 使用自定义StartPage，支持背景图自适应填充
    // 为什么：paintEvent绘制可以随窗口大小变化自动调整
    m_startPage = new StartPage(this);

    // 加载背景图和按钮贴图
    m_bgPixmap.load(":/images/images/start_bg.png");
    m_btnPixmap.load(":/images/images/start_btn.png");
    m_btnHoverPixmap.load(":/images/images/start_btn_hover.png");

    // 设置背景图（填充整个窗口）
    m_startPage->setBackground(m_bgPixmap);

    QVBoxLayout *startLayout = new QVBoxLayout(m_startPage);
    startLayout->setAlignment(Qt::AlignCenter);
    startLayout->addStretch(2);

    m_startBtn = new QPushButton("");
    m_startBtn->setFixedSize(200, 60);
    // 设置按钮贴图（新增）
    if (!m_btnPixmap.isNull() && !m_btnHoverPixmap.isNull()) {
        // 使用QSS设置背景图，支持:hover状态
        m_startBtn->setStyleSheet(QString(
                                      "QPushButton {"
                                      "  background-image: url(%1);"
                                      "  background-repeat: no-repeat;"
                                      "  background-position: center;"
                                      "  border: none;"
                                      "}"
                                      "QPushButton:hover {"
                                      "  background-image: url(%2);"
                                      "  background-repeat: no-repeat;"
                                      "  background-position: center;"
                                      "  border: none;"
                                      "}"
                                      ).arg(":/images/images/start_btn.png")
                                      .arg(":/images/images/start_btn_hover.png"));
    } else if (!m_btnPixmap.isNull()) {
        // 只有普通状态图片
        m_startBtn->setIcon(QIcon(m_btnPixmap));
        m_startBtn->setIconSize(QSize(200, 60));
        m_startBtn->setStyleSheet("QPushButton { background: transparent; border: none; }");
    } else {
        m_startBtn->setText("开始游戏");
    }
    startLayout->addWidget(m_startBtn, 0, Qt::AlignCenter);

    startLayout->addStretch(1);

    connect(m_startBtn, &QPushButton::clicked,
            this, &Widget::onStartClicked);

    // ========== 第1页：游戏场地（初始为空，在onStartClicked中创建）==========
    // 延迟创建GameView，确保每次开始游戏都是全新状态
    m_gamePage = nullptr;

    m_stack->addWidget(m_startPage);
    // m_gamePage 将在 onStartClicked 中添加

    m_stack->setCurrentIndex(0);
}

Widget::~Widget() {}

void Widget::onStartClicked()
{
    // 如果已有游戏页面，先移除
    if (m_gamePage) {
        m_stack->removeWidget(m_gamePage);
        delete m_gamePage;
        m_gamePage = nullptr;
    }

    // 创建新的游戏场景和视图（确保每次都是全新游戏）
    GameScene *scene = new GameScene();
    GameView *view = new GameView(scene, this);
    m_gamePage = view;

    m_stack->addWidget(m_gamePage);
    m_stack->setCurrentIndex(1);
    m_gamePage->setFocus();

    GameScene *gameScene = qobject_cast<GameScene*>(m_gamePage->scene());
    GameView *gameView = qobject_cast<GameView*>(m_gamePage);

    if (gameScene && gameView) {
        // 连接血量变化信号
        connect(gameScene, &GameScene::playerHpChanged,
                this, [this](int hp, int maxHp) {
                    m_gamePage->playerHpBar()->setHp(hp, maxHp);
                });
        connect(gameScene, &GameScene::bossHpChanged,
                this, [this](int hp, int maxHp) {
                    m_gamePage->bossHpBar()->setHp(hp, maxHp);
                });

        // 连接一阶段死亡演出信号
        connect(gameScene, &GameScene::bossDeathStageChanged,
                gameView, &GameView::onBossDeathStageChanged);
        connect(gameScene, &GameScene::bossDeathCameraMove,
                gameView, &GameView::onBossDeathCameraMove);
        connect(gameScene, &GameScene::bossDeathFadeOut,
                gameView, &GameView::onBossDeathFadeOut);
        connect(gameScene, &GameScene::bossDeathFadeIn,
                gameView, &GameView::onBossDeathFadeIn);
        connect(gameScene, &GameScene::bossDeathFadeToBlack,
                gameView, &GameView::onBossDeathFadeToBlack);
        connect(gameScene, &GameScene::bossDeathCameraToPlayer,
                gameView, &GameView::onBossDeathCameraToPlayer);
        connect(gameScene, &GameScene::bossDeathComplete,
                gameView, &GameView::onBossDeathComplete);

        // 连接二阶段死亡演出信号
        connect(gameScene, &GameScene::phase2DeathStageChanged,
                gameView, &GameView::onBossPhase2DeathStageChanged);
        connect(gameScene, &GameScene::phase2DeathCameraMove,
                gameView, &GameView::onBossPhase2DeathCameraMove);
        connect(gameScene, &GameScene::phase2DeathFadeOut,
                gameView, &GameView::onBossPhase2DeathFadeOut);
        connect(gameScene, &GameScene::phase2DeathCleanup,
                gameView, &GameView::onBossPhase2DeathCleanup);
        connect(gameScene, &GameScene::phase2DeathComplete,
                gameView, &GameView::onBossPhase2DeathComplete);

        // 连接玩家死亡演出信号（修正后）
        connect(gameScene, &GameScene::playerDeathStageChanged,
                gameView, &GameView::onPlayerDeathStageChanged);
        connect(gameScene, &GameScene::playerDeathPlayerFadeOut,
                gameView, &GameView::onPlayerDeathPlayerFadeOut);
        connect(gameScene, &GameScene::playerDeathImageFadeIn,
                gameView, &GameView::onPlayerDeathImageFadeIn);
        connect(gameScene, &GameScene::playerDeathScreenFadeToBlack,
                gameView, &GameView::onPlayerDeathScreenFadeToBlack);
        connect(gameScene, &GameScene::playerDeathComplete,
                gameView, &GameView::onPlayerDeathComplete);
        connect(gameScene, &GameScene::playerDeathComplete,
                this, &Widget::onPlayerDeathComplete);

        // 连接胜利演出信号（新增）
        connect(gameScene, &GameScene::victoryStageChanged,
                this, [this](int stage) {
                    Q_UNUSED(stage)
                    // VictoryOverlay在GameView中通过其他信号控制
                });
        connect(gameScene, &GameScene::victoryImageFadeIn,
                gameView, &GameView::onVictoryImageFadeIn);
        connect(gameScene, &GameScene::victoryScreenFadeToBlack,
                gameView, &GameView::onVictoryScreenFadeToBlack);
        connect(gameScene, &GameScene::victoryComplete,
                gameView, &GameView::onVictoryComplete);
        connect(gameScene, &GameScene::victoryComplete,
                this, &Widget::onPlayerDeathComplete);

        // 初始化血条显示
        m_gamePage->playerHpBar()->setHp(
            gameScene->getPlayer()->getHp(),
            gameScene->getPlayer()->getMaxHp()
            );

        // 启动开场动画
        gameView->startIntro();
    }
}

// onPlayerDeathComplete: 玩家死亡演出结束
void Widget::onPlayerDeathComplete()
{
    // 延迟一小段时间后切回开始界面（让用户看到完全黑屏）
    QTimer::singleShot(500, this, [this]() {
        resetGame();
    });
}

// resetGame: 重置游戏状态
void Widget::resetGame()
{
    if (m_gamePage) {
        m_stack->removeWidget(m_gamePage);
        delete m_gamePage;
        m_gamePage = nullptr;
    }

    m_stack->setCurrentIndex(0);
    m_startBtn->setFocus();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11) {
        if (m_isFullScreen) {
            showNormal();
        } else {
            showFullScreen();
        }
        m_isFullScreen = !m_isFullScreen;
    } else {
        QWidget::keyPressEvent(event);
    }
}
