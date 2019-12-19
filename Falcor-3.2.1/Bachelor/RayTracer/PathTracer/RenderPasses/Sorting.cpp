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
    r.addInput("frame_input", "rendered frame from path tracing").format(ResourceFormat::RGBA32Float).bindFlags(Resource::BindFlags::ShaderResource |
                                                                                                                                                                                                                Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                 Resource::BindFlags::RenderTarget);
    r.addInput("seed_input", "the incoming seed texture").bindFlags(Resource::BindFlags::ShaderResource |
        Resource::BindFlags::UnorderedAccess |
        Resource::BindFlags::RenderTarget);;
    //ResourceFormat::RGBA8Uint
    //(color&0xff000000)>>24
    //(color&0x00ff0000)>>16

    //output
    r.addOutput("blue_noise", "our blue noise texture").format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::ShaderResource |
                                                                                                                                                                                            Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                            Resource::BindFlags::RenderTarget);
    r.addOutput("seed_output", "the outgoing seed texture").format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::ShaderResource |
                                                                                                                                                                                                        Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                        Resource::BindFlags::RenderTarget);

    return r;
}

void Sorting::initialize(RenderContext * pContext, const RenderData * pRenderData) {
   
    //load prog from file
    mpComputeProg = ComputeProgram::createFromFile("Sort.hlsl", "main");
    //initialize state
    mpComputeState = ComputeState::create();
    mpComputeState->setProgram(mpComputeProg);

    Falcor::ProgramReflection::SharedConstPtr reflector = mpComputeProg->getReflector();

    mpComputeProgVars = ComputeVars::create(mpComputeProg->getReflector());

    //createTextureFromFile!!!!
    Texture::SharedPtr bluenoise = createTextureFromFile("Tiled.png", false, true, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess | Resource::BindFlags::RenderTarget);
    mpComputeProgVars->setTexture("input_blue_noise_texture", bluenoise);
    mpComputeProgVars->setTexture("input_seed_texture", pRenderData->getTexture("seed_input"));

    //create PerFrameData
    const ParameterBlockReflection* pDefaultBlockReflection = mpComputeProg->getReflector()->getDefaultParameterBlock().get();
    perFrameData = pDefaultBlockReflection->getResourceBinding("perFrameData");

    ParameterBlock* pDefaultBlock = mpComputeProgVars->getDefaultBlock().get();
    ConstantBuffer* pCB = pDefaultBlock->getConstantBuffer(perFrameData, 0).get();
    width_offset = pCB->getVariableOffset("width");
    height_offset = pCB->getVariableOffset("height");
    frame_count_offset = pCB->getVariableOffset("frame_count");
    
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
    ConstantBuffer* pCB = mpComputeProgVars->getDefaultBlock()->getConstantBuffer(perFrameData, 0).get();
    pCB->setVariable("width", frame_width);
    pCB->setVariable("height", frame_height);
    pCB->setVariable("frame_count", frame_count++);
    
    mpComputeProgVars->setTexture("input_frame_texture",pData->getTexture("frame_input"));
    mpComputeProgVars->setTexture("input_seed_texture", pData->getTexture("seed_input"));

    pContext->setComputeState(mpComputeState);
    pContext->setComputeVars(mpComputeProgVars);

    //implementation info from here : https://hal.archives-ouvertes.fr/hal-02158423/file/blueNoiseTemporal2019_slides.pdf
    //Dispatch groupSizeX,GroupSizeY,GroupSizeZ;
    uint32_t groupSizeX = (frame_width / groupDimX) + 1;
    uint32_t groupSizeY = (frame_height / groupDimY) + 1;
    pContext->dispatch(groupSizeX, groupSizeY, 1);
    //Dispatch groupSizeX,GroupSizeY,GroupSizeZ;

}

void Sorting::renderUI(Gui* pGui, const char* uiGroup) {

    pGui->addCheckBox("Distributing Errors as Blue Noise", distributeAsBlueNoise);
}

void Sorting::setScene(const std::shared_ptr<Scene>& pScene) {
   
}
void Sorting::onResize(uint32_t width, uint32_t height) {

    //Upadate frame data 
    frame_width = width;
    frame_height = height;
}
