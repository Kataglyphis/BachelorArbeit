/***************************************************************************
# Copyright (c) 2019, Jonas Heinle. All rights reserved.
# RenderPass for generating a blue noise redistribution
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
    r.addInput("input_seed", "resorted seeds from the sorting phase").texture2D(seed_texture_width, seed_texture_height).format(ResourceFormat::BGRA8Unorm).bindFlags
                                                                                                                                                                                                                        (Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                        Resource::BindFlags::RenderTarget |
                                                                                                                                                                                                                        Resource::BindFlags::ShaderResource).mipLevels(1);

    r.addInput("input_depthStencil", "depth and stencil from g-buffer").format(ResourceFormat::D32Float).bindFlags(Resource::BindFlags::DepthStencil);

    r.addInput("input_average_motion_vector", "the average motion vector for reprojection").texture2D(frame_width, frame_height).format(ResourceFormat::RG16Float).bindFlags
                                                                                                                                                                                                                                (Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                                 Resource::BindFlags::RenderTarget |
                                                                                                                                                                                                                                 Resource::BindFlags::ShaderResource).mipLevels(1);

    //output
    r.addOutput("output_seed", "the retargeted seed texture outgoing to path tracer").texture2D(seed_texture_width, seed_texture_height).format(ResourceFormat::BGRA8Unorm).bindFlags
                                                                                                                                                                                                                                              (Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                                              Resource::BindFlags::RenderTarget |
                                                                                                                                                                                                                                              Resource::BindFlags::ShaderResource).mipLevels(1);
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
    //30; ; 238866; 228324, 47
    Texture::SharedPtr retarget = createTextureFromFile("permutation_texture_1373806_swapsKirkpatrickCooldownSchedule.png", false, false, Resource::BindFlags::ShaderResource
                                                                                                                                                                                                                                            | /*Resource::BindFlags::UnorderedAccess|*/
                                                                                                                                                                                                                                            Resource::BindFlags::RenderTarget);
    //mpComputeProgVars->setTexture("retarget_texture0x0", retarget);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////// load textures for the temporal reprojection phase!!!
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i = -dim_motion_vec; i < dim_motion_vec; i++) {
        for (int j = -dim_motion_vec; j < dim_motion_vec; j++) {
            std::string name = std::to_string(i) + "x" + std::to_string(j) + "_permutation_texture.png";
            auto retarget_texture = createTextureFromFile(temporal_reprojection_base_folder + name, false, false, Resource::BindFlags::ShaderResource
                                                                                                                                                                                      | Resource::BindFlags::RenderTarget);
            std::string shader_name;
            if (i < 0 && j < 0) {
                shader_name = "temporal_projecting" + '_' + std::to_string(i) + "x" + '_' + std::to_string(j);
            }
            else if (i < 0 && j >= 0) {
                shader_name = "temporal_projecting" + '_' + std::to_string(i) + "x" + std::to_string(j);
            }
            else if (i >= 0 && j < 0) {
                shader_name = "temporal_projecting" + std::to_string(i) + "x" + '_' + std::to_string(j);
            }
            else {
                shader_name = "temporal_projecting" + std::to_string(i) + "x" + std::to_string(j);
            }
            mpComputeProgVars->setTexture(shader_name, retarget_texture);
        }

    }
    /*retarget_texture0x1 = createTextureFromFile(temporal_reprojection_base_folder + "0x1/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
                                                                                                                                                                                                                        | 
                                                                                                                                                                                                                            Resource::BindFlags::RenderTarget);
    retarget_texture0x2 = createTextureFromFile(temporal_reprojection_base_folder + "0x2/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        |
        Resource::BindFlags::RenderTarget);
    retarget_texture0x3 = createTextureFromFile(temporal_reprojection_base_folder + "0x3/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        | 
        Resource::BindFlags::RenderTarget);
    retarget_texture1x0 = createTextureFromFile(temporal_reprojection_base_folder + "1x0/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        | 
        Resource::BindFlags::RenderTarget);
    retarget_texture1x1 = createTextureFromFile(temporal_reprojection_base_folder + "1x1/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        | 
        Resource::BindFlags::RenderTarget);
    retarget_texture1x2 = createTextureFromFile(temporal_reprojection_base_folder + "1x2/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        | 
        Resource::BindFlags::RenderTarget);
    retarget_texture1x3 = createTextureFromFile(temporal_reprojection_base_folder + "1x3/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        |
        Resource::BindFlags::RenderTarget);
    retarget_texture2x0 = createTextureFromFile(temporal_reprojection_base_folder + "2x0/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        | 
        Resource::BindFlags::RenderTarget);
    retarget_texture2x1 = createTextureFromFile(temporal_reprojection_base_folder + "2x1/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        |
        Resource::BindFlags::RenderTarget);
    retarget_texture2x2 = createTextureFromFile(temporal_reprojection_base_folder + "2x2/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        | 
        Resource::BindFlags::RenderTarget);
    retarget_texture2x3 = createTextureFromFile(temporal_reprojection_base_folder + "2x3/" + "permutation_texture.png", false, false, Resource::BindFlags::ShaderResource
        | /*Resource::BindFlags::UnorderedAccess|
        Resource::BindFlags::RenderTarget);

    mpComputeProgVars->setTexture("retarget_texture0x1", retarget_texture0x1);
    mpComputeProgVars->setTexture("retarget_texture0x2", retarget_texture0x2);
    mpComputeProgVars->setTexture("retarget_texture0x3", retarget_texture0x3);
    mpComputeProgVars->setTexture("retarget_texture1x0", retarget_texture1x0);
    mpComputeProgVars->setTexture("retarget_texture1x1", retarget_texture1x1);
    mpComputeProgVars->setTexture("retarget_texture1x2", retarget_texture1x2);
    mpComputeProgVars->setTexture("retarget_texture1x3", retarget_texture1x3);
    mpComputeProgVars->setTexture("retarget_texture2x0", retarget_texture2x0);
    mpComputeProgVars->setTexture("retarget_texture2x1", retarget_texture2x1);
    mpComputeProgVars->setTexture("retarget_texture2x2", retarget_texture2x2);
    mpComputeProgVars->setTexture("retarget_texture2x3", retarget_texture2x3);*/
    
    mpComputeProgVars->setTexture("retarget_texture", retarget);

    //info for the frame
    // Send data to compute shader; first fill structured buffer
    mpComputeProgVars->setStructuredBuffer("data", StructuredBuffer::create(mpComputeProg, "data", 1));

    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_width"] = tile_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["tile_height"] = tile_height;

    if (mpComputeProg != nullptr) mIsInitialized = true;

    //retargeting pass is initialized in the beginning!
    this->enable_retarget_pass_shader_var = this->enableRetargetingPass ? 1 : 0;
    this->enable_temporal_reprojection_pass_shader_var = this->enableTemporalReprojectionPass ? 1 : 0;

    this->mpCurrViewProjMatrix = mat4x4(0.f);
    this->mpPrevViewProjMatrix = mat4x4(0.f);

}

