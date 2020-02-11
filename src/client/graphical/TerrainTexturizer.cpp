/* Create the terrain texture for the whole block */

#include <glm/glm.hpp>

#include <client/graphical/TerrainRenderer.hpp>
#include <client/graphical/TextureOpener.hpp>

#include <algorithm>
#include <array>
#include <iterator>

#include <common/Log.hpp>

using namespace familyline::graphics;

/* Count of the terrain slot square side to be rendered */
constexpr int SlotSide = SECTION_SIDE;

/* One terrain square */
typedef std::array<unsigned int, ImageWidth*ImageHeight> TerrainBitmap;
    
/* One terrain slot. One slot has multiple squares */
typedef std::vector<unsigned int> TerrainSlotTexture;


static unsigned int ColorToPixel(glm::vec4 color) {
    /* Normalize the pixels too.
       This allows a more correct color, and a more natural color gradation in the texture merging

    */
    auto pmax = std::max(color.r, std::max(color.g, std::max(color.b, color.a)));

    unsigned r = std::min(unsigned(color.r/pmax * 255), 255u);
    unsigned g = std::min(unsigned(color.g/pmax * 255), 255u);
    unsigned b = std::min(unsigned(color.b/pmax * 255), 255u);
    unsigned a = std::min(unsigned(color.a * 255), 255u);

    return r | (g << 8) | (b << 16) | (a << 24);
}

/* Merge the four terrain types on each corner into one terrain, with influences
   from the four courners

   Each array represents a 32-bit color bitmap for each terrain type.

   This function assumes all surfaces have the RGBA format (the less significant byte is red,
   the most significant byte is alpha)
*/
static TerrainBitmap MergeTerrain(TerrainBitmap lt, TerrainBitmap rt,
                                  TerrainBitmap lb, TerrainBitmap rb)
{
    /*
      lt------rt
      |        |
      |        |
      |        |
      lb------rb
    */

    TerrainBitmap ret;

    /* Convert a pixel to 4 floats between 0-1
       Easier to do calculations
    */
    auto fnGetFloatPixel = [](unsigned int val) {
                               unsigned r = val & 0xff;
                               unsigned g = (val >> 8) & 0xff;
                               unsigned b = (val >> 16) & 0xff;
                               unsigned a = (val >> 24) & 0xff;

                               return glm::vec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
                           };


    unsigned int* plt = (unsigned int*)lt.data();
    unsigned int* prt = (unsigned int*)rt.data();
    unsigned int* plb = (unsigned int*)lb.data();
    unsigned int* prb = (unsigned int*)rb.data();

    #pragma omp parallel for
    for (unsigned int y = 0; y < ImageHeight; y++) {
        for (unsigned int x = 0; x < ImageWidth; x++) {
            float mixy = float(y) / float(ImageHeight);
            float mixx = float(x) / float(ImageWidth);
            const auto idx = y * ImageWidth + x;

            const glm::vec4 flt = fnGetFloatPixel(plt[idx]);
            const glm::vec4 frt = fnGetFloatPixel(prt[idx]);
            const glm::vec4 flb = fnGetFloatPixel(plb[idx]);
            const glm::vec4 frb = fnGetFloatPixel(prb[idx]);

            const glm::vec4 cleft = (flt * (1.0f-mixy) + flb * mixy) * (1.0f-mixx);
            const glm::vec4 cright = (frt * (1.0f-mixy) + frb * mixy) * (mixx);
            const glm::vec4 ctop = (flt * (1.0f-mixx) + frt * mixx ) * (1.0f-mixy);
            const glm::vec4 cbottom = (flb * (1.0f-mixx) + frb * mixx ) * (mixy);

            const glm::vec4 cfinal = cleft + cright + ctop + cbottom;

            ret[idx] = ColorToPixel(cfinal);
        }
    }

    return ret;
}

/* TODO: Read this from a configuration file */
#define GET_TEXTURE_RAW(fn) (unsigned int*)                             \
    TextureOpener::OpenFile(fn)->GetTextureRaw(0, 0, ImageHeight, ImageWidth)

TerrainBitmap TerrainRenderer::GetTerrainTexture(unsigned int type_id)
{
    static unsigned int* textures[] = {
        GET_TEXTURE_RAW("textures/terrain/grass.png")
    };
    
    if (type_id >= (sizeof(textures)/sizeof(unsigned int*))) {
        Log::GetLog()->Fatal("terrain-renderer", "terrain type %d has no texture! Using id 0 in place",
                             type_id);
        type_id = 0;
    }
    
    TerrainBitmap ret;
    auto tex = textures[type_id];

    std::copy_n(tex, ImageWidth*ImageHeight, std::begin(ret));
    return ret;
}


/* Generate terrain texture from the terrain data named 'data' */
Texture* TerrainRenderer::GenerateTerrainSlotTexture(familyline::logic::TerrainData* data)
{
    TerrainSlotTexture terrain_surface;
    terrain_surface.resize(ImageWidth*SlotSide*ImageHeight*SlotSide);

    #pragma omp parallel for
    for (unsigned sy = 0; sy < SlotSide-1; sy++) {
        for (unsigned sx = 0; sx < SlotSide-1; sx++) {
            auto texture_lt = data->data[(sy * SlotSide + sx)].terrain_type;
            auto texture_rt = data->data[(sy * SlotSide + sx+1)].terrain_type;
            auto texture_lb = data->data[((sy+1) * SlotSide + sx)].terrain_type;
            auto texture_rb = data->data[((sy+1) * SlotSide + sx+1)].terrain_type;
        
            auto srcs = MergeTerrain(GetTerrainTexture(texture_lt), GetTerrainTexture(texture_rt),
                                     GetTerrainTexture(texture_lb), GetTerrainTexture(texture_rb));

            const unsigned dstx = sx*ImageWidth;
            const unsigned dsty = sy*ImageHeight;
            const unsigned terrainw = ImageWidth*SlotSide;
        
            // Puts the terrain image into the whole texture for that slot.
            for (unsigned int offy = 0; offy < ImageHeight; offy++) {
                auto itstart = (offy*ImageWidth);
                std::copy_n(srcs.begin()+itstart, ImageWidth,
                            terrain_surface.begin() + ((dsty + offy) * terrainw + dstx ));
            }


        }
        
        fprintf(stderr, ".");
    }

    Log::GetLog()->Write("terrain-renderer", "Terrain texture generation complete");
    return new Texture(ImageWidth * SlotSide, ImageHeight * SlotSide, GL_RGBA, terrain_surface.data());
}
