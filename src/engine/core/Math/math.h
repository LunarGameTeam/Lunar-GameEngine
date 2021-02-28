#include <Eigen/Dense>

namespace luna
{

using LVector3f = Eigen::Vector3f;
using LVector4f = Eigen::Vector4f;
using LQuaternion = Eigen::Quaternionf;
using LTransform = Eigen::Transform<float, 3, Eigen::Affine, Eigen::RowMajor>;
using LMatrix3f = Eigen::Matrix3f;
using LMatrix4f = Eigen::Matrix4f;

}