#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>

// StartPage: 自定义开始界面
// 用途：支持背景图填充整个窗口
// 为什么：QLabel无法自适应窗口大小变化，自定义绘制更可靠
class StartPage : public QWidget
{
    Q_OBJECT
public:
    explicit StartPage(QWidget *parent = nullptr);
    void setBackground(const QPixmap &pixmap);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPixmap m_bgPixmap;
};

class GameView;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onStartClicked();
    void onPlayerDeathComplete();

private:
    void resetGame();

    QStackedWidget *m_stack;
    StartPage *m_startPage;
    GameView *m_gamePage;
    QPushButton *m_startBtn;

    QPixmap m_bgPixmap;
    QPixmap m_btnPixmap;
    QPixmap m_btnHoverPixmap;

    bool m_isFullScreen = false;
};
#endif // WIDGET_H
