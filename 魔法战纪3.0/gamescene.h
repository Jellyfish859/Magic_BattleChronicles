#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QPixmap>
#include <QList>
#include <QElapsedTimer>
#include <QRectF>
#include "warningcircle.h"

class Player;
class Boss;
class BossPhase2;
class BossClone;
class Bullet;

class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum class GameState {
        Intro,
        Playing,
        PhaseTransition,    // 一阶段死亡演出
        Phase2Transition,   // 二阶段死亡演出
        PlayerDying,        // 玩家死亡演出（新增）
        GameOver,
        Victory
    };

    // 一阶段死亡演出状态机
    enum class BossDeathState {
        NotStarted,
        CameraToBoss,
        Blinking,
        FadeOut,
        AppearAtCenter,
        CameraToCenter,
        WaitBeforeFade,
        FadeToBlack,
        CameraToPlayer,
        Complete
    };

    // 二阶段死亡演出状态机
    // 用途：控制二阶段Boss死亡后的演出（更简单，没有转场和重生）
    // 为什么：二阶段死亡是彻底死亡，不需要中心淡入和镜头回主角
    enum class Phase2DeathState {
        NotStarted,
        CameraToBoss,   // 镜头移向Boss
        Blinking,       // Boss闪烁
        FadeOut,        // Boss淡出死亡
        Cleanup,        // 清理血条、分身、子弹
        Complete        // 演出结束，游戏胜利
    };

    // 玩家死亡演出状态机（新增）
    // 用途：控制玩家死亡后的完整演出流程
    // 为什么：玩家死亡需要淡出→图片淡入→黑屏→返回开始界面的连贯演出
    enum class PlayerDeathState {
        NotStarted,
        PlayerFadeOut,      // 玩家淡出
        ImageFadeIn,        // "YOU DIED"图片淡入
        WaitBeforeBlack,    // 等待
        ScreenFadeToBlack,  // 屏幕变黑
        Complete            // 演出结束
    };

    explicit GameScene(QObject *parent = nullptr);
    Player* getPlayer() const;
    Boss* getBoss() const;
    BossPhase2* getBossPhase2() const;

    QRectF getValidArea() const;

    GameState getGameState() const;
    bool isPlaying() const;
    bool isIntro() const;
    bool isPhaseTransition() const;
    bool isPhase2Transition() const;  // 是否处于二阶段死亡演出
    bool isPlayerDying() const;       // 是否处于玩家死亡演出（新增）

    void processShoot();
    void processPotion();
    void updateLockTarget();

    void updateBullets();
    void checkBulletCollisions();
    void cleanupBullets();

    void updateBossOrbitingBullets();
    void updateBossHomingBullets();
    void updateBossSpread();
    void updateBossDash();
    void updateBossSkillState();
    void updateBossClones();
    void updateFissionBullets();
    void updateBossPhase2();
    void updateBossPhase2Finale();

    void checkPhaseTransition();
    void checkPlayerDeath();          // 检查玩家死亡（新增）

    // 开场动画系统
    void startIntro();
    void updateIntro();
    bool isIntroComplete() const;

    // 一阶段死亡演出系统
    void startBossDeathSequence();
    void updateBossDeathSequence();
    BossDeathState getBossDeathState() const;
    bool isBossDeathComplete() const;
    qreal getBossDeathProgress() const;

    // 二阶段死亡演出系统
    // 用途：控制二阶段Boss死亡后的演出流程
    // 为什么：二阶段死亡需要镜头移动、闪烁、淡出，然后清理分身和血条
    void startPhase2DeathSequence();
    void updatePhase2DeathSequence();
    Phase2DeathState getPhase2DeathState() const;
    bool isPhase2DeathComplete() const;
    qreal getPhase2DeathProgress() const;

    // 玩家死亡演出系统（新增）
    // 用途：控制玩家死亡后的完整演出流程
    // 为什么：需要连贯的淡出→图片→黑屏→返回开始界面
    void startPlayerDeathSequence();
    void updatePlayerDeathSequence();
    PlayerDeathState getPlayerDeathState() const;
    bool isPlayerDeathComplete() const;
    qreal getPlayerDeathProgress() const;

    // 胜利演出系统（新增）
    // 用途：Boss二阶段死亡后显示VICTORY并返回开始界面
    // 为什么：需要完整的胜利演出流程
    void startVictorySequence();
    void updateVictorySequence();

