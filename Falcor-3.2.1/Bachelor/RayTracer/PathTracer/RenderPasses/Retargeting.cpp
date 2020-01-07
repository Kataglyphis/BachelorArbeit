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
    r.addInput("input_seed", "resorted seeds from the sorting phase").format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                        Resource::BindFlags::RenderTarget |
                                                                                                                                                                                                                        Resource::BindFlags::ShaderResource);
    //output
    r.addOutput("output_seed", "the retargeted seed texture outgoing to path tracer").format(ResourceFormat::BGRA8Unorm).bindFlags(Resource::BindFlags::UnorderedAccess |
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

    //create PerFrameData
    const ParameterBlockReflection* pDefaultBlockReflection = mpComputeProg->getReflector()->getDefaultParameterBlock().get();
    mBindLocations.perFrameData = pDefaultBlockReflection->getResourceBinding("perFrameData");

    ParameterBlock* pDefaultBlock = mpComputeProgVars->getDefaultBlock().get();
    ConstantBuffer* pCB = pDefaultBlock->getConstantBuffer(mBindLocations.perFrameData, 0).get();
    width_offset = pCB->getVariableOffset("tile_width");
    height_offset = pCB->getVariableOffset("tile_height");
    frame_count_offset = pCB->getVariableOffset("frame_count");

    if (mpComputeProg != nullptr) mIsInitialized = true;

}

void Retargeting::execute(RenderContext* pContext, const RenderData* pData) {
    //on first run we want it to intialize
    if (!mIsInitialized) {

        initialize(pContext, pData);

    }

    //info for the frame
    ConstantBuffer* pCB = mpComputeProgVars->getDefaultBlock()->getConstantBuffer(mBindLocations.perFrameData, 0).get();
    pCB->setVariable("tile_width", tile_width);
    pCB->setVariable("tile_height", tile_height);
    pCB->setVariable("frame_count", frame_count++);

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
    auto texture = mpComputeProgVars->getTexture("output_seed_texture");
    int seed_texture_width = texture->getWidth();
    int seed_texture_height = texture->getHeight();
    auto type = texture->getType();
    auto handle = texture->getApiHandle();
    auto depth = texture->getDepth();
    auto format = texture->getFormat();
    auto sampleCount = texture->getSampleCount();
    auto array_size = texture->getArraySize();
    auto mip_levels = texture->getMipCount();
    auto flags = texture->getBindFlags();

    pData->getTexture("output_seed") = Texture::createFromApiHandle(handle, type, seed_texture_width, seed_texture_height, depth, format, sampleCount, array_size, mip_levels, Resource::State::UnorderedAccess, flags);
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
