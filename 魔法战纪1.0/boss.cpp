#include "boss.h"
#include "bullet.h"
#include <QtMath>
#include <QRandomGenerator>

Boss::Boss(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    QPixmap pixmap("D:/qt/MyGame/images/test_boss.png");
    setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setPos(640, 640);
    setZValue(0);

    m_orbSkillTimer.start();
    m_homingSkillTimer.start();
    m_spreadTimer.start();
    m_dashSkillTimer.start();

    // 初始化技能冷却计时器（新增）
    // 用途：控制三个技能轮流释放的间隔
    m_skillCooldownTimer.start();

    // 启动技能执行计时器（新增）
    // 用途：确保 restart() 时计时器是 valid 状态，避免未定义行为
    // 为什么：QElapsedTimer 默认构造后是 invalid，直接调用 restart() 是未定义行为
    m_orbExecTimer.start();
    m_homingExecTimer.start();
    m_spreadExecTimer.start();
}

void Boss::moveTowardsPlayer(qreal playerX, qreal playerY, const QRectF &validArea)
{
    if (m_isDashing) return;

    qreal bossX = getCenterX();
    qreal bossY = getCenterY();

    qreal dx = playerX - bossX;
    qreal dy = playerY - bossY;
    qreal len = qSqrt(dx * dx + dy * dy);

    if (len < 0.001) return;

    dx = dx / len * MOVE_SPEED;
    dy = dy / len * MOVE_SPEED;

    qreal newX = pos().x() + dx;
    qreal newY = pos().y() + dy;

    qreal halfW = pixmap().width() / 2;
    qreal halfH = pixmap().height() / 2;

    newX = qBound(validArea.left() + halfW, newX, validArea.right() - halfW);
    newY = qBound(validArea.top() + halfH, newY, validArea.bottom() - halfH);

    setPos(newX, newY);
}

bool Boss::shouldSpawnDashSkill() const
{
    return !m_isDashing && m_dashSkillTimer.elapsed() >= DASH_SKILL_INTERVAL;
}

QList<Bullet*> Boss::updateDashState(qreal playerX, qreal playerY, const QRectF &validArea)
{
    QList<Bullet*> newBullets;

    if (!m_isDashing && m_dashSkillTimer.elapsed() >= DASH_SKILL_INTERVAL) {
        m_isDashing = true;
        m_dashPhaseTimer.restart();
        m_dashSkillTimer.restart();
        m_dashMidFired = false;
        m_dashEndFired = false;

        qreal bossX = getCenterX();
        qreal bossY = getCenterY();

        m_dashStartX = bossX;
        m_dashStartY = bossY;

        qreal dx = playerX - bossX;
        qreal dy = playerY - bossY;
        qreal len = qSqrt(dx * dx + dy * dy);

        if (len < 0.001) {
            m_dashDirX = 1;
            m_dashDirY = 0;
        } else {
            m_dashDirX = dx / len;
            m_dashDirY = dy / len;
        }

        newBullets.append(spawnRingBullets(bossX, bossY, DASH_BULLET_COUNT, DASH_BULLET_SPEED, DASH_BULLET_SIZE));
    }

    if (!m_isDashing) return newBullets;

    qreal elapsed = m_dashPhaseTimer.elapsed();
    qreal bossX = getCenterX();
    qreal bossY = getCenterY();

    qreal moveX = m_dashDirX * DASH_SPEED;
    qreal moveY = m_dashDirY * DASH_SPEED;

    qreal newX = pos().x() + moveX;
    qreal newY = pos().y() + moveY;

    qreal halfW = pixmap().width() / 2;
    qreal halfH = pixmap().height() / 2;

    newX = qBound(validArea.left() + halfW, newX, validArea.right() - halfW);
    newY = qBound(validArea.top() + halfH, newY, validArea.bottom() - halfH);

    setPos(newX, newY);

    if (!m_dashMidFired && elapsed >= DASH_DURATION / 2) {
        m_dashMidFired = true;
        qreal midX = getCenterX();
        qreal midY = getCenterY();
        newBullets.append(spawnRingBullets(midX, midY, DASH_BULLET_COUNT, DASH_BULLET_SPEED, DASH_BULLET_SIZE));
    }

    if (elapsed >= DASH_DURATION) {
        m_isDashing = false;
        qreal endX = getCenterX();
        qreal endY = getCenterY();
        newBullets.append(spawnRingBullets(endX, endY, DASH_BULLET_COUNT, DASH_BULLET_SPEED, DASH_BULLET_SIZE));
    }

    return newBullets;
}

bool Boss::isDashing() const
{
    return m_isDashing;
}

