#pragma once
#include "Falcor.h"
#include "FalcorExperimental.h"

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream

using namespace Falcor;

class TemporalReprojection :
	public RenderPass, inherit_shared_from_this<RenderPass, TemporalReprojection> {
public:
    using SharedPtr = std::shared_ptr<TemporalReprojection>;

    uint32_t frame_count = 0;
    uint32_t tile_width = 64;
    uint32_t tile_height = 64;
    uint32_t frame_width = 1920;
    uint32_t frame_height = 1080;
    uint32_t enable_reprojection_pass_shader_var;

    //for compute context
    uint32_t groupDimX = 8;
    uint32_t groupDimY = 8;

    //for compute state
    uint32_t numberOfGroupsX = (frame_width / groupDimX) + 1;
    uint32_t numberOfGroupsY = (frame_height / groupDimY) + 1;

    //offsets for our struct variables
    size_t width_offset;
    size_t height_offset;
    size_t frame_count_offset;

    //survey variables
    bool mIsInitialized = false;
    bool enableTemporalReprojectionPass = true;

    //the seed texture for stopping the TemporalReprojection and sorting
    Texture::SharedPtr copyForUnsorted;

    //seed texture stats
    uint seed_texture_width = 1920;
    uint seed_texture_height = 1080;

    //temporal part; 
    mat4                mpLastViewProjMatrix;
    mat4                mpViewProjMatrixPreviousPos;
    std::shared_ptr<Scene>    mpScene;


    /** Instantiate our pass.  The input Python dictionary is where you can extract pass parameters
    */
    static SharedPtr create(const Dictionary& params = {});

    /** Get a string describing what the pass is doing
    */
    virtual std::string getDesc() override { return "Retarget Seeds with precomputed texture for accumulate improvements!"; }

    /** Defines the inputs/outputs required for this render pass
    */
    virtual RenderPassReflection reflect(void) const override;

    /** Runs the TemporalReprojection pass
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
    TemporalReprojection() : RenderPass("TemporalReprojection") {}

    /** Runs on first execute() to initialize rendering resources
    */
    void initialize(RenderContext* pContext, const RenderData* pRenderData);

    /**
        we want to take screenshots as soon as the camera has moved
    */
    void takeScreenshot(SampleCallbacks* pCallbacks);

    /**
    signals whether the camera has moved or not
    */
    bool hasCameraMoved();

    //Internal pass state
    ComputeProgram::SharedPtr mpComputeProg;
    ComputeState::SharedPtr mpComputeState;
    ComputeVars::SharedPtr mpComputeProgVars;

    //holding trace of the capturing
    uint32_t trace_count = 0;
    bool take_shot_of_next_frame = 0;

};

