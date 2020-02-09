#pragma once
#include "Falcor.h"
#include "FalcorExperimental.h"

using namespace Falcor;
/**
    this pass defines the distribution monte carlo errors as a blue noise in screen space by permuting pixel seeds between frames
*/
class Sorting : public RenderPass, inherit_shared_from_this<RenderPass, Sorting> {

public:
    using SharedPtr = std::shared_ptr<Sorting>;

    // survey variables
    bool mIsInitialized = false;
    //for sorting pass be dis-or enabled
    bool sortingEnabled = true;

    //frame infos
    uint32_t frame_count = 1;
    uint32_t tile_width = 64;
    uint32_t tile_height = 64;
    uint32_t frame_width = 1920;
    uint32_t frame_height = 1080;

    //offsets for our struct variables
    size_t width_offset;
    size_t height_offset;
    size_t frame_count_offset;

    //compute context
    uint32_t groupDimX = 4;
    uint32_t groupDimY = 4;
    uint32_t numberOfGroupsX = (frame_width / groupDimX) + 1;
    uint32_t numberOfGroupsY = (frame_height / groupDimY) +1;

    //Internal pass state
    ComputeProgram::SharedPtr mpComputeProg;
    ComputeState::SharedPtr mpComputeState;
    ComputeVars::SharedPtr mpComputeProgVars;

    //textures
    Texture::SharedPtr bluenoise;
    Texture::SharedPtr copyForUnsorted;

    //seed texture stats
    uint seed_texture_width = 1920;
    uint seed_texture_height = 1080;

    /** Instantiate our pass.  The input Python dictionary is where you can extract pass parameters
    */
    static SharedPtr create(const Dictionary& params = {});

    /** Get a string describing what the pass is doing
    */
    virtual std::string getDesc() override { return "Sorting and retargeting seed for having a blue noise distributions."; }

    /** Defines the inputs/outputs required for this render pass
    */
    virtual RenderPassReflection reflect(void) const override;

    /** Run our sorting pass!
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
    Sorting() : RenderPass("Sorting")  {}

    /** Runs on first execute() to initialize rendering resources
    */
    void initialize(RenderContext* pContext, const RenderData* pRenderData);

};
