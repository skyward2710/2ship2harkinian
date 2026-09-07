#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Etcetera/z_obj_etcetera.h"
#include "overlays/actors/ovl_Obj_Rotlift/z_obj_rotlift.h"
}

#define SHUFFLED_DEKU_FLOWERS (IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_DEKU_FLOWERS])
#define IS_AT(xx, yy, zz) (actor->home.pos.x == xx && actor->home.pos.y == yy && actor->home.pos.z == zz)

// clang-format off
std::map < std::tuple<s16, s16, s16, s16>, RandoCheckId> objEtceteraMap {
    //Termina Field
    { { ACTOR_OBJ_ETCETERA, SCENE_00KEIKOKU, 0, 47 }, RC_TERMINA_FIELD_DEKU_FLOWER_NEAR_HOLLOW_LOG_RAMP, },
    { { ACTOR_OBJ_ETCETERA, SCENE_00KEIKOKU, 0, 48 }, RC_TERMINA_FIELD_DEKU_FLOWER_NEAR_CHEST_STUMP, },
    { { ACTOR_OBJ_ETCETERA, SCENE_00KEIKOKU, 0, 49 }, RC_TERMINA_FIELD_DEKU_FLOWER_NEAR_SWAMP_PATH, },
    { { ACTOR_OBJ_ETCETERA, SCENE_00KEIKOKU, 0, 50 }, RC_TERMINA_FIELD_DEKU_FLOWER_NEAR_OBSERVATORY, },
	//South Clock Town
	{ { ACTOR_OBJ_ETCETERA, SCENE_CLOCKTOWER, 0, 4 }, RC_CLOCK_TOWN_SOUTH_MERCHANT_DEKU_FLOWER, },
    //North Clock Town
    { { ACTOR_OBJ_ETCETERA, SCENE_BACKTOWN, 0, 2 }, RC_CLOCK_TOWN_NORTH_DEKU_FLOWER_OUTSIDE_FENCE, },
    { { ACTOR_OBJ_ETCETERA, SCENE_BACKTOWN, 0, 3 }, RC_CLOCK_TOWN_NORTH_DEKU_FLOWER_INISDE_FENCE, },
    //East Clock Town
    { { ACTOR_OBJ_ETCETERA, SCENE_TOWN, 0, 9 }, RC_CLOCK_TOWN_EAST_DEKU_FLOWER_NEAR_INN, },
    //Road to Southern Swamp
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 18 }, RC_ROAD_TO_SOUTHERN_SWAMP_DEKU_FLOWER_PATH_TO_TERMINA_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 19 }, RC_ROAD_TO_SOUTHERN_SWAMP_DEKU_FLOWER_PATH_TO_TERMINA_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 17 }, RC_ROAD_TO_SOUTHERN_SWAMP_DEKU_FLOWER_PATH_TO_TERMINA_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 20 }, RC_ROAD_TO_SOUTHERN_SWAMP_DEKU_FLOWER_PATH_TO_ARCHERY_HOUSE_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 21 }, RC_ROAD_TO_SOUTHERN_SWAMP_DEKU_FLOWER_PATH_TO_ARCHERY_HOUSE_2, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 22 }, RC_ROAD_TO_SOUTHERN_SWAMP_DEKU_FLOWER_PATH_TO_ARCHERY_HOUSE_3, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 16 }, RC_ROAD_TO_SOUTHERN_SWAMP_DEKU_FLOWER_PATH_TO_SWAMP_1, },
    { { ACTOR_OBJ_ETCETERA, SCENE_24KEMONOMITI, 0, 15 }, RC_ROAD_TO_SOUTHERN_SWAMP_DEKU_FLOWER_PATH_TO_SWAMP_2, },
};

std::map < std::tuple<s16, s16, s16, s16>, RandoCheckId> objRailliftMap{
    {{ACTOR_OBJ_RAILLIFT,SCENE_22DEKUCITY,1,9},RC_DEKU_PALACE_BEAN_SIDE_MOVING_PLATFORM_DEKU_FLOWER_2, },
};
// clang-format on

