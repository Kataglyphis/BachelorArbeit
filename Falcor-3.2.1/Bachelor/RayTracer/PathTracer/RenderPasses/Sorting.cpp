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
    r.addInput("frameInput", "rendered frame from path tracing");
    r.addInput("inputSeedTexture", "the incoming seed texture");
    r.addInput("inputBlueNoiseTexture", "the incoming blue noise texture");
    //output
    r.addOutput("outputSeedTexture", "the outgoing seed texture");

    return r;
}

void Sorting::initialize(RenderContext * pContext, const RenderData * pRenderData) {

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
