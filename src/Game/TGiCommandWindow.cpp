/* TGiCommandWindow (class name from RTTI): the heads-up display of the player's current weapon and
 * vehicle commands. Its state lives in a TGiCommandWindowImpl (class name from the RTTI of its draw
 * callback's bind_t) that plays the CSD project "CsdFiles/CommandWindow" (in two-player mode a
 * second one, "CsdFiles/2P_CommandWindow"): the scenes Window, Weapon, Car_BCom and CarMeter. A
 * TGiNumberDisplay per player prints the weapon's ammunition. The file name and the names marked
 * "(name inferred)" are not original; the functions other units call keep their fn_ names.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>

#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "Game/TGi.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The argument a Render passes to its draw callbacks (unused here). */
struct RednerDrawArgs;

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The path of a localized file (behaviour inferred). */
const char *fn_800C02CC(const char *name);
/* Loads a texture list file into resource (behaviour inferred). */
void fn_800422A8(Chao::CSD::CProject *resource, const char *file);
/* The player's command and vehicle kinds (behaviour inferred). */
int fn_8016EEC8(UnkPlayerInfo_80178B5C *player);
int fn_8016EED0(UnkPlayerInfo_80178B5C *player);
}

/* The patterns one command shows (name inferred): the icon and the button. */
struct TGiCommand {
    /* 0x0 */ s32 m_icon;
    /* 0x4 */ s32 m_button;
};

/* The state of a player's window (name inferred). */
struct TGiCommandState {
    /* 0x00 */ TGiCommand m_weapon;
    /* 0x08 */ TGiCommand m_car;
    /* 0x10 */ u8 m_open;
    /* 0x14 */ float m_time;
};

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the RTTI names and literals below reuse these copies). The .rodata is
 * kept whole (the constructor addresses it through one base), so it also holds the RTTI names of
 * the CSD and boost headers' inline classes, which this unit does not reconstruct. */
static const char *s_rodataPool[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, TGiCommandWindowImpl, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<TGiCommandWindowImpl *>, boost::arg<1>>>",
    "TGiCommandWindow",
    "icon_gun",
    "Car_BCom",
    "icon_car",
    "CarMeter",
    "Not inititalized m_eDealloctor\n",
    "csdRCObject.h",
    "DefaultAnim",
    "Memory allocation failure",
    "csdBase.h",
    "CsdFiles/CommandWindow",
    "CsdFiles/2P_CommnandWindow",
    "CsdFiles/2P_CommandWindow",
    "ad_hpasc1",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>",
    "Chao::CSD::CResourceBase<Chao::CSD::Project>",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "boost::detail::sp_counted_base_impl<TGiNumberDisplay *, boost::checked_deleter<TGiNumberDisplay>>",
    "boost::detail::sp_counted_base",
    "std::exception",
    "exception",
    "std::bad_alloc",
    "bad_alloc",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "boost::checked_deleter<TGiNumberDisplay>",
};

class TGiCommandWindowImpl {
  public:
    TGiCommandWindowImpl();
    ~TGiCommandWindowImpl();

    /* Puts project's scenes at their first frames (name inferred). */
    void InitProject(Chao::CSD::CProject *project);
    /* Advances the windows (name inferred). */
    void Tick(float dt);
    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Opens and closes player's window on project (name inferred). */
    void UpdateWindow(Chao::CSD::CProject *project, UnkPlayerInfo_80178B5C *player,
                      TGiCommandState *state);

    /* The weapon command player shows (name inferred). */
    TGiCommand GetWeaponCommand(UnkPlayerInfo_80178B5C *player);
    /* The vehicle command player shows (name inferred). */
    TGiCommand GetCarCommand(UnkPlayerInfo_80178B5C *player);

    /* 0x00 */ boost::shared_ptr<Render> m_render;
    /* 0x08 */ Chao::CSD::CProject *m_projects[2];
    /* 0x10 */ TGiCommandState m_states[2];
    /* 0x40 */ boost::shared_ptr<TGiNumberDisplay> m_numbers[2];
};