// For flowers that are identified based on being a bouncing pink flower, identify based on the home pos.
// This home position is inherited from the object that spawned it
RandoCheckId IdentifyEtceteraBasedOnPos(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    switch (gPlayState->sceneId) {
        case SCENE_22DEKUCITY:
            if (IS_AT(681.0f, 160.0f, 1785.0f)) {
                randoCheckId = RC_DEKU_PALACE_BEAN_SIDE_DEKU_SCRUB_FLOWER_2;
            }
            break;
    }
    return randoCheckId;
}

// For flowers spawned by the scene, identify based on actor id
RandoCheckId IdentifyEtceteraBasedOnId(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    auto it = objEtceteraMap.find(
        { actor->id, gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex(actor) });
    if (it == objEtceteraMap.end()) {
        return randoCheckId;
    }
    randoCheckId = it->second;
    return randoCheckId;
}

RandoCheckId IdentifyEtceteraBasedOnParent(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;
    ObjEtcetera* flower = (ObjEtcetera*)actor;
    Actor* parent = actor->parent;
    if (parent->id == ACTOR_OBJ_RAILLIFT) {
        auto it = objRailliftMap.find(
            { parent->id, gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex(parent) });
        if (it == objRailliftMap.end()) {
            return randoCheckId;
        }
        randoCheckId = it->second;
        return randoCheckId;
    }
    if (parent->id == ACTOR_OBJ_ROTLIFT) {
        ObjRotlift* lift = (ObjRotlift*)parent;
        if (GetActorListIndex(parent) == 5) {
            if (lift->dekuFlowers[0] == flower) {
                return RC_MOON_TRAIL_DEKU_ROTATING_PLATFORM_SET_1_DEKU_FLOWER_1;
            } else {
                return RC_MOON_TRAIL_DEKU_ROTATING_PLATFORM_SET_1_DEKU_FLOWER_2;
            }
        } else if (GetActorListIndex(parent) == 6) {
            if (lift->dekuFlowers[0] == flower) {
                return RC_MOON_TRAIL_DEKU_ROTATING_PLATFORM_SET_2_GOLD_DEKU_FLOWER;
            } else {
                return RC_MOON_TRAIL_DEKU_ROTATING_PLATFORM_SET_2_PINK_DEKU_FLOWER;
            }
        }
    }
}

void Draw_ObjEtcetera_Idle_Rando(Actor* actor, PlayState* play) {
    ObjEtcetera* etcetera = (ObjEtcetera*)actor;

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, etcetera->dList);

    CLOSE_DISPS(play->state.gfxCtx);
    Rando::ActorBehavior::SpawnDekuFlowerSparkle(actor);
}

void Draw_ObjEtcetera_Animated_Rando(Actor* actor, PlayState* play) {
    ObjEtcetera* etcetera = (ObjEtcetera*)actor;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, etcetera->skelAnime.skeleton, etcetera->skelAnime.jointTable, NULL, NULL,
                      &etcetera->dyna.actor);
    Rando::ActorBehavior::SpawnDekuFlowerSparkle(actor);
}

// Exact copy of the vanilla function to break out of the rando idle -> animated -> idle loop after the check has been
// collected. Animated because the only way to collect the check is to launch out of the flower, which always sets the
// function to animated
void ObjEtcetera_DrawAnimated(Actor* actor, PlayState* play) {
    ObjEtcetera* etcetera = (ObjEtcetera*)actor;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, etcetera->skelAnime.skeleton, etcetera->skelAnime.jointTable, NULL, NULL,
                      &etcetera->dyna.actor);
}

