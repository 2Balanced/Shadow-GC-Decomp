/* NpcNameDisplay (class name from RTTI): the name plate of a character met in a mission. It loads
 * the character's CSD project ("CsdFiles/Mchara_<name>"), plays its "MissionCharacters" scene and
 * draws it through a Render. Only one exists at a time; fn_80204240 creates it for a character and
 * fn_80204214 closes it (other units call them under those names). File name from the class;
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/FogTable.h"
#include "Game/PjsCsd.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" char *strcpy(char *dst, const char *src);

class UnkFileSystem_800371D8;

extern "C" {
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The path of a file in the CSD archive (behaviour inferred; see Game/TGi.h). */
void *fn_8003F8D4(void);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
/* The file system, and a load request to it (see Game/FogTable.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, FogRequest *request, int arg2);
void fn_80049B34(void *data);
/* The task registry, and adding and removing a named task (see Game/PjsEventActionManager.cpp). */
void *fn_80014B90(void);
void fn_8025D144(void *registry, Task *task);
void fn_8025D1C4(void *registry, Task *task, int arg2, const char *name);
}

/* The characters' CSD projects (name inferred). */
static const char *s_files[20] = {
    0,
    "CsdFiles/Mchara_sonic",
    "CsdFiles/Mchara_tails",
    "CsdFiles/Mchara_knuckles",
    "CsdFiles/MChara_amy",
    "CsdFiles/Mchara_rouge",
    "CsdFiles/Mchara_omega",
    "CsdFiles/Mchara_vector",
    "CsdFiles/Mchara_espio",
    "CsdFiles/Mchara_maria",
    "CsdFiles/MChara_charmy",
    "CsdFiles/Mchara_eggman",
    "CsdFiles/MChara_doomeye",
};

/* The load request (name inferred). */
static FogRequest s_request;

class NpcNameDisplay : public Task, public TPjsCsd {
  public:
    NpcNameDisplay(Task *parent, int character);
    virtual ~NpcNameDisplay();
    virtual void Tick(float dt);

    /* Starts loading character's project (name inferred). */
    void Load(int character);
    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Called by the file system with the project's data (name inferred). */
    static void OnLoaded(FogRequest *request);

    /* The plate being shown, if any (name inferred). */
    static NpcNameDisplay *s_instance;

    /* 0x7C */ s32 m_state;  /* 1 loading, 2 shown */
    /* 0x80 */ u8 m_closing; /* (meaning inferred) */
    /* 0x84 */ boost::shared_ptr<Render> m_render;
    /* 0x8C */ u8 field_0x8C[0x90 - 0x8C];
};
SIZE_ASSERT(NpcNameDisplay, 0x90);

NpcNameDisplay *NpcNameDisplay::s_instance;

/* Shows the name plate of character unless one is shown (name inferred). */
extern "C" bool fn_80204240(int character) {
    if (NpcNameDisplay::s_instance == 0) {
        new NpcNameDisplay(fn_801EB304(fn_8001C4F8(), 3), character);
        return true;
    }
    return false;
}

/* Closes the name plate (name inferred). */
extern "C" bool fn_80204214(void) {
    if (NpcNameDisplay::s_instance != 0) {
        NpcNameDisplay::s_instance->m_closing = 1;
        NpcNameDisplay::s_instance = 0;
        return true;
    }
    return false;
}

NpcNameDisplay::NpcNameDisplay(Task *parent, int character) : Task(parent) {
    m_name = "NpcNameDisplay";
    s_instance = this;
    m_state = 1;
    m_closing = 0;
    Load(character);
    m_render = fn_8004E24C(fn_80046178()->field_0x38);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&NpcNameDisplay::Draw, this, _1));
    fn_8025D1C4(fn_80014B90(), this, 1, "NpcName");
}

NpcNameDisplay::~NpcNameDisplay() {
    if (s_instance == this) {
        s_instance = 0;
    }
    fn_8025D144(fn_80014B90(), this);
}

void NpcNameDisplay::Tick(float dt) {
    switch (m_state) {
    case 1:
        break;
    case 2:
        if (m_closing) {
            m_flags |= 1;
        } else {
            fn_8003FE2C(*this);
        }
        break;
    }
}

void NpcNameDisplay::Draw(RednerDrawArgs &args) {
    if (m_state == 2) {
        fn_8003FDF4(*this);
    }
}

void NpcNameDisplay::Load(int character) {
    s_request.m_path = fn_800423B8(fn_8003F8D4(), s_files[character], 0);
    s_request.m_data = 0;
    s_request.m_size = 0;
    s_request.m_user = this;
    s_request.m_callback = OnLoaded;
    fn_80045044(fn_800371D8(), &s_request, 1);
    m_state = 1;
}

void NpcNameDisplay::OnLoaded(FogRequest *request) {
    NpcNameDisplay *display = (NpcNameDisplay *)request->m_user;
    fn_8003E750(*display, request->m_data, request->m_size);
    fn_80049B34(request->m_data);
    strcpy(display->m_sceneName, "MissionCharacters");
    fn_8003E414(*display, 0, 1);
    fn_8003FB10(*display);
    display->m_state = 2;
}
