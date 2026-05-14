#include "widget.h"
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
    m_startPage = new QWidget();

    QVBoxLayout *startLayout = new QVBoxLayout(m_startPage);
    startLayout->setAlignment(Qt::AlignCenter);
    startLayout->addStretch(2);

    m_startBtn = new QPushButton("开始游戏");
    m_startBtn->setFixedSize(200, 60);
    startLayout->addWidget(m_startBtn, 0, Qt::AlignCenter);

    startLayout->addStretch(1);

    connect(m_startBtn, &QPushButton::clicked,
            this, &Widget::onStartClicked);

    // ========== 第1页：游戏场地 ==========
    GameScene *scene = new GameScene();
    GameView *view = new GameView(scene, this);
    m_gamePage = view;

    m_stack->addWidget(m_startPage);
    m_stack->addWidget(m_gamePage);

    m_stack->setCurrentIndex(0);
}

Widget::~Widget() {}

void Widget::onStartClicked()
{
    m_stack->setCurrentIndex(1);
    m_gamePage->setFocus();

    GameScene *gameScene = (GameScene*)(m_gamePage->scene());
    if (gameScene) {
        // 连接血量变化信号，自动更新 UI 血条
        connect(gameScene, &GameScene::playerHpChanged,
                this, [this](int hp, int maxHp) {
                    m_gamePage->playerHpBar()->setHp(hp, maxHp);
                });
        connect(gameScene, &GameScene::bossHpChanged,
                this, [this](int hp, int maxHp) {
                    m_gamePage->bossHpBar()->setHp(hp, maxHp);
                });

        // 初始化血条显示
        m_gamePage->playerHpBar()->setHp(
            gameScene->getPlayer()->getHp(),
            gameScene->getPlayer()->getMaxHp()
            );
        m_gamePage->bossHpBar()->setHp(
            gameScene->getBoss()->getHp(),
            gameScene->getBoss()->getMaxHp()
            );
    }
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
