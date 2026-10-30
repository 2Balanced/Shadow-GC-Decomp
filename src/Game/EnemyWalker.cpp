/* EnemyWalker (class name from RTTI; Game/EnemyWalker.h): the body of the walking enemies, an
 * EnemyWalkerCommon with nine pure virtual functions that EggPawn and GunRobot implement. This
 * unit holds its constructor and destructor and emits its vtable and the RTTI of its bases. */
#include "Game/EnemyWalker.h"

EnemyWalker::EnemyWalker(int type) : EnemyWalkerCommon(type) {
    m_enemy = this;
}

EnemyWalker::~EnemyWalker() {}
