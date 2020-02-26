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
    r.addInput("input_frame", "rendered frame from path tracing").format(ResourceFormat::RGBA32Float).bindFlags(Resource::BindFlags::ShaderResource |
                                                                                                                                                                                                                Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                 Resource::BindFlags::RenderTarget);
    r.addInput("input_seed", "the incoming seed texture").texture2D(seed_texture_width, seed_texture_height).bindFlags(/*Resource::BindFlags::ShaderResource |*/
                                                                                                                            Resource::BindFlags::UnorderedAccess |
                                                                                                                            Resource::BindFlags::RenderTarget).mipLevels(1);

    //output
    /**r.addOutput("blue_noise", "our blue noise texture").texture2D(tile_width, tile_height).format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::ShaderResource |
                                                                                                                                                                                            Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                            Resource::BindFlags::RenderTarget);*/

    r.addOutput("output_seed", "the outgoing seed texture").texture2D(seed_texture_width, seed_texture_height).format(ResourceFormat::BGRA8Unorm).bindFlags(/*Resource::BindFlags::ShaderResource |*/
                                                                                                                                                                                                        Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                        Resource::BindFlags::RenderTarget).mipLevels(1);

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
    bluenoise = createTextureFromFile("LDR_RGBA_0_64.png", false, false, Resource::BindFlags::ShaderResource | /*Resource::BindFlags::UnorderedAccess |*/ Resource::BindFlags::RenderTarget);
    mpComputeProgVars->setTexture("input_blue_noise_texture", bluenoise);
   

    if (mpComputeProg != nullptr) {
        mIsInitialized = true;
    }

}

void Sorting::execute(RenderContext* pContext, const RenderData* pData) {

    //on first run we want it to intialize
    if (!mIsInitialized) {
        initialize(pContext, pData);
    }



    //update frame count
    //it is enough for this application to toroidally switch between 128 frame counts
    this->frame_count = frame_count % 128;

    //info for the frame
    // Send data to compute shader; first fill structured buffer
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_width"] = tile_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_height"] = tile_height;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_width"] = frame_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_height"] = frame_height;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_count"] = frame_count++;

    mpComputeProgVars->setTexture("input_frame_texture", pData->getTexture("input_frame"));
    mpComputeProgVars->setTexture("input_seed_texture", pData->getTexture("input_seed"));
    mpComputeProgVars->setTexture("output_seed_texture", pData->getTexture("output_seed"));
    mpComputeProgVars->setTexture("input_blue_noise_texture", bluenoise);

    pContext->setComputeState(mpComputeState);
    pContext->setComputeVars(mpComputeProgVars);

    //implementation info from here : https://hal.archives-ouvertes.fr/hal-02158423/file/blueNoiseTemporal2019_slides.pdf
    //Dispatch groupSizeX,GroupSizeY,GroupSizeZ;
    pContext->dispatch(numberOfGroupsX, numberOfGroupsY, 1);

    //set the outgoing seed texture for working in sorting step!
    if (sortingEnabled) {

        pContext->copyResource(pData->getTexture("output_seed").get(), mpComputeProgVars->getTexture("output_seed_texture").get());

    } else {

        pContext->copyResource(pData->getTexture("output_seed").get(), pData->getTexture("input_seed").get());

    }
    
}

void Sorting::renderUI(Gui* pGui, const char* uiGroup) {

    pGui->addCheckBox("Enable Sorting Pass", sortingEnabled);

}

void Sorting::setScene(const std::shared_ptr<Scene>& pScene) {
   
}

void Sorting::onResize(uint32_t width, uint32_t height) {

    //Upadate frame data 
    frame_width = width;
    frame_height = height;
}
