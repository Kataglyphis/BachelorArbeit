/***************************************************************************
# Copyright (c) 2019, Jonas Heinle. All rights reserved.
# RenderPass for generating a blue noise redistribution 
# refering to the research from Eric Heitz and Laurent Belcour
# https://eheitzresearch.wordpress.com/772-2/
***************************************************************************/

#include "BlueNoise.h"

// global varibles
namespace {
    //Shader files
    const char* sortShader = "Sort.hlsl";
    const char* retargetShader = "Retarget.hlsl";

    //
};

BlueNoise::SharedPtr BlueNoise::create(const Dictionary &params) {

    BlueNoise::SharedPtr pPass(new BlueNoise());

    //loading from python
    //TODO: create phyton file & create variables + keys

    return pPass;
}

Dictionary BlueNoise::getScriptingDictionary() const {

    Dictionary serialize;
    //TODO: create phyton file & create variables + keys
    return serialize;

}

RenderPassReflection BlueNoise::reflect(void) const {

    RenderPassReflection r;
    //input
    r.addInput("srcBlueNoise", "Source texture of our blue noise");
    r.addInput("srcSeeds", "Source texture of our seeds from path tracing");
    //output
    r.addOutput("dst", "").format(ResourceFormat::RGBA32Float).bindFlags(Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess | Resource::BindFlags::RenderTarget);

    return r;
}

void BlueNoise::initialize(RenderContext * pContext, const RenderData * pRenderData) {

    // clear render target view
    mpBlackHDR = Texture::create2D(128, 128, ResourceFormat::RGBA32Float, 1u, 1u, nullptr, Resource::BindFlags::ShaderResource | Resource::BindFlags::RenderTarget);
    pContext->clearRtv(mpBlackHDR->getRTV().get(), vec4(0.0f, 0.0f, 0.0f, 1.0f));

    //load prog from file
    mpProg = ComputeProgram::createFromFile("Sort.hlsl", "Sort");
    //initialize state
    mpState = ComputeState::create();
    mpState->setProgram(mpProg);
    mpProgVars = ComputeVars::create(mpProg->getReflector());

    if (mpProg != nullptr) {
        //mpProgVars = ComputeVars::create();
    }

    mIsInitialized = true;

}

void BlueNoise::execute(RenderContext* pContext, const RenderData* pData) {
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
    pCB["index"] =1 ;
    //mpProgVars->setTexture("",);
    //mpProgVars->setTexture("",);
    //mpProgVars->setTexture("",);

    
}

void BlueNoise::renderUI(Gui* pGui, const char* uiGroup) {
    //pGui->addText("Distributing Errors as Blue Noise");
    pGui->addCheckBox("Distributing Errors as Blue Noise", distributeAsBlueNoise);
}

void BlueNoise::setScene(const std::shared_ptr<Scene>& pScene) {

}
void BlueNoise::onResize(uint32_t width, uint32_t height) {

}
