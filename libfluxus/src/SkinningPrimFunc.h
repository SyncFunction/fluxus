// Copyright (C) 2005 Dave Griffiths
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef N_SKINNING_PRIMITIVE_FUNCTION
#define N_SKINNING_PRIMITIVE_FUNCTION

#include "PrimitiveFunction.h"
#include "Primitive.h"
#include "SceneGraph.h"

using namespace std;

namespace Fluxus
{

//////////////////////////////////////////////////
/// A primitive function for debugging skin weights
class SkinningPrimFunc : public PrimitiveFunction
{
public:
	SkinningPrimFunc();
	~SkinningPrimFunc();

	virtual void Run(Primitive &prim, const SceneGraph &world);

private:
	
};


}

#endif
