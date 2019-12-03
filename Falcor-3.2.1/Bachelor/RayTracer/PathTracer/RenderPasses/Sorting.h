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
    bool distributeAsBlueNoise = false;
    /** Instantiate our pass.  The input Python dictionary is where you can extract pass parameters
    */
    static SharedPtr create(const Dictionary& params = {});

    /** Get a string describing what the pass is doing
    */
    virtual std::string getDesc() override { return "Sorting and retargeting seed for having a blue noise distributions."; }

    /** Defines the inputs/outputs required for this render pass
    */
    virtual RenderPassReflection reflect(void) const override;

    /** Run our multibounce GI pass
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
    Sorting() : RenderPass("Sorting") {}

    /** Runs on first execute() to initialize rendering resources
    */
    void initialize(RenderContext* pContext, const RenderData* pRenderData);

    //Internal pass state
    ComputeProgram::SharedPtr mpComputeProg;
    ComputeState::SharedPtr mpComputeState;
    ComputeVars::SharedPtr mpComputeProgVars;

    Texture::SharedPtr  mpBlackHDR = nullptr;
    //all buffer objects
    StructuredBuffer::SharedPtr rwBuffer;
    StructuredBuffer::SharedPtr frameInfo;

};
