#include "GameObject.h"

using namespace Glaxion::Core;
/*
GameObject::GameObject()
{
	Construction();
}

void GameObject::Construction()
{
	meshes = gcnew List<Mesh^>();
	mesh = nullptr;
	transform = gcnew Transform();
	colour = gcnew Colour();
	name = "Game Object";
	visible = true;
}

GameObject::GameObject(Primitive^ prim)
{

	Construction();
	name = prim->Name;
	GetPrimitive(prim);
}

GameObject::GameObject(Primitive^ prim,String^ Name)
{
	Construction();
	name = Name;
	GetPrimitive(prim);
}

void GameObject::AddMesh(Mesh^ mesh)
{
	mesh->transform->SetParent(transform);
	meshes->Add(mesh);
	this->mesh = mesh;
}

void GameObject::Draw(Renderer^ renderer)
{	
	for each(Mesh^ m in meshes)
	{

		glm::mat4 mat = transform->gxtransform->GetWorldMatrix();
		if (m->visible == true)
		{

			renderer->NativeRenderer->DrawMesh(*m->vbo, mat,&(colour)->ToGLX(), (int)m->mode);
		}
				
	}
}

void GameObject::Debug()
{

	Console::WriteLine("\nDebugging Game Object : " + name);
	int i = 1;
	for each(Mesh^ m in meshes)
	{

		Console::WriteLine("Mesh " + i);
		if(m->transform->Parent != nullptr)
		Console::WriteLine("Parented : true");
		else
			Console::WriteLine("Parented : false");
			i++;
	}
}
 
void GameObject::GetPrimitive(Primitive^ primitive)
{
	Mesh^ mesh = gcnew Mesh(primitive);
	name = primitive->Name;
	AddMesh(mesh);

}
*/
