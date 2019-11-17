/***************************************************************************
# Copyright (c) 2019, Jonas Heinle. All rights reserved.
# RenderPass for generating a blue noise redistribution
# refering to the research from Eric Heitz and Laurent Belcour
# https://eheitzresearch.wordpress.com/772-2/
***************************************************************************/

#include "Sorting.h"

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
    r.addInput("srcSorting", "Source texture of our blue noise");
    r.addInput("srcSeeds", "Source texture of our seeds from path tracing");
    //output
    r.addOutput("dst", "").format(ResourceFormat::RGBA32Float).bindFlags(Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess | Resource::BindFlags::RenderTarget);

    return r;
}

void Sorting::initialize(RenderContext * pContext, const RenderData * pRenderData) {

    // clear render target view
    mpBlackHDR = Texture::create2D(128, 128, ResourceFormat::RGBA32Float, 1u, 1u, nullptr, Resource::BindFlags::ShaderResource | Resource::BindFlags::RenderTarget);
    pContext->clearRtv(mpBlackHDR->getRTV().get(), vec4(0.0f, 0.0f, 0.0f, 1.0f));

    //load prog from file
    mpProg = ComputeProgram::createFromFile("Sort.hlsl", "main");
    //initialize state
    mpState = ComputeState::create();
    mpState->setProgram(mpProg);
    mpProgVars = ComputeVars::create(mpProg->getReflector());

    if (mpProg != nullptr) {
        //mpProgVars = ComputeVars::create();
    }

    mIsInitialized = true;

}

void Sorting::execute(RenderContext* pContext, const RenderData* pData) {
    //on first run we want it to intialize
    if (!mIsInitialized) {

        initialize(pContext, pData);

    }

    // Get the output buffer an dclear it
    Texture::SharedPtr pDstTex = pData->getTexture("output");
    pContext->clearUAV(pDstTex->getUAV().get(), vec4(0.0f, 0.0f, 0.0f, 1.0f));

    if (pDstTex == nullptr) return;

    // Set our variables into the global HLSL namespace
    ConstantBuffer::SharedPtr pCB = mpProgVars->getConstantBuffer("GlobalCB");
    pCB["width"] = 1;
    pCB["height"] = 1;
    pCB["index"] = 1;
    //mpProgVars->setTexture("",);
    //mpProgVars->setTexture("",);
    //mpProgVars->setTexture("",);


}

void Sorting::renderUI(Gui* pGui, const char* uiGroup) {
    //pGui->addText("Distributing Errors as Blue Noise");
    pGui->addCheckBox("Distributing Errors as Blue Noise", distributeAsBlueNoise);
}

void Sorting::setScene(const std::shared_ptr<Scene>& pScene) {

}
void Sorting::onResize(uint32_t width, uint32_t height) {

}