QList<Bullet*> Boss::spawnRingBullets(qreal centerX, qreal centerY, int count, int speed, int size)
{
    QList<Bullet*> bullets;

    for (int i = 0; i < count; ++i) {
        qreal angle = i * (2.0 * M_PI / count);
        Bullet *bullet = Bullet::createBossSpreadBullet(
            centerX - size / 2.0,
            centerY - size / 2.0,
            angle,
            speed,
            size
            );
        bullets.append(bullet);
    }

    return bullets;
}

bool Boss::shouldSpawnOrbitSkill() const
{
    // 检查触发标志：如果标志为 true，说明技能被选中需要触发
    // 用途：技能状态系统通过标志位强制触发技能，而非操作计时器
    if (m_orbTrigger) return true;
    // 否则按正常计时器判断（自动释放模式）
    return m_orbSkillTimer.elapsed() >= ORB_SKILL_INTERVAL;
}

QList<Bullet*> Boss::spawnOrbitingBullets()
{
    QList<Bullet*> bullets;
    qreal bossX = getCenterX();
    qreal bossY = getCenterY();

    // 清除触发标志（如果是由技能状态系统触发的）
    // 用途：防止技能被重复触发
    m_orbTrigger = false;

    m_orbSkillTimer.restart();

    for (int i = 0; i < ORB_BULLET_COUNT; ++i) {
        qreal angle = i * (M_PI / 2.0);
        Bullet *bullet = Bullet::createBossOrbitingBullet(bossX, bossY, angle, ORB_RADIUS, ORB_BULLET_SIZE);

        // 设置裂变参数（新增）
        // 用途：让环绕子弹在飞行一定距离后裂变出一圈新子弹
        bullet->setFissionParams(ORB_FISSION_DISTANCE, ORB_FISSION_COUNT, ORB_FISSION_SPEED, ORB_FISSION_SIZE);

        bullets.append(bullet);
    }

    return bullets;
}

bool Boss::shouldSpawnHomingSkill() const
{
    // 检查触发标志
    if (m_homingTrigger) return true;
    return m_homingSkillTimer.elapsed() >= HOMING_SKILL_INTERVAL;
}

QList<Bullet*> Boss::spawnHomingBullets(qreal playerX, qreal playerY)
{
    Q_UNUSED(playerX);
    Q_UNUSED(playerY);

    QList<Bullet*> bullets;
    qreal bossX = getCenterX();
    qreal bossY = getCenterY();

    // 清除触发标志
    m_homingTrigger = false;

    m_homingSkillTimer.restart();

    for (int i = 0; i < HOMING_BULLET_COUNT; ++i) {
        qreal angle = i * (2.0 * M_PI / HOMING_BULLET_COUNT);
        qreal dx = qCos(angle);
        qreal dy = qSin(angle);

        qreal spawnX = bossX + dx * 60 - HOMING_BULLET_SIZE / 2.0;
        qreal spawnY = bossY + dy * 60 - HOMING_BULLET_SIZE / 2.0;

        Bullet *bullet = Bullet::createBossHomingBullet(spawnX, spawnY, dx, dy, HOMING_BULLET_SPEED, HOMING_BULLET_SIZE);
        bullets.append(bullet);
    }

    return bullets;
}

bool Boss::shouldSpawnSpreadSkill() const
{
    // 检查触发标志
    if (m_spreadTrigger) return true;
    return m_spreadTimer.elapsed() >= SPREAD_INTERVAL;
}

Bullet* Boss::getNextSpreadBullet(qreal playerX, qreal playerY)
{
    // 清除触发标志（如果是由技能状态系统触发的）
    if (m_spreadTrigger) {
        m_spreadTrigger = false;
    }

    if (!m_spreadActive && m_spreadTimer.elapsed() >= SPREAD_INTERVAL) {
        m_spreadActive = true;
        m_spreadShotCount = 0;
        m_spreadShotTimer.restart();
        m_spreadTimer.restart();
    }

    if (!m_spreadActive) return nullptr;

    if (m_spreadShotCount >= SPREAD_SHOT_COUNT) {
        m_spreadActive = false;
        return nullptr;
    }

    if (m_spreadShotTimer.elapsed() < SPREAD_SHOT_INTERVAL) {
        return nullptr;
    }

    m_spreadShotCount++;
    m_spreadShotTimer.restart();

    qreal bossX = getCenterX();
    qreal bossY = getCenterY();
    qreal dx = playerX - bossX;
    qreal dy = playerY - bossY;
    qreal baseAngle = qAtan2(dy, dx);

    qreal randomOffset = (QRandomGenerator::global()->generateDouble() - 0.5) * SPREAD_ANGLE_RANGE;
    qreal finalAngle = baseAngle + randomOffset;

    qreal spawnX = bossX - SPREAD_BULLET_SIZE / 2.0;
    qreal spawnY = bossY - SPREAD_BULLET_SIZE / 2.0;

    return Bullet::createBossSpreadBullet(spawnX, spawnY, finalAngle, SPREAD_BULLET_SPEED, SPREAD_BULLET_SIZE);
}

