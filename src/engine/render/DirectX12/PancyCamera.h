#pragma once
#include "core/core_module.h"
#include"PancyDx12Basic.h"
#define CameraForFPS 0
#define CameraForFly 1
class PancyCamera
{
	DirectX::XMFLOAT3 camera_right;    //摄像机的向右方向向量
	DirectX::XMFLOAT3 camera_look;     //摄像机的观察方向向量
	DirectX::XMFLOAT3 camera_up;       //摄像机的向上方向向量
	DirectX::XMFLOAT3 camera_position; //摄像机的所在位置向量
private:
	PancyCamera();
public:
	static PancyCamera* GetInstance()
	{
		static PancyCamera* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new PancyCamera();
		}
		return this_instance;
	}
	void RotationRight(float angle);                    //沿着向右向量旋转
	void RotationUp(float angle);                       //沿着向上向量旋转
	void RotationLook(float angle);                     //沿着观察向量旋转

	void RotationX(float angle);                    //沿着x轴向量旋转
	void RotationY(float angle);                    //沿着y轴向量旋转
	void RotationZ(float angle);                    //沿着z轴向量旋转

	void WalkFront(float distance);                     //摄像机向前平移
	void WalkRight(float distance);                     //摄像机向右平移
	void WalkUp(float distance);                        //摄像机向上平移
	void CountViewMatrix(DirectX::XMFLOAT4X4* view_matrix);     //计算取景矩阵
	void CountViewMatrix(DirectX::XMFLOAT3 rec_look, DirectX::XMFLOAT3 rec_up, DirectX::XMFLOAT3 rec_pos, DirectX::XMFLOAT4X4* matrix);
	void CountInvviewMatrix(DirectX::XMFLOAT4X4* inv_view_matrix);  //计算取景矩阵逆矩阵
	void CountInvviewMatrix(DirectX::XMFLOAT3 rec_look, DirectX::XMFLOAT3 rec_up, DirectX::XMFLOAT3 rec_pos, DirectX::XMFLOAT4X4* inv_view_matrix);  //计算取景矩阵逆矩阵

	void GetViewPosition(DirectX::XMFLOAT4* view_pos);
	void GetViewDirect(DirectX::XMFLOAT3* view_direct);
	void GetRightDirect(DirectX::XMFLOAT3* right_direct);
	void SetCamera(DirectX::XMFLOAT3 rec_look, DirectX::XMFLOAT3 rec_up, DirectX::XMFLOAT3 rec_pos);
	void ResetCamera();
private:
};