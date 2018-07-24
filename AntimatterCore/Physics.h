#pragma once
#include "Shape.h"
#include "Draw.h"
#include "Velocity.h"

#define MAX_ANGVEL (math_pi * 2.5f)
//supposedly correct forward definition but causes lots of errors
//namespace Antimatter::Games { ref class ContactPair; }

//clamping impulses
//http://allenchou.net/2013/12/game-physics-resolution-contact-constraints/

//some physics engine
//http://www.sciencedirect.com/science/article/pii/S0010448514000311


//special spring
//https://www.reddit.com/r/gamedev/comments/5e3ine/game_physics_article_on_how_to_implement_a_stable/

//least gaus method
//http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.3.9171&rep=rep1&type=pdf

namespace Antimatter {
	namespace Games {

		public ref class Physics
		{
		protected:
			!Physics()
			{
				delete localInertiaMatrix;
				delete inverseLocalInertiaMatrix;
				localInertiaMatrix = NULL;
				inverseLocalInertiaMatrix = NULL;
			}

		public:

			Physics(Shape^ shape, Transform^ t);
			
			~Physics()
			{
				this->!Physics();
			}

			Shape^ shape;
			AABB^ aabb;
			Transform^ transform;
			static Vector^ Gravity = gcnew Vector(0.f,-9.0f,0.f);
			static bool Break = false;
			static bool Check = false;
			Vector^ gravity;
			Vector^ com;
			Vector^ totalForce;
			Vector^ totalTorque;
			Vector^ avgAcceleration;
			Vector^ accleration;
			Vector^ minBounds;
			Vector^ maxBounds;
			Vector^ worldPosition;
			Quaternion^ worldRotation;
			Velocity^ velocity;
			float maxAngVel = MAX_ANGVEL;
			float mass;
			float inverseMass;
			float density;
			float resititution;
			float staticFriction;
			float dynamicFriction;
			float friction;
			float timeStep = time_step;
			bool isStatic;
			bool isKinematic;
			bool debug;
			bool collidable;
			bool enabled;
			bool frozen;
			bool cleanMomentmatrix;
			bool usedampening;
			int integrationMode;
			glm::mat3* localInertiaMatrix;  //can probably replace these with vectors from the matrix's diag
			glm::mat3* inverseLocalInertiaMatrix;

			void SetMassProperties(float value);
			void DrawBounds(Colour^ col,bool updateBounds);
			void ApplyImpulse(Vector^ point, Vector^ impulse);
			void ApplyMomentum(Vector ^ point, Vector ^ impulse);
			void UpdateVelocities();
			void UpdateTransform();

			//taken from Xenocollide
			void ComputeMassProperties(float density)
			{
				Vector^ diag = gcnew Vector(0.f, 0.f, 0.f);
				Vector^ offDiag = gcnew Vector(0.f, 0.f, 0.f);
				Vector^ weightedCenterOfMass = gcnew Vector(0.f, 0.f, 0.f);
				float volume = 0.f;
				mass = 0.f;

				// Iterate through the faces
				for (int faceIndex = 0; faceIndex < shape->Faces->Count; faceIndex++)
				{
					Face^ face = shape->Faces[faceIndex];
					// Iterate through the tris in the face
					Vector^ v0 = shape->points[face->A];
					Vector^ v1 = shape->points[face->B];
					Vector^ v2 = shape->points[face->C];
					float det = Vector::Det(v0, v1, v2);

					// Volume
					float tetVolume = det / 6.0f;
					volume += tetVolume;

					// Mass
					float tetMass = tetVolume * density;
					mass += tetMass;

					// Center of Mass
					Vector^ tetCenterOfMass = (v0 + v1 + v2) / 4.0f; // Note: includes origin (0, 0, 0) as fourth vertex
					weightedCenterOfMass += tetMass * tetCenterOfMass;

					// Inertia Tensor
					diag += det * (v0*v1 + v1*v2 + v2*v0 + v0*v0 + v1*v1 + v2*v2) / 60.0f;

					//probably wrong
					float of = det * (
						v0->y*v1->z + v1->y*v2->z + v2->y*v0->z +
						v0->y*v2->z + v1->y*v0->z + v2->y*v1->z +
						2.f * v0->y*v0->z + 2.f * v1->y*v1->z + 2.f * v2->y*v2->z) / 120.0f;

					offDiag += gcnew Vector(of, of, of);
				}

				Vector^ centerOfMass = weightedCenterOfMass / mass;

				diag *= density;
				offDiag *= density;

				glm::mat3 I = glm::mat3(1.f);
				I[0][0] = diag->y + diag->z;
				I[1][1] = diag->z + diag->x;
				I[2][2] = diag->x + diag->y;
				I[1][2] = I[2][1] = -offDiag->x;
				I[0][2] = I[2][0] = -offDiag->y;
				I[0][1] = I[1][0] = -offDiag->z;

				// Move inertia tensor to be relative to center of mass (rather than origin)

				// Translate intertia to center of mass
				float x = centerOfMass->x;
				float y = centerOfMass->y;
				float z = centerOfMass->z;

				I[0][0] -= mass*(y*y + z*z);
				I[0][1] -= mass*(-x*y);
				I[0][2] -= mass*(-x*z);
				I[1][1] -= mass*(x*x + z*z);
				I[1][2] -= mass*(-y*z);
				I[2][2] -= mass*(x*x + y*y);

				// Symmetry
				I[1][0] = I[0][1];
				I[2][0] = I[0][2];
				I[2][1] = I[1][2];

				com = centerOfMass;
				inverseMass = 1.0f / mass;
				localInertia = &I;
			}

			Physics(const Velocity^% v)
			{
				this->velocity->linear = v->linear;
				this->velocity->angular = v->angular;
			}
		
			void CancelLinearVelocity()
			{
				velocity->linear -= velocity->linear;
			}

			glm::mat3 ComputeInertiaTensor(float density);
			void GetBounds();
			virtual void ApplyForces();

			bool IsMovable()
			{
				if (!frozen && !isStatic)
					return true;
				return false;
			}
		
			bool IsCollidable()
			{
				if (this == nullptr || !collidable || !enabled)
					return false;
				return true;
			}

			void ReverseVelocity()
			{
				velocity->linear = -velocity->linear;
				velocity->angular = -velocity->angular;
			}

			Vector^ PointWorldToBody(Vector^ p)
			{
				return gcnew Vector(worldOrientation->inverse() * (p - worldPosition));
			}

			Vector^ PointBodyToWorld(Vector^ p)
			{
				return gcnew Vector(worldPosition + (worldOrientation*p));
			}

			Vector^ VectorWorldToBody(Vector^ v)
			{
				return gcnew Vector(worldOrientation * v);
			}

			Vector^ VectorBodyToWorld(Vector^ v)
			{
				return  gcnew Vector(worldOrientation* v);
			}

			//damping, clamped from 0 to 1
			float _linearDamping;
			float _angularDamping;
			void SetDamping(float factor)
			{
				SetLinearDampening(factor);
				SetAngularDampening(factor);
			}

			void SetLinearDampening(float factor)
			{
				_linearDamping = tool::clamp(factor, 0.f, 1.f);
			}

			void SetAngularDampening(float factor)
			{
				_angularDamping = tool::clamp(factor, 0.f, 1.f);
			}

			void ApplyLinearDamping()
			{
				velocity->linear = ApplyDamping(velocity->linear, _linearDamping);
			}

			void ApplyAngularDamping()
			{
				velocity->angular = ApplyDamping(velocity->angular, _angularDamping);
			}

			static Vector^ ApplyDamping(Vector^ velocity, float damping)
			{
				Vector^ v = velocity * pow(1 - damping, time_step);
				return v;
			}

			property glm::mat3* localInertia
			{
				glm::mat3* get()
				{
					return localInertiaMatrix;
				};

				void set(glm::mat3* mat)
				{
					delete localInertiaMatrix;
					localInertiaMatrix = new glm::mat3(*mat);
				};
			}

			//??  do we need this?
			property glm::mat3 momentInertia
			{
				glm::mat3 get()
				{
						// TODO : DO NOT RECOMPUTE THE MATRIX MULTIPLICATION EVERY TIME. WE NEED TO STORE THE
						//        INVERSE WORLD TENSOR IN THE CLASS AND UPDATE IT WHEN THE ORIENTATION OF THE BODY CHANGES
					glm::mat3 rot = glm::mat3(worldOrientation->ToMatrix());
					glm::mat3 wi = rot*(*localInertia)*glm::transpose(rot);
						// Compute and return the inertia tensor in world coordinates
					if (cleanMomentmatrix)
					{
						wi[0][1] = 0.f;
						wi[0][2] = 0.f;
						wi[1][0] = 0.f;
						wi[1][2] = 0.f;
						wi[2][0] = 0.f;
						wi[2][1] = 0.f;
					}
					return wi;
					
				};
			}

			property glm::mat3 momentInertiaInverse
			{
				glm::mat3 get()
				{
					// TODO : DO NOT RECOMPUTE THE MATRIX MULTIPLICATION EVERY TIME. WE NEED TO STORE THE
					//        INVERSE WORLD TENSOR IN THE CLASS AND UPLDATE IT WHEN THE ORIENTATION OF THE BODY CHANGES
					glm::mat3 rot = glm::mat3(worldOrientation->glm());
					glm::mat3 wi = rot*(*localInertiaInverse)*glm::transpose(rot);
	
					if (cleanMomentmatrix)
					{
						wi[0][1] = 0.f;
						wi[0][2] = 0.f;
						wi[1][0] = 0.f;
						wi[1][2] = 0.f;
						wi[2][0] = 0.f;
						wi[2][1] = 0.f;
					}

					return wi;
				};
			}
			
			property glm::mat3* localInertiaInverse
			{
				glm::mat3* get()
				{
					return inverseLocalInertiaMatrix;
				};
				
				void set(glm::mat3* mat)
				{
					delete inverseLocalInertiaMatrix;
					inverseLocalInertiaMatrix = new glm::mat3(*mat);
				};
			}

			property Quaternion^ worldOrientation
			{
				Quaternion^ get()
				{
					return transform->Rotation;
				};
			}

			bool Colliding(Physics^ p)
			{
				p->GetBounds();
				Vector^ pmin = p->minBounds;
				Vector^ pmax = p->maxBounds;
				GetBounds();
				Vector^ mmin = minBounds+pmin;
				Vector^ mmax = maxBounds+pmax;
				if (mmin->x <= 0 && mmax->x >= 0 && mmin->y <= 0 && mmax->y >= 0 && mmin->z <= 0 && mmax->z >= 0)
				{
					return true;
				}
				return false;
			}
		};
	}
}