void Retargeting::execute(RenderContext* pContext, const RenderData* pData) {
    //on first run we want it to intialize
    if (!mIsInitialized) {
        initialize(pContext, pData);
    }
    
    bool camera_moved = hasCameraMoved();

    if (camera_moved) {

        mpPrevViewProjMatrix = mpCurrViewProjMatrix;
        mpPrevViewProjMatrixInv = mpCurrViewProjMatrixInv;
        mpCurrViewProjMatrix = mpScene->getActiveCamera()->getViewProjMatrix();
        mpCurrViewProjMatrixInv = mpScene->getActiveCamera()->getInvViewProjMatrix();

    }

    //update frame count
    //it is enough for this application to toroidally switch between 128 frame counts
    if(frame_count > 127) this->frame_count = frame_count % 128;

    //info for the frame
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_width"] = frame_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_height"] = frame_height;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_count"] = frame_count++;
    mpComputeProgVars->getStructuredBuffer("data")[0]["enable_retargeting"] = this->enable_retarget_pass_shader_var;
    mpComputeProgVars->getStructuredBuffer("data")[0]["enable_temporal_reprojection"] = this->enable_temporal_reprojection_pass_shader_var;
    mpComputeProgVars->getStructuredBuffer("data")[0]["camera_moved"] = camera_moved;

    mpComputeProgVars->setTexture("input_average_motion_vector", pData->getTexture("input_average_motion_vector"));
    mpComputeProgVars->setTexture("src_seed_texture", pData->getTexture("input_seed"));
    mpComputeProgVars->setTexture("output_seed_texture", pData->getTexture("output_seed"));

    pContext->setComputeState(mpComputeState);
    pContext->setComputeVars(mpComputeProgVars);

    //we will start with retargeting if first sorting has happend !!!!
    if ((frame_count >= 0) && (enableRetargetingPass || enableTemporalReprojectionPass)) pContext->dispatch(numberOfGroupsX, numberOfGroupsY, 1);
    
    if (!this->enableRetargetingPass && !this->enableTemporalReprojectionPass) pContext->copyResource(pData->getTexture("output_seed").get(), pData->getTexture("input_seed").get());

}

void Retargeting::renderUI(Gui* pGui, const char* uiGroup) {

    pGui->addCheckBox("Dis-/Enable the retargeting pass", enableRetargetingPass);
    pGui->addCheckBox("Dis-/Enable retargeting with additional temporal reprojection", enableTemporalReprojectionPass);

    enable_retarget_pass_shader_var = enableRetargetingPass ? 1 : 0;
    enable_temporal_reprojection_pass_shader_var = enableTemporalReprojectionPass ? 1 : 0;
}

void Retargeting::setScene(const std::shared_ptr<Scene>& pScene) {

    this->mpScene = pScene;
    // Grab a copy of the current scene's camera matrix (if it exists)
    if (mpScene && mpScene->getActiveCamera()) {

        mpPrevViewProjMatrix = mpCurrViewProjMatrix;
        mpPrevViewProjMatrixInv = mpCurrViewProjMatrixInv;
        mpCurrViewProjMatrix = mpScene->getActiveCamera()->getViewProjMatrix();
        mpCurrViewProjMatrixInv = mpScene->getActiveCamera()->getInvViewProjMatrix();
    }

}

void Retargeting::onResize(uint32_t width, uint32_t height) {
    //Upadate frame data 
    frame_width = width;
    frame_height = height;
}

bool Retargeting::hasCameraMoved()
{
   
    // Has our camera moved?
    return mpScene &&                   // No scene?  Then the answer is no
        mpScene->getActiveCamera() &&   // No camera in our scene?  Then the answer is no
        (mpCurrViewProjMatrix != mpScene->getActiveCamera()->getViewProjMatrix());   // Compare the current matrix with the last one
}
