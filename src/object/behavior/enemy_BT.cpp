#include "object/enemy/enemy_BT.h"
#include "object/enemy/enemy.h"
#include "object/behavior_tree/selector.h"
#include "object/behavior_tree/sequence.h"
#include "object/behavior_tree/behavior/Bpatrol.h"
#include "object/behavior_tree/behavior/Bfind_target.h"
#include "object/behavior_tree/behavior/Bmove_to_target.h"
#include "object/behavior_tree/behavior/Bcheck_in_attack_range.h"
#include "object/behavior_tree/behavior/Benemy_attack.h"
#include "object/behavior_tree/behavior/Benemy_hit.h"
#include "object/behavior_tree/behavior/Bdeath.h"
#include "object/behavior_tree/behavior/Bcheck_origin_position.h"

EnemyBT::EnemyBT(Enemy& enemy, std::vector<glm::vec3> &wayPoints, GameObject &target) : m_enemy(enemy), m_wayPoints(wayPoints), m_target(target)
{
    m_speed = 2.0f;
    m_radius = 8.0f;
    m_attackRange = 1.5f;
}

std::unique_ptr<BTNode> EnemyBT::SetupTree()
{
    std::vector<std::unique_ptr<BTNode>> children;

    // add death behavior
    children.push_back(std::make_unique<Death>(m_enemy));

    // add hit behavior
    children.push_back(std::make_unique<EnemyHit>(m_enemy));

    // add attack behavior
    auto attackSeq = std::vector<std::unique_ptr<BTNode>>();
    attackSeq.push_back(std::make_unique<CheckInAttackRange>(m_enemy, m_attackRange));
    attackSeq.push_back(std::make_unique<EnemyAttack>(m_enemy));
    children.push_back(std::make_unique<Sequence>(std::move(attackSeq)));

    // add find and move to target
    auto findTargetSeq = std::vector<std::unique_ptr<BTNode>>();
    findTargetSeq.push_back(std::make_unique<FindTarget>(m_enemy, m_target, m_radius));
    findTargetSeq.push_back(std::make_unique<MoveToTarget>(m_enemy, m_target, m_speed * 3.0f));
    children.push_back(std::make_unique<Sequence>(std::move(findTargetSeq)));
    // add patrol
    if(m_wayPoints.size())
        children.push_back(std::make_unique<Patrol>(m_enemy, m_wayPoints, m_speed));
    else
        children.push_back(std::make_unique<CheckOriginPosition>(m_enemy, m_enemy.transform.GetLocalPosition(), m_enemy.transform.GetLocalRotation(), m_speed * 3.0f));

    return std::make_unique<Selector>(std::move(children));
}