#pragma once
#include "GlaxionHeader.h"
#include "Vector.h"
#include "Matrix3.h"
#include "Shape.h"
//#include "Plane.h"

using namespace System::Collections;
using namespace Microsoft::DirectX;

namespace Glaxion
{
	namespace Games
	{
		public ref class Polyhedron
		{
		public:
			
		 
			ref class Vertex
			{
			public:
				ArrayList^ e = gcnew ArrayList();
				ArrayList^ f = gcnew ArrayList();

				Vector^ pos;
				Vector^ normal;

				void Initialize()
				{
					// compute the interpolated normal
					for each(Glaxion::Games::Polyhedron::Face^ g in f)
					{
						for (int i = 0; i < 3; i++)
						{
							if (this == g->v[i])
							{
								// weight the contribution of each face's normal by the angle that face
								// subtends with respect this vertex
								if (g->e[i] != nullptr && g->e[(i + 2) % 3] != nullptr) // HACK to make holey meshes work
								{
									float angle = (float)Math::Acos(Math::Abs(Vector::Dot(g->e[i]->normal, g->e[(i + 2) % 3]->normal)));
									normal += gcnew Vector(g->plane->A, g->plane->B, g->plane->C) * angle;
								}
								break;
							}
						}
					}
				
					normal = normal->Normalize();
				}

				void Transform(Matrix m)
				{
					Vector3^ vp = pos->toVector3();
					vp->TransformCoordinate(m);
					pos = gcnew Vector(vp);
					Vector3^ nv = normal->toVector3();
					nv->TransformNormal(m);
					normal = gcnew Vector(nv);
				}
			};

			ref class Edge
			{
			public:
				cli::array<Vertex^>^ v = gcnew cli::array<Vertex^>(2);
				cli::array<Glaxion::Games::Polyhedron::Face^>^ f = gcnew cli::array<Glaxion::Games::Polyhedron::Face^>(2);

				Vector^ normal;
				Vector^ edge;

				void Initialize()
				{
					normal = (v[0]->pos - v[1]->pos)->Normalize() ;
				}

				//vector3.TransformNormal(Mat4) is regular matrix x vector?
				void Transform(Matrix m)
				{
					Vector3^ nor = normal->toVector3();
					nor->TransformNormal(m);
					normal = gcnew Vector(nor);
				}
			};
			ref class Face
			{
			public:
				cli::array<Vertex^>^ v = gcnew cli::array<Vertex^>(3);
				cli::array<Edge^>^ e = gcnew cli::array<Edge^>(3);

				Plane^ plane;

				// for distance calculations
				Vector^ edge0;
				Vector^ edge1;
				float fA00, fA01, fA11, fDet;

				void Initialize()
				{
					Vector3 v0(v[0]->pos->x, v[0]->pos->y, v[0]->pos->z);
					Vector3 v1(v[1]->pos->x, v[1]->pos->y, v[1]->pos->z);
					Vector3 v2(v[2]->pos->x, v[2]->pos->y, v[2]->pos->z);
					plane = Plane::Normalize(Plane::FromPoints(v0, v1, v2));

					// for distance calculations
					edge0 = v[1]->pos - v[0]->pos;
					edge1 = v[2]->pos - v[0]->pos;
					fA00 = edge0->LengthSquared();  //to sqaure root or not to square root?
					fA01 = Vector::Dot(edge0, edge1);
					fA11 = edge1->LengthSquared();
					fDet = Math::Abs(fA00*fA11 - fA01*fA01);
				}


				Vector^ PointFromST(float s, float t)
				{
					return (1 - (s + t))*v[0]->pos + s*v[1]->pos + t*v[2]->pos;
				}

				Vector^ NormalFromST(float s, float t)
				{
					return (1 - (s + t))*v[0]->normal + s*v[1]->normal + t*v[2]->normal;
				}

				void Transform(Matrix m)
				{
					plane->Transform(m);
					Vector3^ e0 = edge0->toVector3();
					Vector3^ e1 = edge1->toVector3();
					e0->TransformNormal(m);
					edge0 = gcnew Vector(e0);
					edge1 = gcnew Vector(e1);
				}

				// this code was adapted from Magic Software's point-triangle distance
				// code which can be found at:
				// http://www.magic-software.com
				void SqrDistance(Vector^ pt, float% fSqrDist, float% fS, float% fT)
				{
					Vector^ kDiff = v[0]->pos - pt;
					float fB0 = Vector::Dot(kDiff, edge0);
					float fB1 = Vector::Dot(kDiff, edge1);
					float fC = kDiff->LengthSquared();
					fS = fA01*fB1 - fA11*fB0;
					fT = fA01*fB0 - fA00*fB1;
					fSqrDist = (float)0.0;

					if (fS + fT <= fDet)
					{
						if (fS < (float)0.0)
						{
							if (fT < (float)0.0)  // region 4
							{
								if (fB0 < (float)0.0)
								{
									fT = (float)0.0;
									if (-fB0 >= fA00)
									{
										fS = (float)1.0;
										fSqrDist = fA00 + ((float)2.0)*fB0 + fC;
									}
									else
									{
										fS = -fB0 / fA00;
										fSqrDist = fB0*fS + fC;
									}
								}
								else
								{
									fS = (float)0.0;
									if (fB1 >= (float)0.0)
									{
										fT = (float)0.0;
										fSqrDist = fC;
									}
									else if (-fB1 >= fA11)
									{
										fT = (float)1.0;
										fSqrDist = fA11 + ((float)2.0)*fB1 + fC;
									}
									else
									{
										fT = -fB1 / fA11;
										fSqrDist = fB1*fT + fC;
									}
								}
							}
							else  // region 3
							{
								fS = (float)0.0;
								if (fB1 >= (float)0.0)
								{
									fT = (float)0.0;
									fSqrDist = fC;
								}
								else if (-fB1 >= fA11)
								{
									fT = (float)1.0;
									fSqrDist = fA11 + ((float)2.0)*fB1 + fC;
								}
								else
								{
									fT = -fB1 / fA11;
									fSqrDist = fB1*fT + fC;
								}
							}
						}
						else if (fT < (float)0.0)  // region 5
						{
							fT = (float)0.0;
							if (fB0 >= (float)0.0)
							{
								fS = (float)0.0;
								fSqrDist = fC;
							}
							else if (-fB0 >= fA00)
							{
								fS = (float)1.0;
								fSqrDist = fA00 + ((float)2.0)*fB0 + fC;
							}
							else
							{
								fS = -fB0 / fA00;
								fSqrDist = fB0*fS + fC;
							}
						}
						else  // region 0
						{
							// minimum at interior point
							float fInvDet = ((float)1.0) / fDet;
							fS *= fInvDet;
							fT *= fInvDet;
							fSqrDist = fS*(fA00*fS + fA01*fT + ((float)2.0)*fB0) +
								fT*(fA01*fS + fA11*fT + ((float)2.0)*fB1) + fC;
						}
					}
					else
					{
						float fTmp0, fTmp1, fNumer, fDenom;

						if (fS < (float)0.0)  // region 2
						{
							fTmp0 = fA01 + fB0;
							fTmp1 = fA11 + fB1;
							if (fTmp1 > fTmp0)
							{
								fNumer = fTmp1 - fTmp0;
								fDenom = fA00 - 2.0f*fA01 + fA11;
								if (fNumer >= fDenom)
								{
									fS = (float)1.0;
									fT = (float)0.0;
									fSqrDist = fA00 + ((float)2.0)*fB0 + fC;
								}
								else
								{
									fS = fNumer / fDenom;
									fT = (float)1.0 - fS;
									fSqrDist = fS*(fA00*fS + fA01*fT + 2.0f*fB0) +
										fT*(fA01*fS + fA11*fT + ((float)2.0)*fB1) + fC;
								}
							}
							else
							{
								fS = (float)0.0;
								if (fTmp1 <= (float)0.0)
								{
									fT = (float)1.0;
									fSqrDist = fA11 + ((float)2.0)*fB1 + fC;
								}
								else if (fB1 >= (float)0.0)
								{
									fT = (float)0.0;
									fSqrDist = fC;
								}
								else
								{
									fT = -fB1 / fA11;
									fSqrDist = fB1*fT + fC;
								}
							}
						}
						else if (fT < (float)0.0)  // region 6
						{
							fTmp0 = fA01 + fB1;
							fTmp1 = fA00 + fB0;
							if (fTmp1 > fTmp0)
							{
								fNumer = fTmp1 - fTmp0;
								fDenom = fA00 - ((float)2.0)*fA01 + fA11;
								if (fNumer >= fDenom)
								{
									fT = (float)1.0;
									fS = (float)0.0;
									fSqrDist = fA11 + ((float)2.0)*fB1 + fC;
								}
								else
								{
									fT = fNumer / fDenom;
									fS = (float)1.0 - fT;
									fSqrDist = fS*(fA00*fS + fA01*fT + ((float)2.0)*fB0) +
										fT*(fA01*fS + fA11*fT + ((float)2.0)*fB1) + fC;
								}
							}
							else
							{
								fT = (float)0.0;
								if (fTmp1 <= (float)0.0)
								{
									fS = (float)1.0;
									fSqrDist = fA00 + ((float)2.0)*fB0 + fC;
								}
								else if (fB0 >= (float)0.0)
								{
									fS = (float)0.0;
									fSqrDist = fC;
								}
								else
								{
									fS = -fB0 / fA00;
									fSqrDist = fB0*fS + fC;
								}
							}
						}
						else  // region 1
						{
							fNumer = fA11 + fB1 - fA01 - fB0;
							if (fNumer <= (float)0.0)
							{
								fS = (float)0.0;
								fT = (float)1.0;
								fSqrDist = fA11 + ((float)2.0)*fB1 + fC;
							}
							else
							{
								fDenom = fA00 - 2.0f*fA01 + fA11;
								if (fNumer >= fDenom)
								{
									fS = (float)1.0;
									fT = (float)0.0;
									fSqrDist = fA00 + ((float)2.0)*fB0 + fC;
								}
								else
								{
									fS = fNumer / fDenom;
									fT = (float)1.0 - fS;
									fSqrDist = fS*(fA00*fS + fA01*fT + ((float)2.0)*fB0) +
										fT*(fA01*fS + fA11*fT + ((float)2.0)*fB1) + fC;
								}
							}
						}
					}

					fSqrDist = Math::Abs(fSqrDist);
				}
			};
		

			// this code was adapted from Brian Mirtich's polyhedral mass
			// calculation code which can be found at:
			// http://www.acm.org/jgt
			ref class PolyhedralMassProperties
			{
			public:
				const int X = 0;
				const int Y = 1;
				const int Z = 2;

				int A;   /* alpha */
				int B;   /* beta */
				int C;   /* gamma */

						 /* projection integrals */
				float P1, Pa, Pb, Paa, Pab, Pbb, Paaa, Paab, Pabb, Pbbb;

				/* face integrals */
				float Fa, Fb, Fc, Faa, Fbb, Fcc, Faaa, Fbbb, Fccc, Faab, Fbbc, Fcca;

				/* volume integrals */
				 float T0;
				 cli::array<float>^ T1 = gcnew cli::array<float>(3);
				 cli::array<float>^  T2 = gcnew cli::array<float>(3);
				 cli::array<float>^  TP = gcnew cli::array<float>(3);

				 static float SQR(float x) { return x*x; }
				 static float CUBE(float x) { return x*x*x; }

				 /*
				 void Compute(Polyhedron^ p, float density,
					 float% mass, Vector^% r, Matrix3^% J)
				 {
					 ComputeVolumeIntegrals(p);

					 //                Console.WriteLine("T0 = " + T0);
					 //                Console.WriteLine("T1 = ({0},{1},{2})", T1[0], T1[1], T1[2]);
					 //                Console.WriteLine("T2 = ({0},{1},{2})", T2[0], T2[1], T2[2]);
					 //                Console.WriteLine("TP = ({0},{1},{2})", TP[0], TP[1], TP[2]);

					 mass = density * T0;

					 // compute center of mass 
					 r->x = T1[X] / T0;
					 r->y = T1[Y] / T0;
					 r->z = T1[Z] / T0;

					 // compute inertia tensor 
					 J = Matrix3::Zero;
					 J->M11 = density * (T2[Y] + T2[Z]);
					 J->M22 = density * (T2[Z] + T2[X]);
					 J->M33 = density * (T2[X] + T2[Y]);
				 
					 J->M12 = J->M21 = -density * TP[X];
					 J->M23 = J->M32 = -density * TP[Y];
					 J->M31 = J->M13 = -density * TP[Z];

					 // translate inertia tensor to center of mass
					 J.M11 -= mass * (r.Y*r.Y + r.Z*r.Z);
					 J.M22 -= mass * (r.Z*r.Z + r.X*r.X);
					 J.M33 -= mass * (r.X*r.X + r.Y*r.Y);
					 J.M12 = J.M21 += mass * r.X * r.Y;
					 J.M23 = J.M32 += mass * r.Y * r.Z;
					 J.M31 = J.M13 += mass * r.Z * r.X;
				 
				 }
				 */
			 
			 
				 void Compute(Polyhedron^ p, float density,
					float% mass, Vector^% r, glm::mat3& J)
				{
					ComputeVolumeIntegrals(p);

					//                Console.WriteLine("T0 = " + T0);
					//                Console.WriteLine("T1 = ({0},{1},{2})", T1[0], T1[1], T1[2]);
					//                Console.WriteLine("T2 = ({0},{1},{2})", T2[0], T2[1], T2[2]);
					//                Console.WriteLine("TP = ({0},{1},{2})", TP[0], TP[1], TP[2]);

					mass = density * T0;

					// compute center of mass 
					r->x = T1[X] / T0;
					r->y = T1[Y] / T0;
					r->z = T1[Z] / T0;

					// compute inertia tensor 
					J = glm::mat3(1.f);
					J[0][0] = density * (T2[Y] + T2[Z]);
					J[1][1] = density * (T2[Z] + T2[X]);
					J[2][2] = density * (T2[X] + T2[Y]);
					J[0][1] = J[1][0] = -density * TP[X];
					J[1][2] = J[2][1] = -density * TP[Y];
					J[2][0] = J[0][2] = -density * TP[Z];

					// translate inertia tensor to center of mass 
					J[0][0] -= mass * (r->y*r->y + r->z*r->z);
					J[1][1] -= mass * (r->z*r->z + r->x*r->x);
					J[2][2] -= mass * (r->x*r->x + r->y*r->y);
					J[0][1] = J[1][0] += mass * r->x * r->y;
					J[1][2] = J[2][1] += mass * r->y * r->z;
					J[2][0] = J[0][2] += mass * r->z * r->x;
				}
			
				void ComputeVolumeIntegrals(Polyhedron^ p)
				{
					T0 = T1[X] = T1[Y] = T1[Z]
						= T2[X] = T2[Y] = T2[Z]
						= TP[X] = TP[Y] = TP[Z] = 0;

					//cli::array<float>^ norm = gcnew cli::array<float>(3);
					cli::array<float>^ norm = gcnew cli::array<float>(3);
					for each(Face^ f in p->faces)
					{
						
						norm[X] = f->plane->A;
						norm[Y] = f->plane->B;
						norm[Z] = f->plane->C;
					
						float nx = Math::Abs(norm[X]);
						float ny = Math::Abs(norm[Y]);
						float nz = Math::Abs(norm[Z]);
						if (nx > ny && nx > nz) C = X;
						else C = (ny > nz) ? Y : Z;
						A = (C + 1) % 3;
						B = (A + 1) % 3;

						ComputeFaceIntegrals(p, f, norm);

						T0 += norm[X] * ((A == X) ? Fa : ((B == X) ? Fb : Fc));

						T1[A] += norm[A] * Faa;
						T1[B] += norm[B] * Fbb;
						T1[C] += norm[C] * Fcc;
						T2[A] += norm[A] * Faaa;
						T2[B] += norm[B] * Fbbb;
						T2[C] += norm[C] * Fccc;
						TP[A] += norm[A] * Faab;
						TP[B] += norm[B] * Fbbc;
						TP[C] += norm[C] * Fcca;
					}

					T1[X] /= 2; T1[Y] /= 2; T1[Z] /= 2;
					T2[X] /= 3; T2[Y] /= 3; T2[Z] /= 3;
					TP[X] /= 2; TP[Y] /= 2; TP[Z] /= 2;
				}

				void ComputeFaceIntegrals(Polyhedron^ p, Glaxion::Games::Polyhedron::Face^ f, cli::array<float>^ n)
				{
					float w = f->plane->D;
					float k1, k2, k3, k4;

					ComputeProjectionIntegrals(p, f);

					k1 = 1 / n[C]; k2 = k1 * k1; k3 = k2 * k1; k4 = k3 * k1;

					Fa = k1 * Pa;
					Fb = k1 * Pb;
					Fc = -k2 * (n[A] * Pa + n[B] * Pb + w*P1);

					Faa = k1 * Paa;
					Fbb = k1 * Pbb;
					Fcc = k3 * (SQR(n[A])*Paa + 2 * n[A] * n[B] * Pab + SQR(n[B])*Pbb
						+ w*(2 * (n[A] * Pa + n[B] * Pb) + w*P1));

					Faaa = k1 * Paaa;
					Fbbb = k1 * Pbbb;
					Fccc = -k4 * (CUBE(n[A])*Paaa + 3 * SQR(n[A])*n[B] * Paab
						+ 3 * n[A] * SQR(n[B])*Pabb + CUBE(n[B])*Pbbb
						+ 3 * w*(SQR(n[A])*Paa + 2 * n[A] * n[B] * Pab + SQR(n[B])*Pbb)
						+ w*w*(3 * (n[A] * Pa + n[B] * Pb) + w*P1));

					Faab = k1 * Paab;
					Fbbc = -k2 * (n[A] * Pabb + n[B] * Pbbb + w*Pbb);
					Fcca = k3 * (SQR(n[A])*Paaa + 2 * n[A] * n[B] * Paab + SQR(n[B])*Pabb
						+ w*(2 * (n[A] * Paa + n[B] * Pab) + w*Pa));
				}

				void ComputeProjectionIntegrals(Polyhedron^ p, Glaxion::Games::Polyhedron::Face^ f)
				{
					float a0, a1, da;
					float b0, b1, db;
					float a0_2, a0_3, a0_4, b0_2, b0_3, b0_4;
					float a1_2, a1_3, b1_2, b1_3;
					float C1, Ca, Caa, Caaa, Cb, Cbb, Cbbb;
					float Cab, Kab, Caab, Kaab, Cabb, Kabb;

					P1 = Pa = Pb = Paa = Pab = Pbb = Paaa = Paab = Pabb = Pbbb = 0.0f;

					cli::array<float,2>^ v = gcnew cli::array<float,2>(3, 3);
					for (int i = 0; i < 3; i++)
					{
						v[i, 0] = f->v[i]->pos->x;
						v[i, 1] = f->v[i]->pos->y;
						v[i, 2] = f->v[i]->pos->z;
					}
					for (int i = 0; i < 3; i++)
					{
						a0 = v[i, A];
						b0 = v[i, B];
						a1 = v[(i + 1) % 3, A];
						b1 = v[(i + 1) % 3, B];
						da = a1 - a0;
						db = b1 - b0;
						a0_2 = a0 * a0; a0_3 = a0_2 * a0; a0_4 = a0_3 * a0;
						b0_2 = b0 * b0; b0_3 = b0_2 * b0; b0_4 = b0_3 * b0;
						a1_2 = a1 * a1; a1_3 = a1_2 * a1;
						b1_2 = b1 * b1; b1_3 = b1_2 * b1;

						C1 = a1 + a0;
						Ca = a1*C1 + a0_2; Caa = a1*Ca + a0_3; Caaa = a1*Caa + a0_4;
						Cb = b1*(b1 + b0) + b0_2; Cbb = b1*Cb + b0_3; Cbbb = b1*Cbb + b0_4;
						Cab = 3 * a1_2 + 2 * a1*a0 + a0_2; Kab = a1_2 + 2 * a1*a0 + 3 * a0_2;
						Caab = a0*Cab + 4 * a1_3; Kaab = a1*Kab + 4 * a0_3;
						Cabb = 4 * b1_3 + 3 * b1_2*b0 + 2 * b1*b0_2 + b0_3;
						Kabb = b1_3 + 2 * b1_2*b0 + 3 * b1*b0_2 + 4 * b0_3;

						P1 += db*C1;
						Pa += db*Ca;
						Paa += db*Caa;
						Paaa += db*Caaa;
						Pb += da*Cb;
						Pbb += da*Cbb;
						Pbbb += da*Cbbb;
						Pab += db*(b1*Cab + b0*Kab);
						Paab += db*(b1*Caab + b0*Kaab);
						Pabb += da*(a1*Cabb + a0*Kabb);
					}

					P1 /= 2.0f;
					Pa /= 6.0f;
					Paa /= 12.0f;
					Paaa /= 20.0f;
					Pb /= -6.0f;
					Pbb /= -12.0f;
					Pbbb /= -20.0f;
					Pab /= 24.0f;
					Paab /= 60.0f;
					Pabb /= -60.0f;
				}

			};

			ArrayList^ verts;
			ArrayList^ edges;
			ArrayList^ faces;
			Polyhedron()
			{
				/*
				// clone the mesh so as to get at it's faces and vertices
				using (Mesh tempMesh = mesh.Clone(MeshFlags.Managed, VertexFormats.Position, mesh.Device))
				{
				CustomVertex.PositionOnly[] vertList = (CustomVertex.PositionOnly[])tempMesh.LockVertexBuffer(typeof(CustomVertex.PositionOnly), LockFlags.ReadOnly, tempMesh.NumberVertices);
				short[] faceList = (short[])tempMesh.LockIndexBuffer(typeof(short), LockFlags.ReadOnly, tempMesh.NumberFaces * 3);

				System.Diagnostics.Debug.Assert(vertList.Length == tempMesh.NumberVertices);
				System.Diagnostics.Debug.Assert(faceList.Length == tempMesh.NumberFaces * 3);

				verts = new ArrayList();
				for (int i = 0; i < tempMesh.NumberVertices; i++)
				{
				Vertex v = new Vertex();
				v.pos = vertList[i].GetPosition();
				verts.Add(v);
				}

				faces = new ArrayList();
				for (int i = 0; i < tempMesh.NumberFaces; i++)
				{
				Face f = new Face();
				for (int j = 0; j < 3; j++)
				f.v[j] = (Vertex)verts[faceList[i * 3 + j]];
				faces.Add(f);
				}

				tempMesh.UnlockVertexBuffer();
				tempMesh.UnlockIndexBuffer();
				}
				*/
				Initialize();
			}

			void Initialize()
			{
				// backlink each vertex to all the faces it is in
				for each(Face^ f in faces)
					for each(Vertex^ v in f->v)
						v->f->Add(f);

				// clean the mesh of duplicate vertices
				// for speed would want to sort the vertices in some way
				for (int i = 0; i < verts->Count; i++)
				{
					for (int j = i + 1; j < verts->Count; )
					{
						Vertex^ v1 = (Vertex^)verts[i];
						Vertex^ v2 = (Vertex^)verts[j];
						if (v1->pos->x == v2->pos->x && v1->pos->y == v2->pos->y && v1->pos->z == v2->pos->z)
						{
							for each(Face^ f in v2->f)
							{
								// replace v2 in face f with v1
								for (int k = 0; k < 3; k++)
									if (f->v[k] == v2)
										f->v[k] = v1;

								// add f to v1's list of faces
								v1->f->Add(f);
							}

							// eliminate v2 from the list of vertices
							verts->RemoveAt(j);
						}
						else
						{
							// only move on when didn't delete something
							j++;
						}
					}
				}

				// identify all edges
				edges = gcnew ArrayList();
				int numMissingEdges = 0;
				for each(Face^ f1 in faces)
				{
					for (int i = 0; i < 3; i++)
					{
						// check if edge already created
						if (f1->e[i] != nullptr) continue;

						for each(Face^ f2 in f1->v[i]->f)
						{
							if (f1 == f2) continue;

							for (int j = 0; j < 3; j++)
							{
								if (f1->v[i] == f2->v[(j + 1) % 3] && f1->v[(i + 1) % 3] == f2->v[j])
								{
									Edge^ e = gcnew Edge();
									e->v[0] = f1->v[i];
									e->v[1] = f1->v[(i + 1) % 3];
									e->f[0] = (Glaxion::Games::Polyhedron::Face^)f1;
									e->f[1] = (Glaxion::Games::Polyhedron::Face^)f2;
									f1->e[i] = e;
									f2->e[j] = e;
									f1->v[i]->e->Add(e);
									f2->v[j]->e->Add(e);
									edges->Add(e);
								}
							}
						}

						// make sure edge was found
						if (f1->e[i] == nullptr)
						{
							numMissingEdges++;
						}
					}
				}

				//            if (numMissingEdges > 0)
				//                throw new Exception("Unable to construct polyhedron b/c of hole in mesh: " + numMissingEdges + " missing edges\n");
				//
				for each(Face^ f in faces)
				{
					f->Initialize();
				}

				for each(Edge^ e in edges)
				{
					e->Initialize();
				}

				for each(Vertex^ v in verts)
				{
					v->Initialize();
				}
			}

			void ComputeMassProperties(float density, float% mass, Vector^% centerOfMass, glm::mat3& inertiaTensor)
			{
				PolyhedralMassProperties^ massProps = gcnew PolyhedralMassProperties();
				massProps->Compute(this, density, mass, centerOfMass, inertiaTensor);
			}

			void Transform(Matrix m)
			{
				for each(Vertex^ v in verts)
				{
					v->Transform(m);
				}

				for each(Face^ f in faces)
				{
					f->Transform(m);
				}

				for each(Edge^ e in edges)
				{
					e->Transform(m);
				}
			}

			// determines the squared distance from the given point to this polyhedron
			// also gives the closest point on the polyhedron and the interpolated normal at the point
			void SqrDistance(Vector^ pt, float% sqrDist, Vector^% closestPt, Vector^% normal)
			{
				float bestDist = float::PositiveInfinity;
				float bestS = 0, bestT = 0;
				Face^ bestFace = nullptr;

				for each(Face^ curFace in faces)
				{
					float curDist;
					float curS, curT;

					curFace->SqrDistance(pt, curDist, curS, curT);
					if (curDist < bestDist)
					{
						bestDist = curDist;
						bestS = curS;
						bestT = curT;
						bestFace = curFace;
					}
				}

				sqrDist = bestDist;
				closestPt = bestFace->PointFromST(bestS, bestT);
				normal = bestFace->NormalFromST(bestS, bestT);
			}
		};
	}
}