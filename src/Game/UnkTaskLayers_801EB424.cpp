/* The root of the task tree and its layers (class name unknown; fn_8001C4F8 returns the object, a
 * function-local static): every task hangs below one of nineteen named layer tasks. The system layers
 * hang below the root and the game layers below the "Scene" layer, which a stage reset (fn_801EB314)
 * empties and builds again. No RTTI names the class; the file is named after its constructor. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* Kills a task's children, and runs a task tree for dt (behaviour inferred). */
void fn_8004EE88(Task *task);
void fn_8004EC68(Task *task, float dt);
}

/* Whether the game is paused for the camera code (set here, read by Game/CameraManagerUnit.cpp and
 * the effects; meaning inferred). */
extern u8 lbl_805EE1DC;

/* The layer names, then the layers built below the root and below the "Scene" layer. */
static const char *s_layerNames[] = {
    "System",   "Audio",   "Debug",   "Scene",   "PostSystem", "Render",      "Manager",
    "Controller", "Command", "CharaColli", "Landscape", "Gadget",   "Vehicle",     "Player",
    "Enemy",    "PostManager", "Particle", "Editor", "Camera",
};
static int s_systemLayers[] = {0, 1, 2, 3, 4, 5};
static int s_gameLayers[] = {6, 7, 8, 9, 10, 11, 12, 14, 13, 15, 16, 17, 18};

/* The root of the task tree (class name unknown). */
class UnkTaskLayers_801EB424 {
  public:
    UnkTaskLayers_801EB424();
    ~UnkTaskLayers_801EB424();

    /* Builds the system layers, and the game layers (names inferred). */
    void InitSystemLayers();
    void InitGameLayers();

    /* 0x00 */ Task *m_root;
    /* 0x04 */ Task *m_layers[19];
};
SIZE_ASSERT(UnkTaskLayers_801EB424, 0x50);

UnkTaskLayers_801EB424::UnkTaskLayers_801EB424() {
    m_root = new Task(0);
    InitSystemLayers();
    InitGameLayers();
}

UnkTaskLayers_801EB424::~UnkTaskLayers_801EB424() {
    delete m_root;
    m_root = 0;
    for (int i = 0; i < 19; i++) {
        m_layers[i] = 0;
    }
}

extern "C" {

/* Runs the task tree for dt (name inferred). */
void fn_801EB370(UnkTaskLayers_801EB424 *layers, float dt) {
    if (layers->m_root != 0) {
        fn_8004EC68(layers->m_root, dt);
    }
}

void fn_801EB364(void) {
    lbl_805EE1DC = 1;
}

void fn_801EB358(void) {
    lbl_805EE1DC = 0;
}

/* Empties the "Scene" layer and builds the game layers again (name inferred). */
void fn_801EB314(UnkTaskLayers_801EB424 *layers) {
    fn_8004EE88(layers->m_layers[3]);
    fn_8004EC68(layers->m_layers[3], 0.0f);
    layers->InitGameLayers();
}

/* The task of a layer (name inferred). */
Task *fn_801EB304(UnkTaskLayers_801EB424 *layers, int layer) {
    return layers->m_layers[layer];
}

/* Kills the tasks of the "Gadget", "Vehicle" and "Enemy" layers (name inferred). */
void fn_801EB2C4(UnkTaskLayers_801EB424 *layers) {
    fn_8004EE88(layers->m_layers[11]);
    fn_8004EE88(layers->m_layers[12]);
    fn_8004EE88(layers->m_layers[14]);
}

}

void UnkTaskLayers_801EB424::InitSystemLayers() {
    for (int i = 0; i < 6; i++) {
        int layer = s_systemLayers[i];
        m_layers[layer] = new Task(m_root);
        m_layers[layer]->m_flags |= 0x100;
        m_layers[layer]->m_name = s_layerNames[layer];
    }
}

void UnkTaskLayers_801EB424::InitGameLayers() {
    for (int i = 0; i < 13; i++) {
        int layer = s_gameLayers[i];
        m_layers[layer] = new Task(m_layers[3]);
        m_layers[layer]->m_flags |= 0x100;
        m_layers[layer]->m_name = s_layerNames[layer];
    }
}
