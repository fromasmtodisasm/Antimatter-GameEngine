#include "glaxTransform.h"

using namespace Antimatter::glx;

void Transform::SetPosition(glm::vec3 value)
{
	WorldPosition = value;

	if (Parent)
	{
		//if parented, set the local position
		//relative to the parent
		glm::quat parentSpaceConjugate = glm::conjugate(Parent->WorldRotation);

		LocalPosition = (parentSpaceConjugate *
			(value - Parent->WorldPosition)) /
			Parent->WorldScale;
	}
}

//world position
glm::vec3 Transform::GetPosition()
{

	if (Parent)
	{
		//The world position is calculated relative
		//to the parent
		//Parent->GetWorldMatrix();
		WorldPosition = Parent->WorldPosition +
			Parent->WorldRotation * (LocalPosition * Parent->WorldScale);
		return WorldPosition;

	}
	else {
		return WorldPosition;
	}
}

//haven't tested this yet
glm::quat Transform::SetDirection(glm::vec3 axis, glm::vec3 direction)
{
	glm::quat q;
	glm::vec3 a = cross(axis, direction);
	q.x = a.x;
	q.y = a.y;
	q.z = a.z;
	q.w = sqrt((pow(glm::length(axis), 2)) * (pow(glm::length(direction), 2))) + dot(axis, direction);
	return glm::normalize(q);
}

void Transform::SetRotation(glm::quat value)
{
	WorldRotation = value;
}

glm::quat Transform::GetRotation()
{
	if (Parent)
	{
		WorldRotation = Parent->GetRotation()*LocalRotation;
		return WorldRotation;
	}
	else
		return WorldRotation;
}

void Transform::SetScale(glm::vec3 value)
{
	if (Parent)
	{
		glm::quat parentSpaceConjugate = glm::conjugate(Parent->WorldRotation);
		LocalScale = parentSpaceConjugate*(value / Parent->WorldScale);
	}
	else {
		WorldScale = value;
	}
}

glm::vec3 Transform::GetScale()
{
	if (Parent)
	{
		WorldScale = Parent->WorldScale*LocalScale;
		return WorldScale;
	}
	else {
		return WorldScale;
	}
}

void Transform::SetLocalPosition(glm::vec3 value)
{
	LocalPosition = value;
}

glm::vec3 Transform::GetLocalPosition()
{
	return LocalPosition;
}

void Transform::SetLocalRotation(glm::quat value)
{
	LocalRotation = value;
}

glm::quat Transform::GetLocalRotation()
{
	return LocalRotation;
}

void Transform::SetLocalScale(glm::vec3 value)
{
	LocalScale = value;
}

glm::vec3 Transform::GetLocalScale()
{
	if (Parent)
		return LocalScale;
	else
		return WorldScale;
}

glm::mat4 Transform::GetWorldMatrix()
{
	if (!isStatic)
	{
		glm::mat4 mat(1.f);
		mat = glm::translate(mat, GetPosition());
		rotationMatrix = glm::mat4(GetRotation());
		mat = mat*rotationMatrix;
		pointTransformMatrix = mat;
		staticMatrix = glm::scale(mat, GetScale());
		return staticMatrix;
	}
	else {
		return staticMatrix;
	}
}

glm::mat4 Transform::GetTranslationMatrix()
{
	return glm::translate(glm::mat4(1.f), GetPosition());
}