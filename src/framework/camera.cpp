#include "camera.h"

#include "main/includes.h"
#include <iostream>

Camera::Camera()
{
	view_matrix.SetIdentity();
	SetOrthographic(-1,1,1,-1,-1,1);
}

Vector3 Camera::GetLocalVector(const Vector3& v)
{
	Matrix44 iV = view_matrix;
	if (iV.Inverse() == false)
		std::cout << "Matrix Inverse error" << std::endl;
	Vector3 result = iV.RotateVector(v);
	return result;
}

Vector3 Camera::ProjectVector(Vector3 pos, bool& negZ)
{
	Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
	Vector4 result = viewprojection_matrix * pos4;
	negZ = result.z < 0;
	if (type == ORTHOGRAPHIC)
		return result.GetVector3();
	else
		return result.GetVector3() / result.w;
}

void Camera::Rotate(float angle, const Vector3& axis)
{
	Matrix44 R;
	R.SetRotation(angle, axis);
	Vector3 new_front = R * (center - eye);
	center = eye + new_front;
	UpdateViewMatrix();
}

void Camera::Move(Vector3 delta)
{
	Vector3 localDelta = GetLocalVector(delta);
	eye = eye - localDelta;
	center = center - localDelta;
	UpdateViewMatrix();
}

void Camera::Zoom(Vector3 delta)
{
	Vector3 localDelta = GetLocalVector(delta);
	eye = eye - localDelta;
	UpdateViewMatrix();
}

void Camera::Orbit(float angle, const Vector3& axis)
{
	Matrix44 R;
	R.SetRotation(angle, axis);
	Vector3 new_pov = R * (eye - center);
	eye = center + new_pov;
	UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
	type = ORTHOGRAPHIC;

	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

Vector3 Camera::OrthogonalVector(Vector3 pos, bool& out)
{
	Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
	Vector4 result = viewprojection_matrix * pos4;
	if (result.x > right || result.x < left || result.y > top || result.y < bottom || result.z > far_plane || result.z < near_plane)
	{
		out = false;
	}
	else
	{
		out = true;
	}
	return result.GetVector3();
}


void Camera::SetPerspective(float fov, float aspect, float near_plane, float far_plane)
{
	type = PERSPECTIVE;

	this->fov = fov;
	this->aspect = aspect;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	this->eye = eye;
	this->center = center;
	this->up = up;

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	// Reset Matrix (Identity)
	view_matrix.SetIdentity();

	// Comment this line to create your own projection matrix!
	//SetExampleViewMatrix();



	Vector3 forward = center - eye;
	forward = forward.Normalize();
	Vector3 side = forward.Cross(up).Normalize();
	Vector3 top = side.Cross(forward).Normalize();

	view_matrix._11 = side.x; view_matrix._12 = top.x; view_matrix._13 = -forward.x; view_matrix._14 = 0;
	view_matrix._21 = side.y; view_matrix._22 = top.y; view_matrix._23= -forward.y;	 view_matrix._24 = 0;
	view_matrix._31 = side.z; view_matrix._32 = top.z; view_matrix._33 = -forward.z; view_matrix._34 = 0;
	view_matrix._41 = 0;	  view_matrix._42 = 0;	   view_matrix._43 = 0;			 view_matrix._44 = 1;

	Matrix44 T;
	T.TranslateLocal(-eye.x, -eye.y, -eye.z);
	view_matrix = T * view_matrix;
	

	UpdateViewProjectionMatrix();
}

// Create a projection matrix
void Camera::UpdateProjectionMatrix()
{
	// Reset Matrix (Identity)
	projection_matrix.SetIdentity();

	// Comment this line to create your own projection matrix!
	//SetExampleProjectionMatrix();

	if (type == PERSPECTIVE) {
		float f = 1 / (tan(fov / 2));
		projection_matrix._11 = f / aspect;
		projection_matrix._22 = f;
		projection_matrix._33 = (far_plane + near_plane) / (near_plane - far_plane);
		projection_matrix._34 = -1;
		projection_matrix._43 = 2 * ((far_plane * near_plane) / (near_plane - far_plane));
		projection_matrix._44 = 0;
	}
	else if (type == ORTHOGRAPHIC) {
		Vector3 left_v = Vector3(2 / (right - left), 0, 0);
		Vector3 center_v = Vector3(0, 2 / (top - bottom), 0);
		Vector3 right_v = Vector3(0, 0, 2 / (far_plane - near_plane));
		Vector3 trans_v = Vector3(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far_plane + near_plane) / (far_plane - near_plane));

		projection_matrix._11 = left_v.x;		projection_matrix._14 = trans_v.x;
		projection_matrix._22 = center_v.y;		projection_matrix._24 = trans_v.y;
		projection_matrix._33 = right_v.z;		projection_matrix._34 = trans_v.z;
	}

	UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
	viewprojection_matrix = view_matrix * projection_matrix;
}

Matrix44 Camera::GetViewProjectionMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	return viewprojection_matrix;
}

// The following methods have been created for testing.
// Do not modify them.

void Camera::SetExampleViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m );
}

void Camera::SetExampleProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type == PERSPECTIVE)
		gluPerspective(fov, aspect, near_plane, far_plane);
	else
		glOrtho(left,right,bottom,top,near_plane,far_plane);

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m );
	glMatrixMode(GL_MODELVIEW);
}

