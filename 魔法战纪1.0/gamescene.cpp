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

    m_boss = new Boss();
    addItem(m_boss);

    m_validArea = QRectF(100, 100, 1080, 1080);
}

QRectF GameScene::getValidArea() const
{
    return m_validArea;
}

void GameScene::setupBackground()
{
    m_background.load("D:/qt/MyGame/images/text.jpg");
    setBackgroundBrush(m_background);
}

void GameScene::processShoot()
{
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
    if (!m_player->usePotion()) {
        return;
    }
}

void GameScene::updateLockTarget()
{
    if (!m_player->isLocked()) {
        return;
    }
    // 一阶段Boss已死亡，锁定二阶段Boss（如果有）
    if (m_boss) {
        m_player->setLockTarget(m_boss->getCenterX(), m_boss->getCenterY());
    } else if (m_bossPhase2) {
        m_player->setLockTarget(m_bossPhase2->getCenterX(), m_bossPhase2->getCenterY());
    }
}
void GameScene::checkPhaseTransition()
{
    if (m_phaseTransitionDone) return;
    if (m_currentPhase != 1) return;
    if (!m_boss || m_boss->getHp() > 0) return;

    m_phaseTransitionDone = true;
    m_currentPhase = 2;

    // 清理一阶段Boss
    removeItem(m_boss);
    delete m_boss;
    m_boss = nullptr;

    // 清理一阶段遗留子弹
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

    // 创建二阶段Boss
    m_bossPhase2 = new BossPhase2();
    addItem(m_bossPhase2);

    if (m_bossPhase2->getWarningRect()) {
        addItem(m_bossPhase2->getWarningRect());
    }
    if (m_bossPhase2->getWarningCircle()) {
        addItem(m_bossPhase2->getWarningCircle());
    }

    emit phaseChanged(2);
    emit bossHpChanged(m_bossPhase2->getHp(), m_bossPhase2->getMaxHp());
}

BossPhase2* GameScene::getBossPhase2() const
{
    return m_bossPhase2;
}


// updateBossSkillState: 更新一阶段Boss技能状态机
// 用途：控制Boss随机释放技能，每次只放一个，放完才能放下一个
void GameScene::updateBossSkillState()
{
    if (!m_boss) return;

    // 更新技能状态（检查当前技能是否完成）
    m_boss->updateSkillState();

    // 如果没有活跃技能，随机选一个新技能
    if (!m_boss->isAnySkillActive()) {
        m_boss->pickNextSkill();
    }
}


// updateBossSkillState: 更新一阶段Boss技能状态
// 用途：控制三个技能（环绕、追踪、扇形）轮流随机释放，每次只放一个，放完才能放下一个

void GameScene::updateBossDash()
{
    // 一阶段Boss已死亡，不更新冲刺
    if (!m_boss) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();

    // 只有在Idle状态才允许生成新的冲刺子弹（技能状态机控制）
    // 用途：防止冲刺技能与其他技能同时释放
    // 为什么：把"生成新子弹"和"更新已有子弹"分离，冲刺结束后已有子弹继续飞行
    QList<Bullet*> newBullets;
    if (m_boss->getCurrentSkillState() == Boss::SkillState::Idle) {
        newBullets = m_boss->updateDashState(playerX, playerY, m_validArea);
    }

    for (Bullet *bullet : newBullets) {
        m_bossDashBullets.append(bullet);
        addItem(bullet);
    }

    // 更新所有已有的冲刺子弹（无论冲刺是否结束，子弹都要继续飞行）
    // 用途：冲刺结束后，场上已有的子弹继续移动和碰撞检测
    // 为什么：之前技能状态检查阻止了所有子弹更新，导致冲刺结束后子弹冻结在场上
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
    // 一阶段Boss已死亡，不更新环绕子弹
    if (!m_boss) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();

    // 只有在环绕技能状态才生成新的环绕子弹（技能状态机控制）
    // 用途：防止环绕技能与其他技能同时释放
    // 为什么：把"生成新子弹"和"更新已有子弹"分离，技能结束后已有子弹继续飞行
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

    // 更新所有已有的环绕子弹（无论技能是否结束，子弹都要继续飞行）
    // 用途：技能结束后，场上已有的子弹继续移动和碰撞检测
    // 为什么：之前技能状态检查阻止了所有子弹更新，导致技能结束后子弹冻结在场上
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

// updateBossPhase2Finale: 更新二阶段Boss终结技
// 用途：处理Boss终结技的预警、现身、子弹发射，以及终结技完成后生成四个角落的分身
// spawnBossClones: 在地图四个角落生成Boss分身
// 用途：终结技完成后，在左上、右上、左下、右下四个位置生成分身
// 分身特性：固定不动、无法被攻击、定期发射环形弹幕
void GameScene::spawnBossClones()
{
    if (m_clonesSpawned) return;
    m_clonesSpawned = true;

    // 计算四个角落的位置（在有效区域内）
    qreal margin = 80;  // 距离边界的边距
    qreal left = m_validArea.left() + margin;
    qreal right = m_validArea.right() - margin;
    qreal top = m_validArea.top() + margin;
    qreal bottom = m_validArea.bottom() - margin;

    // 四个角落：左上、右上、左下、右下
    QPointF positions[4] = {
        QPointF(left, top),      // 左上
        QPointF(right, top),     // 右上
        QPointF(left, bottom),   // 左下
        QPointF(right, bottom)   // 右下
    };

    for (int i = 0; i < 4; ++i) {
        BossClone *clone = new BossClone(positions[i].x(), positions[i].y());
        m_bossClones.append(clone);
        addItem(clone);
    }
}

// updateBossClones: 更新所有Boss分身的发射状态
// 用途：终结技完成后，四个角落的分身定期发射环形弹幕
// 为什么：之前声明了但没有实现，导致分身生成后完全不攻击
void GameScene::updateBossClones()
{
    if (m_bossClones.isEmpty()) return;

    for (BossClone *clone : m_bossClones) {
        QList<Bullet*> newBullets = clone->updateShoot();
        for (Bullet *bullet : newBullets) {
            m_phase2Bullets.append(bullet);
            addItem(bullet);
        }
    }
}

void GameScene::updateBossPhase2Finale()
{
    if (!m_bossPhase2) return;

    // 如果终结技已完成，检查是否需要生成分身
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

    // 检测终结技能的范围伤害（Boss现身瞬间）
    // 如果玩家在预警范围内，造成伤害
    if (m_bossPhase2->getWarningCircle() &&
        !m_bossPhase2->getWarningCircle()->isVisible() &&
        m_bossPhase2->getOpacity() > 0.5) {
        // 圆形预警刚消失且Boss可见，说明刚现身
        qreal dx = m_bossPhase2->getCenterX() - playerX;
        qreal dy = m_bossPhase2->getCenterY() - playerY;
        qreal dist = qSqrt(dx * dx + dy * dy);
        if (dist <= 200 + playerRadius) {  // FINALE_RADIUS
            m_player->takeDamage(50);  // 高伤害
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());
        }
    }
}


