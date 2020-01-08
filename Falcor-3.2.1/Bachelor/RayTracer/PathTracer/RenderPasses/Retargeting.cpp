/***************************************************************************
# Copyright (c) 2019, Jonas Heinle. All rights reserved.
# RenderPass for generating a blue noise redistribution
# refering to the research from Eric Heitz and Laurent Belcour
# https://eheitzresearch.wordpress.com/772-2/
***************************************************************************/

#include "Retargeting.h"
#include "Graphics/TextureHelper.h"

// global varibles
namespace {
    //Shader files
    const char* retargetShader = "Retarget.hlsl";
};

Retargeting::SharedPtr Retargeting::create(const Dictionary &params) {

    Retargeting::SharedPtr pPass(new Retargeting());

    //loading from python
    //TODO: create phyton file & create variables + keys

    return pPass;
}

Dictionary Retargeting::getScriptingDictionary() const {

    Dictionary serialize;
    //TODO: create phyton file & create variables + keys
    return serialize;

}

RenderPassReflection Retargeting::reflect(void) const {

    RenderPassReflection r;
    //input
    r.addInput("input_seed", "resorted seeds from the sorting phase").texture2D(1920, 720).format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                        Resource::BindFlags::RenderTarget |
                                                                                                                                                                                                                        Resource::BindFlags::ShaderResource);
    //output
    r.addOutput("output_seed", "the retargeted seed texture outgoing to path tracer").texture2D(1920, 720).format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                                                       Resource::BindFlags::RenderTarget |
                                                                                                                                                                                                                                                        Resource::BindFlags::ShaderResource);
    /**r.addOutput("retarget","texture were our retargeting is stored").format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                     Resource::BindFlags::RenderTarget |
                                                                                                                                                                                                                    Resource::BindFlags::ShaderResource);*/
    return r;
}

void Retargeting::initialize(RenderContext * pContext, const RenderData * pRenderData) {

    //load prog from file
    mpComputeProg = ComputeProgram::createFromFile("Retarget.hlsl", "main");
    //initialize state
    mpComputeState = ComputeState::create();
    mpComputeState->setProgram(mpComputeProg);
    mpComputeProgVars = ComputeVars::create(mpComputeProg->getReflector());

    //textures for retargeting

    Texture::SharedPtr retarget = createTextureFromFile("retargeted_texture.png", false, false, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess|
                                                                                                                                                                                                                                                Resource::BindFlags::RenderTarget);
    mpComputeProgVars->setTexture("retarget_texture", retarget);

    //info for the frame
    // Send data to compute shader; first fill structured buffer
    mpComputeProgVars->setStructuredBuffer("data", StructuredBuffer::create(mpComputeProg, "data", 1));

    if (mpComputeProg != nullptr) mIsInitialized = true;

}

void Retargeting::execute(RenderContext* pContext, const RenderData* pData) {
    //on first run we want it to intialize
    if (!mIsInitialized) {

        initialize(pContext, pData);

    }

    //info for the frame
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_width"] = tile_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_height"] = tile_height;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_count"] = frame_count;

    mpComputeProgVars->setTexture("src_seed_texture", pData->getTexture("input_seed"));
    //set the putput seed tex in HLSL namespace!!
    mpComputeProgVars->setTexture("output_seed_texture", pData->getTexture("input_seed"));

    pContext->setComputeState(mpComputeState);
    pContext->setComputeVars(mpComputeProgVars);

    //implementation info from here : https://hal.archives-ouvertes.fr/hal-02158423/file/blueNoiseTemporal2019_slides.pdf 
    uint32_t groupSizeX = (frame_width / groupDimX) + 1;
    uint32_t groupSizeY = (frame_height / groupDimY) + 1;
    pContext->dispatch(groupSizeX, groupSizeY, 1);

    //set the outgoing blue noise texture!
    pContext->copyResource(pData->getTexture("output_seed").get(), pData->getTexture("input_seed").get());
}

void Retargeting::renderUI(Gui* pGui, const char* uiGroup) {
    //pGui->addText("Distributing Errors as Blue Noise");
    pGui->addCheckBox("Retarget Seeds", retargetSeeds);
}

void Retargeting::setScene(const std::shared_ptr<Scene>& pScene) {

}
void Retargeting::onResize(uint32_t width, uint32_t height) {
    //Upadate frame data 
    frame_width = width;
    frame_height = height;
}