/* Deletes the impl (name inferred; see checked_delete below). */
inline void DeleteImpl(TGiCommandWindowImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<TGiCommandWindowImpl> out of line and the destructor at its
 * own place; this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<TGiCommandWindowImpl>(TGiCommandWindowImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

class TGiCommandWindow : public Task {
  public:
    TGiCommandWindow(Task *parent);
    virtual ~TGiCommandWindow();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<TGiCommandWindowImpl> m_impl;
};

SIZE_ASSERT(TGiCommandWindowImpl, 0x50);
SIZE_ASSERT(TGiCommandWindow, 0x2C);

TGiCommandWindowImpl::TGiCommandWindowImpl() {
    m_render = fn_8004E24C(fn_80046178()->field_0x54);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&TGiCommandWindowImpl::Draw, this, _1));

    m_projects[0] = new Chao::CSD::CProject();
    fn_8036E114(m_projects[0], fn_800423B8(fn_8003F8D4(), "CsdFiles/CommandWindow", 0));
    fn_8036D598(m_projects[0], 0);
    InitProject(m_projects[0]);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            (&m_states[i].m_weapon)[j].m_icon = 16;
            (&m_states[i].m_weapon)[j].m_button = 3;
        }
        m_states[i].m_open = 0;
    }

    m_projects[1] = 0;
    if (lbl_8057D6A0.m_twoPlayer) {
        m_projects[1] = new Chao::CSD::CProject();
        fn_8003F8D4();
        fn_800422A8(m_projects[0], fn_800C02CC("CsdFiles/2P_CommnandWindow"));
        fn_8036E114(m_projects[1], fn_800423B8(fn_8003F8D4(), "CsdFiles/2P_CommandWindow", 0));
        fn_8036D598(m_projects[1], 0);
        InitProject(m_projects[1]);
    }

    TGiColorSet colors = s_numberColors;
    for (int i = 0; i < 2; i++) {
        m_numbers[i] = boost::shared_ptr<TGiNumberDisplay>(new TGiNumberDisplay);
        fn_800BCEA8(m_numbers[i].get(), lbl_805EE68C, "ad_hpasc1");
        fn_800BCC54(m_numbers[i].get(), 64, 64, 16, 18, 4);
        fn_800BCB7C(m_numbers[i].get(), &colors);
        fn_800BCB2C(m_numbers[i].get(), -2.7f);
    }
}

TGiCommandWindowImpl::~TGiCommandWindowImpl() {
    for (int i = 0; i < 2; i++) {
        if (m_projects[i] != 0) {
            delete m_projects[i];
        }
    }
}

/* NonMatching: every instruction matches except the swap of r30 and r31. Retail keeps project in
 * r31 and the .rodata base in r30; GC/2.7 gives the base r31. Both are used about equally often,
 * and one more use of project in the source gives retail's order (the compilers break the tie
 * differently). Local copies, casts, a const parameter, an inline accessor, named string arrays and
 * using this did not change it. */
