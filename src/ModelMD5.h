#pragma once

#include "math/vec3.h"
#include "math/quaternion.h"
#include "BSPBIN.h"
#include <map>

class CModelMD5;
#include "ResourceManager.h"

#define BIG_STUPID_DOOM3_ROCKETLAUNCHER_HACK //FIXME translate some doom3 models in the code... the pain... remove this ASAP... ASAP!

#define MD5_GL_VERTEXINDEX_TYPE  GL_UNSIGNED_INT
typedef uint32_t md5_vertexindex_t;
struct md5_vbo_vertex_t // if you change this, you need to change the VBO code too
{
    vec3_t v;
    vec3_t n;
    float tu, tv;
    vec3_t t; // tangent
    float w; // value to compute bitangent vector: Bitangent = w * (Normal x Tangent)
};

/* This code is based upon the great MD5 documentation
 * and source code by David Henry */

/* Joint */
struct md5_joint_t
{
    char name[64];
    int parent;

    vec3_t pos;
    quaternion_t orient;
};

/* Vertex */
struct md5_vertex_t
{
    float u, v;
    int start; /* start weight index */
    int count; /* weight count */

    // Store the vertex and the normal in the bind pose.
    // This is used for a simple normal calculation.
    vec3_t vertex; // in bind pose
    vec3_t normal; // in bind pose
    vec3_t tangent; // in bind pose
    vec3_t bitangent; // in bind pose
    float w; // bitangent w component -1.0f or 1.0f
};

/* Triangle */
struct md5_triangle_t
{
    int index[3];
};

/* Weight */
struct md5_weight_t
{
    int joint;
    float bias;

    vec3_t pos;
    vec3_t normal;
    vec3_t tangent;
    vec3_t bitangent;
};

/* Bounding box */
struct md5_bbox_t
{
    vec3_t min;
    vec3_t max;
};

/* MD5 mesh */
struct md5_mesh_t
{
    std::vector<md5_vertex_t>   vertices;
    std::vector<md5_triangle_t> triangles;
    std::vector<md5_weight_t>   weights;

    int num_verts;
    int num_tris;
    int num_weights;

    char shader[256];
    int tex;
    int texnormal;
};

/* Animation data */
struct md5_anim_t
{
    int num_frames;
    int num_joints;
    int frameRate;

    std::vector< std::vector<md5_joint_t> > skelFrames;
    std::vector<md5_bbox_t> bboxes;
};

/* md5 animation state */
struct md5_state_t
{
    md5_state_t()
    {
        curr_frame = 0;
        next_frame = 1;
        animation = ANIMATION_NONE;
        animdata = NULL;
        time = 0.0f;
    }
    int curr_frame;
    int next_frame;
    animation_t animation; // animation id
    md5_anim_t* animdata; // md5 animation data
    float time; // from 0.0 s to num_frames/framerate

    // skel holds the data of the interpolated
    // skeleton between two frames
    std::vector<md5_joint_t> skel;
};

class CModelMD5
{
public:
    CModelMD5(void);
    ~CModelMD5(void);

    bool    Load(char *path, CResourceManager* resman, bool loadtexture=true);
    void    Unload();

    void    Render(const md5_state_t* state);
    void    RenderNormals(const md5_state_t* state);
    void    Animate(md5_state_t* state, const float dt) const;
    void    SetAnimation(md5_state_t* state, const animation_t animation);

    float   GetSphere() const { return 2.0f; }; // FIXME

private:
    bool    ReadAnimation(const animation_t animation, const std::string filepath); // animation = "run", "idle", "walk" etc.
    md5_anim_t* GetAnimation(const animation_t animation, bool createnew); // Get md5_anim_t associated with animation string or create a new one
    void    RenderSkeleton(const std::vector<md5_joint_t>& skel) const;
    void    PrepareBindPoseNormals(md5_mesh_t *mesh);
    void    PrepareMesh(const md5_mesh_t *mesh, const std::vector<md5_joint_t>& skeleton);
    bool    AllocVertexBuffer();
    void    DeallocVertexBuffer();
    bool    UploadVertexBuffer(unsigned int vertexcount, unsigned int indexcount) const;

    std::vector<md5_joint_t> m_baseSkel;
    std::vector<md5_mesh_t>  m_meshes;
    int     m_num_joints;
    int     m_num_meshes;

    int     m_max_verts;
    int     m_max_tris;

    // data for vertex buffer objects
    unsigned int       m_vbo;
    unsigned int       m_vboindex;
    md5_vbo_vertex_t*  m_vertex_buffer;
    md5_vertexindex_t* m_vertex_index_buffer;

    // Animation map. Link between the lynx animation id and the raw animation
    // data
    std::map<animation_t, md5_anim_t*> m_animations;

#ifdef BIG_STUPID_DOOM3_ROCKETLAUNCHER_HACK
    vec3_t m_big_stupid_doom3_rocketlauncher_hack_offset;
#endif

};

