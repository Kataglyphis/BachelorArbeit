#pragma once
#include "Falcor.h"
#include "FalcorExperimental.h"

#include <string>

using namespace Falcor;
/**
    this pass defines the distribution monte carlo errors as a blue noise in screen space by permuting pixel seeds between frames
*/
class Retargeting : public RenderPass, inherit_shared_from_this<RenderPass, Retargeting> {

public:
    using SharedPtr = std::shared_ptr<Retargeting>;

    /** Instantiate our pass.  The input Python dictionary is where you can extract pass parameters
    */
    static SharedPtr create(const Dictionary& params = {});

    /** Get a string describing what the pass is doing
    */
    virtual std::string getDesc() override { return "Retarget Seeds with precomputed texture for accumulate improvements!"; }

    /** Defines the inputs/outputs required for this render pass
    */
    virtual RenderPassReflection reflect(void) const override;

    /** Runs the retargeting pass
    */
    virtual void execute(RenderContext* pContext, const RenderData* pRenderData) override;

    /** Display a GUI exposing rendering parameters
    */
    virtual void renderUI(Gui* pGui, const char* uiGroup) override;

    /** Grab the current scene so we can render it!
    */
    virtual void setScene(const std::shared_ptr<Scene>& pScene) override;

    /** Do any updates needed when we resize our window
    */
    virtual void onResize(uint32_t width, uint32_t height) override;

    /** Serialize the render pass parameters out to a python dictionary
    */
    virtual Dictionary getScriptingDictionary() const override;

private:
    Retargeting() : RenderPass("Retargeting") {}

    /** Runs on first execute() to initialize rendering resources
    */
    void initialize(RenderContext* pContext, const RenderData* pRenderData);

    bool hasCameraMoved();

    //Internal pass state
    ComputeProgram::SharedPtr mpComputeProg;
    ComputeState::SharedPtr mpComputeState;
    ComputeVars::SharedPtr mpComputeProgVars;

    //folders
    std::string temporal_reprojection_base_folder = "TemporalReprojection/";
    // will always be the same for x and y dim
    int dim_motion_vec = 4;
    //all textures for temporal retargeting
     /*Texture::SharedPtr retarget_texture0x1;
     Texture::SharedPtr retarget_texture0x2;
     Texture::SharedPtr retarget_texture0x3;
     Texture::SharedPtr retarget_texture1x0;
     Texture::SharedPtr retarget_texture1x1;
     Texture::SharedPtr retarget_texture1x2;
     Texture::SharedPtr retarget_texture1x3;
     Texture::SharedPtr retarget_texture2x0;
     Texture::SharedPtr retarget_texture2x1;
     Texture::SharedPtr retarget_texture2x2;
     Texture::SharedPtr retarget_texture2x3;*/

     //-1 for skipping very first frame; skippng for waiting for first sorting pass to be executed
     int32_t frame_count = -1;
     uint32_t tile_width = 64;
     uint32_t tile_height = 64;
     uint32_t frame_width = 1920;
     uint32_t frame_height = 1080;
     uint32_t enable_retarget_pass_shader_var;
     uint32_t enable_temporal_reprojection_pass_shader_var;

     //for compute context
     uint32_t groupDimX = 8;
     uint32_t groupDimY = 8;
     uint32_t numberOfGroupsX = (frame_width / groupDimX) + 1;
     uint32_t numberOfGroupsY = (frame_height / groupDimY) + 1;

     //survey variables
     bool mIsInitialized = false;
     bool enableRetargetingPass = false;
     bool enableTemporalReprojectionPass = true;

     //seed texture stats
     uint seed_texture_width = 1920;
     uint seed_texture_height = 1080;

     //temporal reprojection part; 
     mat4                mpPrevViewProjMatrix;
     mat4                mpPrevViewProjMatrixInv;
     mat4                mpCurrViewProjMatrix;
     mat4                mpCurrViewProjMatrixInv;
     vec3                lastCameraPosition;
     std::shared_ptr<Scene>    mpScene;

};
