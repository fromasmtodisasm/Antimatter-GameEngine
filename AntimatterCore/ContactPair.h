#pragma once
#include "Renderer.h"
#include "Mesh.h"
#include "Contact.h"
#include "Manifold.h"

namespace Antimatter {
	namespace Games {
		public ref class ContactPair
		{
		private:
			void Construction();

		public:
			Pair^ pair;
			PhysicsPair^ body;
			int lifetime;
			int index;
			ContactPair();
			ContactPair(Mesh^ a, Mesh^ b);
			String^ GetString();
			Manifold^ manifold;

			bool isDebugging()
			{
				if (body[0]->debug || body[1]->debug)
					return true;
				return false;
			}
		};
	}
}