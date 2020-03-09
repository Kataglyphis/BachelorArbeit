/***************************************************************************
# Copyright (c) 2019, Jonas Heinle. All rights reserved.
# RenderPass for generating a blue noise redistribution
***************************************************************************/

#include "AverageMotionVector.h"
#include "Graphics/TextureHelper.h"

// global varibles
namespace {
    //Shader files
    //const char* retargetShader = "Retarget.hlsl";
};

AverageMotionVector::SharedPtr AverageMotionVector::create(const Dictionary &params) {

    AverageMotionVector::SharedPtr pPass(new AverageMotionVector());

    //loading from python
    //TODO: create phyton file & create variables + keys

    return pPass;
}

Dictionary AverageMotionVector::getScriptingDictionary() const {

    Dictionary serialize;
    //TODO: create phyton file & create variables + keys
    return serialize;

}

RenderPassReflection AverageMotionVector::reflect(void) const {

    RenderPassReflection r;
    //input
    r.addInput("input_depthStencil", "depth and stencil from g-buffer").format(ResourceFormat::D32Float).bindFlags(Resource::BindFlags::DepthStencil);

    //output
    r.addOutput("output_average_motion_vector", "the average motion vector for reprojection").texture2D(frame_width, frame_height).format(ResourceFormat::RG16Float).bindFlags
                                                                                                                                                                                                                                    (Resource::BindFlags::UnorderedAccess |
                                                                                                                                                                                                                                     Resource::BindFlags::RenderTarget |
                                                                                                                                                                                                                                     Resource::BindFlags::ShaderResource).mipLevels(1);

    return r;
}

void AverageMotionVector::initialize(RenderContext * pContext, const RenderData * pRenderData) {

    //load prog from file
    mpComputeProg = ComputeProgram::createFromFile("CalcAverageMotionVector.hlsl", "main");
    //initialize state
    mpComputeState = ComputeState::create();
    mpComputeState->setProgram(mpComputeProg);
    mpComputeProgVars = ComputeVars::create(mpComputeProg->getReflector());

    //info for the frame
    // Send data to compute shader; first fill structured buffer
    mpComputeProgVars->setStructuredBuffer("data", StructuredBuffer::create(mpComputeProg, "data", 1));

    if (mpComputeProg != nullptr) mIsInitialized = true;

    this->mpCurrViewProjMatrix = mat4x4(0.f);
    this->mpPrevViewProjMatrix = mat4x4(0.f);

}

void AverageMotionVector::execute(RenderContext* pContext, const RenderData* pData) {
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

    //info for the frame
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_width"] = frame_width;
    mpComputeProgVars->getStructuredBuffer("data")[0]["frame_height"] = frame_height;
    mpComputeProgVars->getStructuredBuffer("data")[0]["camera_moved"] = camera_moved;
    mpComputeProgVars->getStructuredBuffer("data")[0]["Inverse_VP_prev_frame"] = mpPrevViewProjMatrixInv;
    mpComputeProgVars->getStructuredBuffer("data")[0]["VP_curr_frame"] = mpCurrViewProjMatrix;

    mpComputeProgVars->setTexture("depth", pData->getTexture("input_depthStencil"));
    mpComputeProgVars->setTexture("output_average_motion_vector", pData->getTexture("output_average_motion_vector"));

    pContext->setComputeState(mpComputeState);
    pContext->setComputeVars(mpComputeProgVars);

    pContext->dispatch(frame_width, frame_height, 1);
}

void AverageMotionVector::renderUI(Gui* pGui, const char* uiGroup) {

}

void AverageMotionVector::setScene(const std::shared_ptr<Scene>& pScene) {

    this->mpScene = pScene;
    // Grab a copy of the current scene's camera matrix (if it exists)
    if (mpScene && mpScene->getActiveCamera()) {

        mpPrevViewProjMatrix = mpCurrViewProjMatrix;
        mpPrevViewProjMatrixInv = mpCurrViewProjMatrixInv;
        mpCurrViewProjMatrix = mpScene->getActiveCamera()->getViewProjMatrix();
        mpCurrViewProjMatrixInv = mpScene->getActiveCamera()->getInvViewProjMatrix();
    }

}

void AverageMotionVector::onResize(uint32_t width, uint32_t height) {

    //Upadate frame data 
    frame_width = width;
    frame_height = height;

}

bool AverageMotionVector::hasCameraMoved()
{
    // Has our camera moved?
    return mpScene &&                   // No scene?  Then the answer is no
        mpScene->getActiveCamera() &&   // No camera in our scene?  Then the answer is no
        (mpCurrViewProjMatrix != mpScene->getActiveCamera()->getViewProjMatrix());   // Compare the current matrix with the last one
}