signals:
    void playerHpChanged(int hp, int maxHp);
    void bossHpChanged(int hp, int maxHp);
    void phaseChanged(int phase);

    void introStageChanged(int stage);
    void introBossAppearing();
    void introBossHpBarShow();

    // 一阶段死亡演出信号
    void bossDeathStageChanged(int stage);
    void bossDeathCameraMove(qreal targetX, qreal targetY, int durationMs);
    void bossDeathFadeOut();
    void bossDeathFadeIn();
    void bossDeathFadeToBlack();
    void bossDeathCameraToPlayer();
    void bossDeathComplete();

    // 二阶段死亡演出信号
    void phase2DeathStageChanged(int stage);
    void phase2DeathCameraMove(qreal targetX, qreal targetY, int durationMs);
    void phase2DeathFadeOut();
    void phase2DeathCleanup();    // 清理血条、分身
    void phase2DeathComplete();

    // 胜利演出信号（新增）
    void victoryStageChanged(int stage);
    void victoryImageFadeIn(qreal progress);
    void victoryScreenFadeToBlack(qreal progress);
    void victoryComplete();

    // 玩家死亡演出信号（新增）
    // 用途：通知GameView处理玩家死亡演出的视觉效果
    // 为什么：GameView负责渲染，需要知道当前演出阶段来绘制对应效果
    void playerDeathStageChanged(int stage);
    void playerDeathPlayerFadeOut(qreal progress);      // 玩家淡出进度 0.0~1.0
    void playerDeathImageFadeIn(qreal progress);        // 图片淡入进度 0.0~1.0
    void playerDeathScreenFadeToBlack(qreal progress);  // 黑屏进度 0.0~1.0
    void playerDeathComplete();                          // 演出结束，可以切回开始界面

private:
    void setupBackground();
    void spawnBossClones();
    void performPhaseTransition();
    void performPhase2Cleanup();  // 二阶段死亡后的清理

    QList<Bullet*> spawnFissionBullets(Bullet *sourceBullet);

    QPixmap m_background;
    Player *m_player;
    Boss *m_boss;
    BossPhase2 *m_bossPhase2 = nullptr;

    QList<Bullet*> m_bullets;
    QList<Bullet*> m_bossOrbitingBullets;
    QElapsedTimer m_orbLaunchTimer;
    bool m_orbitingActive = false;

    QList<Bullet*> m_bossHomingBullets;
    QList<Bullet*> m_bossSpreadBullets;
    QList<Bullet*> m_bossDashBullets;
    QList<Bullet*> m_fissionBullets;
    QList<Bullet*> m_phase2Bullets;
    QList<BossClone*> m_bossClones;
    bool m_clonesSpawned = false;

    QRectF m_validArea;

    int m_currentPhase = 1;
    void updateBossPhase2WarningRect();
    bool m_phaseTransitionDone = false;
    bool m_phase2DeathDone = false;  // 二阶段死亡演出是否完成

    // 开场动画系统
    GameState m_gameState = GameState::Intro;
    QElapsedTimer m_introTimer;
    int m_introStage = 0;

    static constexpr int INTRO_CAMERA_TO_CENTER = 1500;
    static constexpr int INTRO_BOSS_APPEAR = 2000;
    static constexpr int INTRO_HP_BAR_FADE = 800;
    static constexpr int INTRO_CAMERA_BACK = 1500;
    static constexpr int INTRO_BATTLE_DELAY = 1000;

    // 一阶段死亡演出时间参数
    static constexpr int DEATH_CAMERA_TO_BOSS = 1500;
    static constexpr int DEATH_BLINK_INTERVAL = 150;
    static constexpr int DEATH_FADE_OUT_DURATION = 500;
    static constexpr int DEATH_APPEAR_DURATION = 1000;
    static constexpr int DEATH_CAMERA_TO_CENTER = 1500;
    static constexpr int DEATH_WAIT_DURATION = 800;   // 停留时间缩短：2秒→0.8秒
    static constexpr int DEATH_FADE_TO_BLACK = 1500;  // 黑屏淡出时间延长：1秒→1.5秒
    static constexpr int DEATH_CAMERA_TO_PLAYER = 1500;

    // 二阶段死亡演出时间参数
    static constexpr int P2_DEATH_CAMERA_TO_BOSS = 1500;
    static constexpr int P2_DEATH_BLINK_INTERVAL = 150;
    static constexpr int P2_DEATH_FADE_OUT_DURATION = 500;
    static constexpr int P2_DEATH_CLEANUP_DURATION = 500;  // 清理动画时间

    BossDeathState m_bossDeathState = BossDeathState::NotStarted;
    QElapsedTimer m_bossDeathTimer;
    int m_bossDeathBlinkCount = 0;
    bool m_bossDeathBlinkVisible = true;

    Phase2DeathState m_phase2DeathState = Phase2DeathState::NotStarted;
    QElapsedTimer m_phase2DeathTimer;
    int m_phase2DeathBlinkCount = 0;
    bool m_phase2DeathBlinkVisible = true;

    // 胜利演出参数（新增）
    QElapsedTimer m_victoryTimer;
    int m_victoryState = 0;

    // 玩家死亡演出参数（新增）
    PlayerDeathState m_playerDeathState = PlayerDeathState::NotStarted;
    QElapsedTimer m_playerDeathTimer;
    static constexpr int PLAYER_DEATH_FADE_OUT = 500;       // 玩家淡出时间
    static constexpr int PLAYER_DEATH_IMAGE_FADE_IN = 1000; // 图片淡入时间
    static constexpr int PLAYER_DEATH_WAIT = 1000;          // 等待时间
    static constexpr int PLAYER_DEATH_BLACK_FADE = 1500;    // 黑屏淡出时间
};

#endif // GAMESCENE_H
