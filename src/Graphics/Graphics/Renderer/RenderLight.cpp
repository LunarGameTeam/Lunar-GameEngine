#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Asset/MeshAsset.h"

namespace luna::render
{


void DirectionLight::Init()
{
	mParamBuffer = std::make_shared<ShaderParamsBuffer>(sRenderModule->GetRenderDevice()->mDefaultShader->GetConstantBufferDesc(LString("ViewBuffer").Hash()));
}

void DirectionLight::Update(RenderView* view)
{
	LTransform transform = LTransform::Identity();
	auto translate = LMath::GetMatrixTranslaton(view->GetViewMatrix());
	transform.translate(translate);
	auto rota = LQuaternion::FromTwoVectors(LVector3f(1, 0, 0), mDirection);
	transform.rotate(rota);
	LMatrix4f viewMat = transform.matrix().inverse();
	LMatrix4f proj;
	LMath::GenOrthoLHMatrix(proj, 30, 30, 0.01, 50);
	mParamBuffer->Set("cViewMatrix", viewMat);
	mParamBuffer->Set("cProjectionMatrix", proj);
	view->mOwnerScene->mSceneParamsBuffer->Set("cLightViewMatrix", viewMat, 0);
	view->mOwnerScene->mSceneParamsBuffer->Set("cLightProjMatrix", proj, 0);
	BaseVertex b1;
	b1.pos = translate;
	BaseVertex b2;
	b2.pos = translate;
	b2.pos.x() = b2.pos.x() + 3;

	view->mOwnerScene->mDebugMeshLine->AddLine(b1, b2);
	mParamBuffer->Commit();
}

}