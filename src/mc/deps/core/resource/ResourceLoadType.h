
#pragma once

#include "mc/_HeaderOutputPredefine.h"


#include "mc/deps/core/file/PathBuffer.h"
#include "mc/deps/core/resource/ResourceFileSystem.h"


enum class ResourceLoadType : uint {
    None                        = 0,
    Texture                     = 1,
    Sound                       = 2,
    MusicDefinition             = 3,
    Localization                = 4,
    Geometry                    = 5,
    TerrainAtlas                = 6,
    ItemAtlas                   = 7,
    AnimatedAtlas               = 8,
    FontGlyph                   = 9,
    BlockGraphics               = 10,
    Tessellator                 = 11,
    Material                    = 12,
    Seasons                     = 13,
    ActorResourceDefinition     = 14,
    UIDefinitions               = 15,
    UIDefinitionReferences      = 16,
    UIDefinitionReports         = 17,
    UIDefinitionResults         = 18,
    ActorSkeletalAnimation      = 19,
    ActorAnimationController    = 20,
    MaterialDescription         = 21,
    RenderController            = 22,
    ClientInstanceLoadResources = 23,
    ItemRenderer                = 24,
    DynamicTextures             = 25,
    LevelRenderer               = 26,
    ParticleStaticResources     = 27,
    FoliageColors               = 28,
    ClearAndResetFontData       = 29,
    FinalizeLoad                = 30,
    ParticleEffect              = 31,
    Lighting                    = 32,
    DeferredRendering           = 33,
    AtmosphereScattering        = 34,
    BlockCulling                = 35,
    Water                       = 36,
    JigsawStructure             = 37,
    BlockThumbnailAtlas         = 38,
    All                         = 39,
    Count                       = 39,
};
