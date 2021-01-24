#ifndef ME_CAMERA_HPP
  #define ME_CAMERA_HPP

#include "lme/math/matrix/matrix4.hpp"
#include <lme/math/matrix.hpp>
#include <lme/math/transform.hpp>

namespace me {

  enum ProjectionType {
    PROJECTION_PERSPECTIVE_TYPE,
    PROJECTION_ORTHOGRAPHIC_TYPE
  };


  class Camera {

  protected:

    math::mat4f view_matrix;
    math::mat4f proj_matrix;

  public:

    ProjectionType type;
    float fov;
    float aspect;
    float z_near, z_far;
    math::transform3f transform;

    Camera(ProjectionType type, float fov, float aspect, float z_near, float z_far, math::transform3f transform)
    {
      this->type = type;
      this->fov = fov;
      this->aspect = aspect;
      this->z_near = z_near;
      this->z_far = z_far;
      this->transform = transform;
    }

    int refresh_matrices()
    {
      math::perspective_mat4(proj_matrix, fov, aspect, z_near, z_far);
      return 0;
    }

    const math::mat4f& get_view_matrix() const
    {
      return view_matrix;
    }

    const math::mat4f& get_projection_matrix() const
    {
      return proj_matrix;
    }

  };

}

#endif
