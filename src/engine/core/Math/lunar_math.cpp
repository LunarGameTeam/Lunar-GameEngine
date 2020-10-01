#include "lunar_math.h"
//3*3矩阵
LunarEngine::LunarMatrix3x3::LunarMatrix3x3()
{
	DirectX::XMMATRIX now_mat_value = DirectX::XMLoadFloat3x3(&matrix_value);
	DirectX::XMMATRIX now_identity_value = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat3x3(&matrix_value, now_identity_value);
}
LunarEngine::LunarMatrix3x3 LunarEngine::LunarMatrix3x3::operator*(const LunarEngine::LunarMatrix3x3& matrix_right)
{
	LunarEngine::LunarMatrix3x3 new_mat;
	DirectX::XMMATRIX now_mat_left = DirectX::XMLoadFloat3x3(&matrix_value);
	DirectX::XMMATRIX now_mat_right = DirectX::XMLoadFloat3x3(&matrix_right.matrix_value);
	DirectX::XMStoreFloat3x3(&new_mat.matrix_value, now_mat_left * now_mat_right);
	return new_mat;
}
LunarEngine::LunarMatrix3x3 LunarEngine::LunarMatrix3x3::TransposeMatrix()
{
	LunarEngine::LunarMatrix3x3 new_mat;
	DirectX::XMMATRIX now_mat_value = DirectX::XMLoadFloat3x3(&matrix_value);
	DirectX::XMStoreFloat3x3(&new_mat.matrix_value, DirectX::XMMatrixTranspose(now_mat_value));
	return new_mat;
}
LunarEngine::LunarMatrix3x3 LunarEngine::LunarMatrix3x3::InverseMatrix()
{
	LunarEngine::LunarMatrix3x3 new_mat;
	DirectX::XMMATRIX now_mat_value = DirectX::XMLoadFloat3x3(&matrix_value);
	DirectX::XMVECTOR det;
	DirectX::XMStoreFloat3x3(&new_mat.matrix_value, DirectX::XMMatrixInverse(&det, now_mat_value));
	return new_mat;
}
//4*4矩阵
LunarEngine::LunarMatrix4x4::LunarMatrix4x4()
{
	DirectX::XMMATRIX now_mat_value = DirectX::XMLoadFloat4x4(&matrix_value);
	DirectX::XMMATRIX now_identity_value = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&matrix_value, now_identity_value);
}
LunarEngine::LunarMatrix4x4 LunarEngine::LunarMatrix4x4::operator*(const LunarEngine::LunarMatrix4x4& matrix_right)
{
	LunarEngine::LunarMatrix4x4 new_mat;
	DirectX::XMMATRIX now_mat_left = DirectX::XMLoadFloat4x4(&matrix_value);
	DirectX::XMMATRIX now_mat_right = DirectX::XMLoadFloat4x4(&matrix_right.matrix_value);
	DirectX::XMStoreFloat4x4(&new_mat.matrix_value, now_mat_left * now_mat_right);
	return new_mat;
}
LunarEngine::LunarMatrix4x4 LunarEngine::LunarMatrix4x4::TransposeMatrix()
{
	LunarEngine::LunarMatrix4x4 new_mat;
	DirectX::XMMATRIX now_mat_value = DirectX::XMLoadFloat4x4(&matrix_value);
	DirectX::XMStoreFloat4x4(&new_mat.matrix_value, DirectX::XMMatrixTranspose(now_mat_value));
	return new_mat;
}
LunarEngine::LunarMatrix4x4 LunarEngine::LunarMatrix4x4::InverseMatrix()
{
	LunarEngine::LunarMatrix4x4 new_mat;
	DirectX::XMMATRIX now_mat_value = DirectX::XMLoadFloat4x4(&matrix_value);
	DirectX::XMVECTOR det;
	DirectX::XMStoreFloat4x4(&new_mat.matrix_value, DirectX::XMMatrixInverse(&det, now_mat_value));
	return new_mat;
}
//二位向量
LunarEngine::LunarVector2::LunarVector2()
{
	vector_value.x = 0;
	vector_value.y = 0;
}
LunarEngine::LunarVector2::LunarVector2(const float& x, const float& y)
{
	vector_value.x = x;
	vector_value.y = y;
}
LunarEngine::LunarVector2 LunarEngine::LunarVector2::operator+(const LunarEngine::LunarVector2& vector_right)
{
	LunarEngine::LunarVector2 new_vector;
	new_vector.vector_value.x = vector_value.x + vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y + vector_right.vector_value.y;
	return new_vector;
}
LunarEngine::LunarVector2 LunarEngine::LunarVector2::operator-(const LunarEngine::LunarVector2& vector_right)
{
	LunarEngine::LunarVector2 new_vector;
	new_vector.vector_value.x = vector_value.x - vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y - vector_right.vector_value.y;
	return new_vector;
}
LunarEngine::LunarVector2 LunarEngine::LunarVector2::operator*(const LunarEngine::LunarVector2& vector_right)
{
	LunarEngine::LunarVector2 new_vector;
	new_vector.vector_value.x = vector_value.x * vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y * vector_right.vector_value.y;
	return new_vector;
}
float LunarEngine::LunarVector2::DotValue(const LunarEngine::LunarVector2& vector_right)
{
	return vector_value.x * vector_right.vector_value.x + vector_value.y * vector_right.vector_value.y;
}
float LunarEngine::LunarVector2::CrossVec(const LunarEngine::LunarVector2& vector_right)
{
	return vector_value.x * vector_right.vector_value.y - vector_value.y * vector_right.vector_value.x;
}
LunarEngine::LunarVector2 LunarEngine::LunarVector2::NormalizeVec()
{
	LunarEngine::LunarVector2 new_vector;
	DirectX::XMVECTOR now_vec_left = DirectX::XMLoadFloat2(&vector_value);
	DirectX::XMVECTOR now_vec_ans = DirectX::XMVector2Normalize(now_vec_left);
	DirectX::XMStoreFloat2(&new_vector.vector_value, now_vec_ans);
	return new_vector;
}
//三维向量
LunarEngine::LunarVector3::LunarVector3()
{
	vector_value.x = 0;
	vector_value.y = 0;
	vector_value.z = 0;
}
LunarEngine::LunarVector3::LunarVector3(const float& x, const float& y, const float& z)
{
	vector_value.x = x;
	vector_value.y = y;
	vector_value.z = z;
}
LunarEngine::LunarVector3 LunarEngine::LunarVector3::operator+(const LunarEngine::LunarVector3& vector_right)
{
	LunarEngine::LunarVector3 new_vector;
	new_vector.vector_value.x = vector_value.x + vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y + vector_right.vector_value.y;
	new_vector.vector_value.z = vector_value.z + vector_right.vector_value.z;
	return new_vector;
}
LunarEngine::LunarVector3 LunarEngine::LunarVector3::operator-(const LunarEngine::LunarVector3& vector_right)
{
	LunarEngine::LunarVector3 new_vector;
	new_vector.vector_value.x = vector_value.x - vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y - vector_right.vector_value.y;
	new_vector.vector_value.z = vector_value.z - vector_right.vector_value.z;
	return new_vector;
}
LunarEngine::LunarVector3 LunarEngine::LunarVector3::operator*(const LunarEngine::LunarVector3& vector_right)
{
	LunarEngine::LunarVector3 new_vector;
	new_vector.vector_value.x = vector_value.x * vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y * vector_right.vector_value.y;
	new_vector.vector_value.z = vector_value.z * vector_right.vector_value.z;
	return new_vector;
}
LunarEngine::LunarVector3 LunarEngine::LunarVector3::operator*(const LunarEngine::LunarMatrix3x3& matrix_right)
{
	LunarEngine::LunarVector3 new_vector;
	DirectX::XMVECTOR now_vec_data = DirectX::XMLoadFloat3(&vector_value);
	DirectX::XMMATRIX now_matrix_data = DirectX::XMLoadFloat3x3(&matrix_right.GetMatrixValue());
	DirectX::XMVECTOR result = DirectX::XMVector3Transform(now_vec_data, now_matrix_data);
	DirectX::XMFLOAT3 value_out;
	DirectX::XMStoreFloat3(&value_out, result);
	new_vector.SetValue(value_out.x, value_out.y, value_out.z);
	return new_vector;
}
LunarEngine::LunarVector3 LunarEngine::LunarVector3::operator*(const LunarEngine::LunarMatrix4x4& matrix_right)
{
	LunarEngine::LunarVector3 new_vector;
	DirectX::XMVECTOR now_vec_data = DirectX::XMLoadFloat3(&vector_value);
	DirectX::XMMATRIX now_matrix_data = DirectX::XMLoadFloat4x4(&matrix_right.GetMatrixValue());
	DirectX::XMVECTOR result = DirectX::XMVector3TransformCoord(now_vec_data, now_matrix_data);
	DirectX::XMFLOAT3 value_out;
	DirectX::XMStoreFloat3(&value_out, result);
	new_vector.SetValue(value_out.x, value_out.y, value_out.z);
	return new_vector;
}
float LunarEngine::LunarVector3::DotValue(const LunarVector3& vector_right)
{
	return vector_value.x * vector_right.vector_value.x + vector_value.y * vector_right.vector_value.y + vector_value.z * vector_right.vector_value.z;
}
LunarEngine::LunarVector3 LunarEngine::LunarVector3::CrossVec(const LunarEngine::LunarVector3& vector_right)
{
	LunarEngine::LunarVector3 new_vector;
	DirectX::XMVECTOR now_vec_left = DirectX::XMLoadFloat3(&vector_value);
	DirectX::XMVECTOR now_vec_right = DirectX::XMLoadFloat3(&vector_right.vector_value);
	DirectX::XMVECTOR now_vec_ans = DirectX::XMVector3Cross(now_vec_left, now_vec_right);
	DirectX::XMStoreFloat3(&new_vector.vector_value, now_vec_ans);
	return new_vector;
}
LunarEngine::LunarVector3 LunarEngine::LunarVector3::NormalizeVec()
{
	LunarEngine::LunarVector3 new_vector;
	DirectX::XMVECTOR now_vec_left = DirectX::XMLoadFloat3(&vector_value);
	DirectX::XMVECTOR now_vec_ans = DirectX::XMVector3Normalize(now_vec_left);
	DirectX::XMStoreFloat3(&new_vector.vector_value, now_vec_ans);
	return new_vector;
}
//四维向量
LunarEngine::LunarVector4::LunarVector4()
{
	vector_value.x = 0;
	vector_value.y = 0;
	vector_value.z = 0;
	vector_value.w = 0;
}
LunarEngine::LunarVector4::LunarVector4(const float& x, const float& y, const float& z, const float& w)
{
	vector_value.x = x;
	vector_value.y = y;
	vector_value.z = z;
	vector_value.w = w;
}
LunarEngine::LunarVector4 LunarEngine::LunarVector4::operator+(const LunarEngine::LunarVector4& vector_right)
{
	LunarEngine::LunarVector4 new_vector;
	new_vector.vector_value.x = vector_value.x + vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y + vector_right.vector_value.y;
	new_vector.vector_value.z = vector_value.z + vector_right.vector_value.z;
	new_vector.vector_value.w = vector_value.w + vector_right.vector_value.w;
	return new_vector;
}
LunarEngine::LunarVector4 LunarEngine::LunarVector4::operator-(const LunarEngine::LunarVector4& vector_right)
{
	LunarEngine::LunarVector4 new_vector;
	new_vector.vector_value.x = vector_value.x - vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y - vector_right.vector_value.y;
	new_vector.vector_value.z = vector_value.z - vector_right.vector_value.z;
	new_vector.vector_value.w = vector_value.w - vector_right.vector_value.w;
	return new_vector;
}
LunarEngine::LunarVector4 LunarEngine::LunarVector4::operator*(const LunarEngine::LunarVector4& vector_right)
{
	LunarEngine::LunarVector4 new_vector;
	new_vector.vector_value.x = vector_value.x * vector_right.vector_value.x;
	new_vector.vector_value.y = vector_value.y * vector_right.vector_value.y;
	new_vector.vector_value.z = vector_value.z * vector_right.vector_value.z;
	new_vector.vector_value.w = vector_value.w * vector_right.vector_value.w;
	return new_vector;
}
LunarEngine::LunarVector4 LunarEngine::LunarVector4::operator*(const LunarEngine::LunarMatrix4x4& matrix_right)
{
	LunarEngine::LunarVector4 new_vector;
	DirectX::XMVECTOR now_vec_data = DirectX::XMLoadFloat4(&vector_value);
	DirectX::XMMATRIX now_matrix_data = DirectX::XMLoadFloat4x4(&matrix_right.GetMatrixValue());
	DirectX::XMVECTOR result = DirectX::XMVector4Transform(now_vec_data, now_matrix_data);
	DirectX::XMStoreFloat4(&new_vector.vector_value, result);
	return new_vector;
}
float LunarEngine::LunarVector4::DotValue3(const LunarEngine::LunarVector4& vector_right)
{
	return vector_value.x * vector_right.vector_value.x + vector_value.y * vector_right.vector_value.y + vector_value.z * vector_right.vector_value.z;
}
float LunarEngine::LunarVector4::DotValue4(const LunarEngine::LunarVector4& vector_right)
{
	return vector_value.x * vector_right.vector_value.x + vector_value.y * vector_right.vector_value.y + vector_value.z * vector_right.vector_value.z + vector_value.w * vector_right.vector_value.w;
}
LunarEngine::LunarVector3 LunarEngine::LunarVector4::CrossVec(const LunarEngine::LunarVector3& vector_right)
{
	LunarEngine::LunarVector3 now_vec3_left = GetVector3();
	return now_vec3_left.CrossVec(vector_right);
}
LunarEngine::LunarVector3 LunarEngine::LunarVector4::CrossVec(const LunarEngine::LunarVector4& vector_right)
{
	LunarEngine::LunarVector3 now_vec3_left = GetVector3();
	LunarEngine::LunarVector3 now_vec3_right = vector_right.GetVector3();
	return now_vec3_left.CrossVec(now_vec3_right);
}
LunarEngine::LunarVector3 LunarEngine::LunarVector4::NormalizeVec3()
{
	return GetVector3().NormalizeVec();
}
LunarEngine::LunarVector4 LunarEngine::LunarVector4::NormalizeVec4()
{
	LunarEngine::LunarVector4 normalized_vec;
	DirectX::XMVECTOR now_vec_data = DirectX::XMLoadFloat4(&vector_value);
	DirectX::XMVECTOR now_ans_data = DirectX::XMVector4Normalize(now_vec_data);
	DirectX::XMStoreFloat4(&normalized_vec.vector_value, now_ans_data);
	return normalized_vec;
}