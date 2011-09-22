#pragma once

#include <string>
#include <map>
class CResourceManager;
#include "ModelMD5.h"
#include "Sound.h"

class CWorld;

struct texture_t
{
    unsigned int id; // OpenGL texture id
    unsigned int width; // width in pixel
    unsigned int height; // height in pixel
    std::string path; // file path
};

typedef enum
{
    LYNX_RESOURCE_TYPE_TEXTURE,
    LYNX_RESOURCE_TYPE_MD5,
    LYNX_RESOURCE_TYPE_SOUND
} resource_type_t;

class CResourceManager
{
public:
    CResourceManager(CWorld* world);
    ~CResourceManager(void);

    void Precache(const std::string filename, const resource_type_t type); // just try to precache a resource for later use
    void Shutdown(); // unload everything

    unsigned int GetTexture(const std::string texname, const bool noerrormsg=false);
    void UnloadAllTextures();

    bool GetTextureDimension(const std::string texname,
                             unsigned int* pwidth,
                             unsigned int* pheight) const;

    CModelMD5* GetModel(std::string mdlname);
    void UnloadAllModels();

    CSound* GetSound(const std::string sndname, const bool silent=false); // silent: no output if loading works
    void UnloadAllSounds();

    bool IsServer() const;

    static animation_t GetAnimationFromString(std::string animation_name);
    static std::string GetStringFromAnimation(animation_t animation);

private:
    unsigned int LoadTexture(const std::string path,
                             unsigned int* pwidth,
                             unsigned int* pheight,
                             const bool noerrormsg=false);

    std::map<std::string, texture_t> m_texmap;
    std::map<std::string, CModelMD5*> m_modelmap;
    std::map<std::string, CSound*> m_soundmap;

    CWorld* m_world;
};
