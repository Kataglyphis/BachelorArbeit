/***************************************************************************
# Copyright (c) 2019, Jonas Heinle. All rights reserved.
# RenderPass for generating a blue noise redistribution
# refering to the research from Eric Heitz and Laurent Belcour
# https://eheitzresearch.wordpress.com/772-2/
***************************************************************************/

#include "Sorting.h"
#include "Graphics/TextureHelper.h"

// global varibles
namespace {
    //Shader files
    const char* sortShader = "Sort.hlsl";
};

Sorting::SharedPtr Sorting::create(const Dictionary &params) {

    Sorting::SharedPtr pPass(new Sorting());

    //loading from python
    //TODO: create phyton file & create variables + keys

    return pPass;
}

Dictionary Sorting::getScriptingDictionary() const {

    Dictionary serialize;
    //TODO: create phyton file & create variables + keys
    return serialize;

}

RenderPassReflection Sorting::reflect(void) const {

    RenderPassReflection r;
    //input
    r.addInput("frame_input", "rendered frame from path tracing").format(ResourceFormat::RGBA32Float).bindFlags(Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess | Resource::BindFlags::RenderTarget);
    r.addInput("seed_input", "the incoming seed texture");
    //ResourceFormat::RGBA8Uint
    //(color&0xff000000)>>24
    //(color&0x00ff0000)>>16


    //output
    r.addOutput("output_seed_texture", "the outgoing seed texture");

    return r;
}

void Sorting::initialize(RenderContext * pContext, const RenderData * pRenderData) {

    //load prog from file
    mpComputeProg = ComputeProgram::createFromFile("Sort.hlsl", "main");
    //initialize state
    mpComputeState = ComputeState::create();
    mpComputeState->setProgram(mpComputeProg);
    mpComputeProgVars = ComputeVars::create(mpComputeProg->getReflector());

    //initiallize textures
    //createTextureFromFile!!!!!
    Texture::SharedPtr bluenoise = createTextureFromFile("../Data/64_64/HDR_L_0.png", false, true);
    mpComputeProgVars->setTexture("input_blue_noise_texture", bluenoise);
    mpComputeProgVars->setTexture("input_seed_texture", pRenderData->getTexture("seed_input"));
    
    if (mpComputeProg != nullptr) {
        mIsInitialized = true;
    }


}

void Sorting::execute(RenderContext* pContext, const RenderData* pData) {
    //on first run we want it to intialize
    if (!mIsInitialized) {

        initialize(pContext, pData);

    }

    //info for the frame
    /**frameInfo = StructuredBuffer::create(mpComputeProg, "gInfo", 3);
    mpComputeProgVars->setStructuredBuffer("gInfo", frameInfo);
    mpComputeProgVars->getStructuredBuffer("gInfo")[0]["uintVal"] = (uint)1920;
    mpComputeProgVars->getStructuredBuffer("gInfo")[1]["uintVal"] = (uint)1080;
    mpComputeProgVars->getStructuredBuffer("gInfo")[2]["uintVal"] = (uint)5;*/

    mpComputeProgVars->setTexture("input_frame_texture",pData->getTexture("frameInput"));
    mpComputeProgVars->setTexture("input_seed_texture", pData->getTexture("seed_input"));
    mpComputeProgVars->setTexture("output_seed_texture", pData->getTexture("seed_input"));

    pContext->setComputeState(mpComputeState);
    pContext->setComputeVars(mpComputeProgVars);

    //implementation info from here : https://hal.archives-ouvertes.fr/hal-02158423/file/blueNoiseTemporal2019_slides.pdf 
    uint32_t w = 4;
    uint32_t h = 4;
    //Dispatch groupSizeX,GroupSizeY,GroupSizeZ;
    pContext->dispatch(w, h, 1);
    exit(1);
}

void Sorting::renderUI(Gui* pGui, const char* uiGroup) {
    //pGui->addText("Distributing Errors as Blue Noise");
    pGui->addCheckBox("Distributing Errors as Blue Noise", distributeAsBlueNoise);
}

void Sorting::setScene(const std::shared_ptr<Scene>& pScene) {

}
void Sorting::onResize(uint32_t width, uint32_t height) {

}
