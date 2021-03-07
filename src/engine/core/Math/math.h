#include <Eigen/Dense>

namespace luna
{

using LVector2f = Eigen::Vector2f;
using LVector3f = Eigen::Vector3f;
using LVector4f = Eigen::Vector4f;
using LQuaternion = Eigen::Quaternionf;
using LTransform = Eigen::Transform<float, 3, Eigen::Affine, Eigen::RowMajor>;
using LMatrix3f = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
using LMatrix4f = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;

}