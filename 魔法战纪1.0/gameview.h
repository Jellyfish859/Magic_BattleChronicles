#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include <QWidget>
#include <QPaintEvent>
#include <QSet>
#include <QTimer>

class HpBarWidget : public QWidget
{
public:
    explicit HpBarWidget(QWidget *parent = nullptr);
    void setHp(int hp, int maxHp);
    void setColors(QColor fg, QColor bg);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_hp = 0;
    int m_maxHp = 100;
    QColor m_fgColor = Qt::green;
    QColor m_bgColor = Qt::red;
};

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GameView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~GameView();

    HpBarWidget* playerHpBar() const;
    HpBarWidget* bossHpBar() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void gameLoop();

private:
    HpBarWidget *m_playerHpBar;
    HpBarWidget *m_bossHpBar;

    QSet<int> m_pressedKeys;
    QTimer *m_gameTimer;
};

#endif // GAMEVIEW_H
