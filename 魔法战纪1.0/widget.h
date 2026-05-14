#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>

class GameView;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    // keyPressEvent: 键盘按下事件
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onStartClicked();

private:
    QStackedWidget *m_stack;   // 页面切换器
    QWidget *m_startPage;      // 开始界面
    GameView *m_gamePage;     // 游戏场地
    QPushButton *m_startBtn;   // 开始按钮

    // m_isFullScreen: 记录当前是否处于全屏状态
    bool m_isFullScreen = false;
};
#endif // WIDGET_H
