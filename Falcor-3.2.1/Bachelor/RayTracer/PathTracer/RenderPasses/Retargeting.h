#pragma once
#include "Falcor.h"
#include "FalcorExperimental.h"

using namespace Falcor;
/**
    this pass defines the distribution monte carlo errors as a blue noise in screen space by permuting pixel seeds between frames
*/
class Retargeting : public RenderPass, inherit_shared_from_this<RenderPass, Retargeting> {

public:
    using SharedPtr = std::shared_ptr<Retargeting>;

    uint32_t frame_count = 0;
    uint32_t frame_width = 1920;
    uint32_t frame_height = 720;

    //our bind locations
    struct
    {
        ProgramReflection::BindLocation perFrameData;
    } mBindLocations;

    //offsets for our struct variables
    size_t width_offset;
    size_t height_offset;
    size_t frame_count_offset;

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

    //Internal pass state
    ComputeProgram::SharedPtr mpComputeProg;
    ComputeState::SharedPtr mpComputeState;
    ComputeVars::SharedPtr mpComputeProgVars;

    //survey variables
    bool mIsInitialized = false;
    bool retargetSeeds = true;

};
