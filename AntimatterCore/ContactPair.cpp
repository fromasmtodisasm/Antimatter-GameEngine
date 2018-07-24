#include "ContactPair.h"

//iterative solver
//http://myselph.de/gamePhysics/equalityConstraints.html

//physics engine code and explanation
//https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physics7solvers/2016%20Tutorial%207%20-%20Solvers.pdf
//oneshot
//http://twvideo01.ubm-us.net/o1/vault/gdc2012/slides/Programming%20Track/Vincent_ROBERT_Track_ADifferentApproach.pdf

//http://gamedevs.org/uploads/continuous-collision-detection-and-physics.pdf

//clipping
//https://gamedevelopment.tutsplus.com/tutorials/understanding-sutherland-hodgman-clipping-for-physics-engines--gamedev-11917
//c#
//https://rosettacode.org/wiki/Sutherland-Hodgman_polygon_clipping#C.23

namespace Antimatter {
	namespace Games {
		void ContactPair::Construction()
		{
			lifetime = 0;
			pair = gcnew Pair();
		}

		ContactPair::ContactPair()
		{
			Construction();
		}

		String^ ContactPair::GetString()
		{
			return pair->GetString();
		}

		ContactPair::ContactPair(Mesh^A, Mesh^ B)
		{
			Construction();
		}
	}
}