void GameScene::updateFissionBullets()
{
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
    // 一阶段Boss已死亡，不更新追踪子弹
    if (!m_boss) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();

    // 只有在追踪技能状态才生成新的追踪子弹（技能状态机控制）
    // 用途：防止追踪技能与其他技能同时释放
    // 为什么：把"生成新子弹"和"更新已有子弹"分离，技能结束后已有子弹继续飞行
    if (m_boss->getCurrentSkillState() == Boss::SkillState::Homing) {
        if (m_boss->shouldSpawnHomingSkill()) {
            QList<Bullet*> newBullets = m_boss->spawnHomingBullets(playerX, playerY);
            for (Bullet *bullet : newBullets) {
                m_bossHomingBullets.append(bullet);
                addItem(bullet);
            }
        }
    }

    // 更新所有已有的追踪子弹（无论技能是否结束，子弹都要继续飞行）
    // 用途：技能结束后，场上已有的子弹继续移动和碰撞检测
    // 为什么：之前技能状态检查阻止了所有子弹更新，导致技能结束后子弹冻结在场上
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
    // 一阶段Boss已死亡，不更新扇形散射
    if (!m_boss) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();

    // 只有在散射技能状态才生成新的散射子弹（技能状态机控制）
    // 用途：防止散射技能与其他技能同时释放
    // 为什么：把"生成新子弹"和"更新已有子弹"分离，技能结束后已有子弹继续飞行
    if (m_boss->getCurrentSkillState() == Boss::SkillState::Spread) {
        Bullet *bullet = m_boss->getNextSpreadBullet(playerX, playerY);

        if (bullet) {
            m_bossSpreadBullets.append(bullet);
            addItem(bullet);
        }
    }

    // 更新所有已有的散射子弹（无论技能是否结束，子弹都要继续飞行）
    // 用途：技能结束后，场上已有的子弹继续移动和碰撞检测
    // 为什么：之前技能状态检查阻止了所有子弹更新，导致技能结束后子弹冻结在场上
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
    if (!m_bossPhase2) return;

    qreal playerX = m_player->getCenterX();
    qreal playerY = m_player->getCenterY();
    qreal playerRadius = m_player->pixmap().width() / 2;

    // 更新Boss冲刺状态
    // 用途：处理二阶段Boss的边界冲刺技能（3次冲刺）
    if (!m_bossPhase2->isSkillComplete()) {
        QList<Bullet*> newBullets = m_bossPhase2->updateBoundaryDash(playerX, playerY, m_validArea);

        for (Bullet *bullet : newBullets) {
            m_phase2Bullets.append(bullet);
            addItem(bullet);
        }
    } else {
        // 技能完成后，Boss进入环形发射模式（新增）
        // 用途：冲刺和终结技都完成后，Boss定期向周围发射高密度慢速弹幕
        // 为什么：之前调用丢失了，导致二阶段本体完全不攻击
        QList<Bullet*> ringBullets = m_bossPhase2->updateRingShoot();
        for (Bullet *bullet : ringBullets) {
            m_phase2Bullets.append(bullet);
            addItem(bullet);
        }
    }

    // 更新预警矩形状态（新增）
    // 用途：更新冲刺预警矩形的淡入/闪烁动画
    m_bossPhase2->updateWarningRect();

    // 碰撞检测...
    if (m_bossPhase2->isBoundaryDashing() && m_bossPhase2->getOpacity() > 0.5) {
        if (m_bossPhase2->checkDashCollision(playerX, playerY, playerRadius)) {
            m_player->takeDamage(35);
            emit playerHpChanged(m_player->getHp(), m_player->getMaxHp());
        }
    }


    // 更新子弹...
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
            // 触发裂变（如果可裂变且未裂变过）
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

        // 裂变距离检测
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

void GameScene::updateBullets()
{
    for (Bullet *bullet : m_bullets) {
        bullet->update();
    }
}

void GameScene::checkBulletCollisions()
{
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        Bullet *bullet = m_bullets[i];
        const QList<QGraphicsItem*> collisions = bullet->collidingItems();
        bool hit = false;
        for (QGraphicsItem *item : collisions) {
            // 一阶段Boss存在时检测与一阶段Boss的碰撞
            if (m_boss && item == m_boss) {
                m_boss->takeDamage(10);
                emit bossHpChanged(m_boss->getHp(), m_boss->getMaxHp());
                hit = true;
                break;
            }
            // 二阶段Boss存在时检测与二阶段Boss的碰撞
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
