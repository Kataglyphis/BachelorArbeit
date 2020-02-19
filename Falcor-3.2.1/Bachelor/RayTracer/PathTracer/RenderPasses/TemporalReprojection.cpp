/***************************************************************************
# Copyright(c) 2019, Jonas Heinle.All rights reserved.
# RenderPass for generating a blue noise redistribution
# refering to the research from Eric Heitz and Laurent Belcour
# https://eheitzresearch.wordpress.com/772-2/
***************************************************************************/

#include "TemporalReprojection.h"
#include "Graphics/TextureHelper.h"

// global varibles
namespace {
    //Shader files
    const char* retargetShader = "Reproject.hlsl";
};

TemporalReprojection::SharedPtr TemporalReprojection::create(const Dictionary &params) {

    TemporalReprojection::SharedPtr pPass(new TemporalReprojection());

    //loading from python
    //TODO: create phyton file & create variables + keys

    return pPass;
}

Dictionary TemporalReprojection::getScriptingDictionary() const {

    Dictionary serialize;
    //TODO: create phyton file & create variables + keys
    return serialize;

}

RenderPassReflection TemporalReprojection::reflect(void) const {

    RenderPassReflection r;
    //input
    r.addInput("input_seed", "resorted seeds from the sorting phase").texture2D(seed_texture_width, seed_texture_height).format(ResourceFormat::BGRA8Unorm).bindFlags
    (Resource::BindFlags::UnorderedAccess |
        Resource::BindFlags::RenderTarget |
        Resource::BindFlags::ShaderResource).mipLevels(1);

    r.addInput("input_frame", "last rendered frame").format(ResourceFormat::RGBA32Float).bindFlags(Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess | Resource::BindFlags::RenderTarget);

    //output
    r.addOutput("output_seed", "the retargeted seed texture outgoing to path tracer").texture2D(seed_texture_width, seed_texture_height).format(ResourceFormat::BGRA8Unorm).bindFlags
    (Resource::BindFlags::UnorderedAccess |
        Resource::BindFlags::RenderTarget |
        Resource::BindFlags::ShaderResource).mipLevels(1);

    return r;
}

void TemporalReprojection::initialize(RenderContext * pContext, const RenderData * pRenderData) {

    //load prog from file
    mpComputeProg = ComputeProgram::createFromFile("Reproject.hlsl", "main");
    //initialize state
    mpComputeState = ComputeState::create();
    mpComputeState->setProgram(mpComputeProg);
    mpComputeProgVars = ComputeVars::create(mpComputeProg->getReflector());


    //info for the frame
    // Send data to compute shader; first fill structured buffer
    mpComputeProgVars->setStructuredBuffer("data", StructuredBuffer::create(mpComputeProg, "data", 1));

    if (mpComputeProg != nullptr) mIsInitialized = true;

    //copyForUnsorted = Texture::create2D(seed_texture_width, seed_texture_height, ResourceFormat::BGRA8Unorm,1,1);
    //pContext->copyResource(copyForUnsorted.get(), pRenderData->getTexture("input_seed").get());

    //TemporalReprojection pass is initialized in the beginning!
    this->enable_reprojection_pass_shader_var = 1;

}

void TemporalReprojection::execute(RenderContext* pContext, const RenderData* pData) {

    //on first run we want it to intialize
    if (!mIsInitialized) {

        initialize(pContext, pData);

    }

    //update frame count
    //it is enough for this application to toroidally switch between 128 frame counts
    this->frame_count = frame_count % 128;

    //info for the frame
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_width"] = tile_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_height"] = tile_height;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_width"] = frame_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_height"] = frame_height;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_count"] = frame_count++;
    //mpComputeProgVars->getStructuredBuffer("data")[0]["camera_position"] = pContext->;
    mpComputeProgVars->getStructuredBuffer("data")[0]["enable"] = this->enable_reprojection_pass_shader_var;

    mpComputeProgVars->setTexture("src_seed_texture", pData->getTexture("input_seed"));
    //set the putput seed tex in HLSL namespace!!
    mpComputeProgVars->setTexture("output_seed_texture", pData->getTexture("output_seed"));

    pContext->setComputeState(mpComputeState);
    pContext->setComputeVars(mpComputeProgVars);

    //implementation info from here : https://hal.archives-ouvertes.fr/hal-02158423/file/blueNoiseTemporal2019_slides.pdf
    if (frame_count >= 1) {
    //only reproject if we have a previous frame already :)
    pContext->dispatch(numberOfGroupsX, numberOfGroupsY, 1);
    }

    pContext->copyResource(pData->getTexture("output_seed").get(), mpComputeProgVars->getTexture("output_seed_texture").get());
}

void TemporalReprojection::renderUI(Gui* pGui, const char* uiGroup) {

    pGui->addCheckBox("Dis-/Enable the TemporalReprojection pass", enableTemporalReprojectionPass);

    enable_reprojection_pass_shader_var = enableTemporalReprojectionPass ? 1 : 0;
}

void TemporalReprojection::setScene(const std::shared_ptr<Scene>& pScene) {
    //pScene->getActiveCamera()->getPosition();
}

void TemporalReprojection::onResize(uint32_t width, uint32_t height) {
    //Upadate frame data 
    frame_width = width;
    frame_height = height;
}