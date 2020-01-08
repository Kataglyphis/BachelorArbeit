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
    r.addInput("seed_input", "the incoming seed texture").texture2D(1920, 720).bindFlags(/*Resource::BindFlags::ShaderResource |*/
                                                                                                                            Resource::BindFlags::UnorderedAccess |
                                                                                                                            Resource::BindFlags::RenderTarget);
    //ResourceFormat::RGBA8Uint
    //(color&0xff000000)>>24
    //(color&0x00ff0000)>>16

    //output
    r.addOutput("blue_noise", "our blue noise texture").texture2D(tile_width, tile_height).format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::ShaderResource |
                                                                                                                                                                                            Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                            Resource::BindFlags::RenderTarget);
    r.addOutput("seed_output", "the outgoing seed texture").texture2D(1920, 720).format(ResourceFormat::BGRA8Unorm).bindFlags(/*Resource::BindFlags::ShaderResource |*/
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

    //create vars and immeadiatley set vars
    //create PerFrameData
    //get the binding correct
    mpComputeProgVars = ComputeVars::create(mpComputeProg->getReflector());
    mpComputeProgVars->setStructuredBuffer("data", StructuredBuffer::create(mpComputeProg, "data", 1));

    //createTextureFromFile!!!!
    bluenoise = createTextureFromFile("LDR_RGBA_0_64.png", false, false, Resource::BindFlags::ShaderResource | Resource::BindFlags::UnorderedAccess | Resource::BindFlags::RenderTarget);
    mpComputeProgVars->setTexture("input_blue_noise_texture", bluenoise);
    //mpComputeProgVars->setTexture("input_seed_texture", pRenderData->getTexture("seed_input"));

    
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
    // Send data to compute shader; first fill structured buffer
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_width"] = tile_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_height"] = tile_height;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_count"] = frame_count;

    mpComputeProgVars->setTexture("input_frame_texture",pData->getTexture("frame_input"));
    mpComputeProgVars->setTexture("input_seed_texture", pData->getTexture("seed_input"));
    //mpComputeProgVars->setTexture("input_blue_noise_texture", bluenoise);
    //Texture::SharedPtr t2 = pData->getTexture("seed_input");
    //pData->getTexture("blue_noise") = mpComputeProgVars->getTexture("input_blue_noise_texture");
    Texture::SharedPtr b2 = pData->getTexture("blue_noise");

    pContext->setComputeState(mpComputeState);
    pContext->setComputeVars(mpComputeProgVars);

    //implementation info from here : https://hal.archives-ouvertes.fr/hal-02158423/file/blueNoiseTemporal2019_slides.pdf
    //Dispatch groupSizeX,GroupSizeY,GroupSizeZ;
    uint32_t groupSizeX = (frame_width / groupDimX) + 1;
    uint32_t groupSizeY = (frame_height / groupDimY) + 1;
    pContext->dispatch(groupSizeX, groupSizeY, 1);
    //Dispatch groupSizeX,GroupSizeY,GroupSizeZ;

    //set the outgoing seed texture for working in sorting step!
    pContext->copyResource(pData->getTexture("seed_output").get(), mpComputeProgVars->getTexture("input_seed_texture").get());
    //Texture::SharedPtr t = pData->getTexture("seed_output");
    //set the outgoing blue noise texture!
    pContext->copyResource(pData->getTexture("blue_noise").get(), bluenoise.get());
    //Texture::SharedPtr b = pData->getTexture("blue_noise");
    
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