void Rando::ActorBehavior::SpawnDekuFlowerSparkle(Actor* actor) {
    if ((gGameState->frames % 4) != 0) {
        return;
    }

    static Vec3f sVelocity = { 0.0f, 3.0f, 0.0f };
    static Vec3f sAccel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 0, 255, 64, 255 };
    Vec3f newPos;

    newPos.x = Rand_CenteredFloat(10.0f) + actor->world.pos.x;
    newPos.y = (Rand_ZeroOne() * 10.0f) + actor->world.pos.y;
    newPos.z = Rand_CenteredFloat(10.0f) + actor->world.pos.z;

    EffectSsKirakira_SpawnDispersed(gPlayState, &newPos, &sVelocity, &sAccel, &sPrimColor, &sEnvColor, 5000, 16);
}

void Rando::ActorBehavior::InitObjEtceteraBehavior() {

    // Hijack the normal idle -> animated -> idle cycle of actor draw to rando specific ones
    COND_VB_SHOULD(VB_DRAW_ETCETERA_IDLE, SHUFFLED_DEKU_FLOWERS, {
        ObjEtcetera* etcetera = va_arg(args, ObjEtcetera*);
        if (etcetera->dyna.actor.draw == nullptr) {
            *should = true;
        } else if ((etcetera->dyna.actor.draw == Draw_ObjEtcetera_Idle_Rando) ||
                   (etcetera->dyna.actor.draw == Draw_ObjEtcetera_Animated_Rando)) {
            etcetera->dyna.actor.draw = Draw_ObjEtcetera_Idle_Rando;
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_DRAW_ETCETERA_ANIMATED, SHUFFLED_DEKU_FLOWERS, {
        ObjEtcetera* etcetera = va_arg(args, ObjEtcetera*);
        if (etcetera->dyna.actor.draw == nullptr) {
            *should = true;
        } else if ((etcetera->dyna.actor.draw == Draw_ObjEtcetera_Idle_Rando) ||
                   (etcetera->dyna.actor.draw == Draw_ObjEtcetera_Animated_Rando)) {
            etcetera->dyna.actor.draw = Draw_ObjEtcetera_Animated_Rando;
            *should = false;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_ETCETERA, SHUFFLED_DEKU_FLOWERS, [](Actor* actor) {
        RandoCheckId randoCheckId = RC_UNKNOWN;
        ObjEtcetera* flower = (ObjEtcetera*)actor;
        Player* player = GET_PLAYER(gPlayState);
        // Without the oscillation timer check, the rest of the function fires 10 times per launch
        if ((player->stateFlags3 & PLAYER_STATE3_200) && (flower->dyna.actor.xzDistToPlayer < 20.0f) &&
            (flower->oscillationTimer >= 29)) {
            // All flowers spawned after scene init have their params set to 128 - Pink flower with bounce
            if (actor->params == DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE)) {
                randoCheckId = IdentifyEtceteraBasedOnPos(actor);
            } else if (actor->parent != nullptr) {
                randoCheckId = IdentifyEtceteraBasedOnParent(actor);
            } else {
                randoCheckId = IdentifyEtceteraBasedOnId(actor);
            }
            if (!RANDO_SAVE_CHECKS[randoCheckId].cycleObtained && (randoCheckId != RC_UNKNOWN)) {
                RANDO_SAVE_CHECKS[randoCheckId].eligible = true;
                actor->draw = ObjEtcetera_DrawAnimated;
            }
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_ETCETERA, SHUFFLED_DEKU_FLOWERS, [](Actor* actor) {
        RandoCheckId randoCheckId = RC_UNKNOWN;
        if (actor->params == DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE)) {
            randoCheckId = IdentifyEtceteraBasedOnPos(actor);
        } else if (actor->parent != nullptr) {
            randoCheckId = IdentifyEtceteraBasedOnParent(actor);
        } else {
            randoCheckId = IdentifyEtceteraBasedOnId(actor);
        }
        if (!RANDO_SAVE_CHECKS[randoCheckId].cycleObtained && (randoCheckId != RC_UNKNOWN)) {
            // Init as idle, actor setup will take care of changing this function to animated if its a pink bouncing
            // flower
            actor->draw = Draw_ObjEtcetera_Idle_Rando;
        };
    });
}