void TGiCommandWindowImpl::InitProject(Chao::CSD::CProject *project) {
    RCPtr<CScene> scene;

    scene = fn_8036D660(project, "Window");
    fn_80367238(scene.Get(), "Open");
    {
        CScene *s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "Open"));
    }
    fn_80367140(scene.Get(), -1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659CC(scene.Get(), 0);

    scene = fn_8036D660(project, "Weapon");
    fn_80367238(scene.Get(), "Open");
    {
        CScene *s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "Open"));
    }
    fn_80367140(scene.Get(), -1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659CC(scene.Get(), 0);

    scene = fn_8036D660(project, "Car_BCom");
    fn_80367238(scene.Get(), "Open");
    {
        CScene *s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "Open"));
    }
    fn_80367140(scene.Get(), -1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659CC(scene.Get(), 0);

    scene = fn_8036D660(project, "CarMeter");
    fn_80367238(scene.Get(), "DefaultAnim");
    fn_80367140(scene.Get(), 0.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659CC(scene.Get(), 0);
    fn_80365B40(scene.Get(), 1);
}

void TGiCommandWindowImpl::Tick(float dt) {
    m_states[0].m_time += dt;
    m_states[1].m_time += dt;
    if (lbl_8057D6A0.m_twoPlayer) {
        UpdateWindow(m_projects[1], fn_80178B5C(fn_80079808(), 0), &m_states[0]);
        UpdateWindow(m_projects[0], fn_80178B5C(fn_80079808(), 1), &m_states[1]);
    } else {
        UpdateWindow(m_projects[0], fn_80178B5C(fn_80079808(), 0), &m_states[0]);
    }
    for (int i = 0; i < 2; i++) {
        if (m_projects[i] != 0) {
            fn_8036DAD4(m_projects[i]);
        }
    }
}

void TGiCommandWindowImpl::Draw(RednerDrawArgs &args) {
    if (lbl_8057D6A0.m_hudVisible) {
        for (int i = 0; i < 2; i++) {
            if (m_projects[i] != 0) {
                fn_8036D94C(m_projects[i]);
            }
        }
        RCPtr<CScene> scene;
        char text[8];
        if (lbl_8057D6A0.m_twoPlayer) {
            scene = fn_8036D660(m_projects[1], "Weapon");
            if (scene.Get() != 0 && scene->IsEnd() && fn_80367138(scene.Get()) > 0.0f) {
                sprintf(text, "%03d", fn_80178B5C(fn_80079808(), 0)->m_ammo);
                fn_800BC7BC(m_numbers[0].get(), text, 120.0f, 1, 397.0f);
            }
            scene = fn_8036D660(m_projects[0], "Weapon");
            if (scene.Get() != 0 && scene->IsEnd() && fn_80367138(scene.Get()) > 0.0f) {
                sprintf(text, "%03d", fn_80178B5C(fn_80079808(), 1)->m_ammo);
                fn_800BC7BC(m_numbers[1].get(), text, 520.0f, 1, 397.0f);
            }
        } else {
            scene = fn_8036D660(m_projects[0], "Weapon");
            if (scene.Get() != 0 && scene->IsEnd() && fn_80367138(scene.Get()) > 0.0f) {
                sprintf(text, "%03d", fn_80178B5C(fn_80079808(), 0)->m_ammo);
                fn_800BC7BC(m_numbers[0].get(), text, 520.0f, 1, 397.0f);
            }
        }
    }
}

TGiCommand TGiCommandWindowImpl::GetWeaponCommand(UnkPlayerInfo_80178B5C *player) {
    TGiCommand command = {16, 0};
    if (player == 0) {
        return command;
    }
    switch (fn_8016EED0(player)) {
    case 1:
        switch (player->m_weaponKind) {
        case 1:
            command.m_icon = 1;
            break;
        case 2:
            command.m_icon = 2;
            break;
        case 3:
            command.m_icon = 3;
            break;
        case 4:
            command.m_icon = 4;
            break;
        case 5:
            command.m_icon = 0;
            break;
        case 6:
            command.m_icon = 5;
            break;
        }
        break;
    case 2:
        break;
    }
    return command;
}

TGiCommand TGiCommandWindowImpl::GetCarCommand(UnkPlayerInfo_80178B5C *player) {
    TGiCommand command = {16, 1};
    if (player == 0) {
        return command;
    }
    switch (fn_8016EEC8(player)) {
    case 0:
        break;
    case 1:
        command.m_icon = 11;
        break;
    case 2:
    case 5:
    case 6:
        command.m_icon = 12;
        break;
    case 3:
        break;
    case 4:
        command.m_icon = 13;
        break;
    case 10:
        command.m_icon = 14;
        break;
    case 7:
        command.m_icon = 15;
        break;
    case 12:
        command.m_button = 2;
    case 11:
        switch (player->m_vehicleWeapon) {
        case 1:
        case 2:
        case 3:
            command.m_icon = 6;
            break;
        case 4:
            command.m_icon = 7;
            break;
        case 5:
        case 6:
            command.m_icon = 8;
            break;
        case 7:
            command.m_icon = 9;
            break;
        case 8:
        case 9:
            command.m_icon = 10;
            break;
        }
        break;
    }
    return command;
}

/* NonMatching (99.3%): the stack layout and every call match; the registers do not. Retail keeps the
 * car command's icon in r26 across the calls that show it and reuses the state's icon from the
 * comparison, and gives `this` r27; GC/2.7 reloads both from memory. Naming the icon in a local makes
 * GC/2.7 move the whole command struct into registers instead (also with two TGiCommand locals, an
 * array, a reference, an inline operator= or operator!=). */
void TGiCommandWindowImpl::UpdateWindow(Chao::CSD::CProject *project, UnkPlayerInfo_80178B5C *player,
                                        TGiCommandState *state) {
    RCPtr<CScene> scene;
    RCPtr<CNode> node;
    TGiCommandState next;

    scene = fn_8036D660(project, "Window");
    if (scene.Get() == 0 || !scene->IsEnd()) {
        state->m_time = 0.0f;
        return;
    }

    next.m_weapon = GetWeaponCommand(player);
    scene = fn_8036D660(project, "Weapon");
    if (scene.Get() != 0 && scene->IsEnd() && next.m_weapon.m_icon != state->m_weapon.m_icon) {
        if (state->m_weapon.m_icon == 16) {
            fn_80367140(scene.Get(), 1.0f);
            fn_803659E4(scene.Get());
            node = fn_803666B4(scene.Get(), "icon_gun");
            fn_8036AD14(node.Get(), next.m_weapon.m_icon);
            node = fn_803666B4(scene.Get(), "button1");
            fn_8036AD14(node.Get(), next.m_weapon.m_button);
        } else {
            fn_80367140(scene.Get(), -1.0f);
            fn_803659E4(scene.Get());
            next.m_weapon.m_icon = 16;
        }
        state->m_weapon = next.m_weapon;
    }

    next.m_car = GetCarCommand(player);
    scene = fn_8036D660(project, "Car_BCom");
    bool meter = false;
    if (scene.Get() != 0 && scene->IsEnd()) {
        if (next.m_car.m_icon != state->m_car.m_icon || next.m_car.m_button != state->m_car.m_button) {
            if (next.m_car.m_icon != 16 && state->m_car.m_icon == 16) {
                fn_80367140(scene.Get(), 1.0f);
                fn_803659E4(scene.Get());
                node = fn_803666B4(scene.Get(), "icon_car");
                fn_8036AD14(node.Get(), next.m_car.m_icon);
                node = fn_803666B4(scene.Get(), "button2");
                fn_8036AD14(node.Get(), next.m_car.m_button);
            } else {
                fn_80367140(scene.Get(), -1.0f);
                fn_803659E4(scene.Get());
                next.m_car.m_icon = 16;
            }
            state->m_car = next.m_car;
        } else if (state->m_car.m_icon != 16) {
            meter = true;
        }
    }

    scene = fn_8036D660(project, "CarMeter");
    if (meter && fn_8016EEC8(player) == 12) {
        fn_80365B40(scene.Get(), 0);
        fn_80366740(scene.Get(), 20.0f * (1.0f - player->m_meter / player->m_meterMax));
    } else {
        fn_80365B40(scene.Get(), 1);
    }

    scene = fn_8036D660(project, "Window");
    if (state->m_open) {
        if (state->m_weapon.m_icon == 16 && state->m_car.m_icon == 16) {
            fn_80367140(scene.Get(), -1.0f);
            fn_803659E4(scene.Get());
            state->m_open = 0;
        }
    } else if (state->m_weapon.m_icon != 16 || state->m_car.m_icon != 16) {
        fn_80367140(scene.Get(), 1.0f);
        fn_803659E4(scene.Get());
        state->m_open = 1;
    }
}

TGiCommandWindow::TGiCommandWindow(Task *parent) : Task(parent), m_impl(new TGiCommandWindowImpl) {
    m_name = "TGiCommandWindow";
}

TGiCommandWindow::~TGiCommandWindow() {}

void TGiCommandWindow::Tick(float dt) {
    m_impl->Tick(dt);
}