bool Boss::isSpreadActive() const
{
    return m_spreadActive;
}

qreal Boss::getCenterX() const
{
    return pos().x() + pixmap().width() / 2;
}

qreal Boss::getCenterY() const
{
    return pos().y() + pixmap().height() / 2;
}

int Boss::getHp() const { return m_hp; }
int Boss::getMaxHp() const { return m_maxHp; }

void Boss::setHp(int hp)
{
    m_hp = qBound(0, hp, m_maxHp);
}

void Boss::takeDamage(int damage)
{
    setHp(m_hp - damage);
}

// ========== 技能状态系统实现（新增）==========
// 用途：实现三个技能（环绕、追踪、扇形）轮流随机释放，每次只放一个，放完才能放下一个

Boss::SkillState Boss::getCurrentSkillState() const
{
    return m_currentSkill;
}

bool Boss::isAnySkillActive() const
{
    return m_currentSkill != SkillState::Idle;
}

// updateSkillState: 更新当前技能状态
// 用途：检测当前技能是否完成，完成后切换到Idle状态并启动冷却计时器
// 修复：使用独立的执行计时器判断，避免 spawn 函数 restart 间隔计时器导致判断失效
// 修复：缩短等待时间，让技能切换更流畅
void Boss::updateSkillState()
{
    switch (m_currentSkill) {
    case SkillState::Orbiting:
        // 环绕技能完成条件：从技能开始执行后等待足够时间
        // 为什么用 m_orbExecTimer：spawnOrbitingBullets() 会 restart m_orbSkillTimer（间隔计时器）
        //                        导致无法判断技能是否执行完毕
        //                        m_orbExecTimer 只在 pickNextSkill 时启动一次，不被 restart
        // 时间设置：LAUNCH_INTERVAL=700ms，4颗子弹全部发射需要约 3*700=2100ms
        //          加上子弹飞行时间约 2-3 秒，总共约 5 秒
        // 但为了测试，缩短到 3 秒，确保技能能正常切换
        if (m_orbExecTimer.elapsed() >= 3000) {
            m_currentSkill = SkillState::Idle;
            m_skillCooldownTimer.restart();
        }
        break;

    case SkillState::Homing:
        // 追踪技能完成条件：追踪子弹生命周期结束
        // 为什么用 m_homingExecTimer：同理，spawnHomingBullets() 会 restart m_homingSkillTimer
        // HOMING_LIFE_TIME = 4000ms，加上 1 秒保险
        if (m_homingExecTimer.elapsed() >= HOMING_LIFE_TIME + 500) {  // 缩短500ms，加快切换
            m_currentSkill = SkillState::Idle;
            m_skillCooldownTimer.restart();
        }
        break;

    case SkillState::Spread:
        // 扇形散射完成条件：散射结束（m_spreadActive为false）且执行时间足够
        // 为什么用 m_spreadExecTimer：虽然 spread 用 m_spreadActive 判断，但加上时间保险
        // SPREAD_SHOT_COUNT=10，每发间隔 80ms，总共约 800ms，加上 1 秒保险
        if (!m_spreadActive && m_spreadExecTimer.elapsed() >= 3500) {  // 延长到3.5秒，匹配更多子弹的发射时间
            m_currentSkill = SkillState::Idle;
            m_skillCooldownTimer.restart();
        }
        break;

    case SkillState::Idle:
        // Idle状态不需要更新
        break;
    }
}

// pickNextSkill: 随机选择下一个技能
// 用途：从三个技能中随机选择一个，但只有在Idle状态且冷却完成后才能选择
// 机制：设置对应技能的触发标志为 true，shouldSpawn 函数检查此标志来触发技能
// 新增：启动对应的执行计时器，用于判断技能何时完成
void Boss::pickNextSkill()
{
    if (m_currentSkill != SkillState::Idle) return;
    if (m_skillCooldownTimer.elapsed() < SKILL_COOLDOWN_MS) return;

    // 随机选择三个技能之一：0=环绕, 1=追踪, 2=扇形
    int skillIndex = QRandomGenerator::global()->bounded(3);

    switch (skillIndex) {
    case 0:
        m_currentSkill = SkillState::Orbiting;
        m_orbTrigger = true;  // 设置触发标志
        m_orbExecTimer.restart();  // 启动执行计时器（新增）
        break;
    case 1:
        m_currentSkill = SkillState::Homing;
        m_homingTrigger = true;  // 设置触发标志
        m_homingExecTimer.restart();  // 启动执行计时器（新增）
        break;
    case 2:
        m_currentSkill = SkillState::Spread;
        m_spreadTrigger = true;  // 设置触发标志
        m_spreadExecTimer.restart();  // 启动执行计时器（新增）
        break;
    }
}
