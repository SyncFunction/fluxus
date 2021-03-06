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

#include <assert.h>
#include "ImmediateMode.h"

using namespace Fluxus;

ImmediateMode::ImmediateMode()
{
}

ImmediateMode::~ImmediateMode()
{
}

void ImmediateMode::Add(Primitive *p, State *s, bool del /* = false */)
{
	assert(p!=NULL);
	assert(s!=NULL);
	IMItem *newitem = new IMItem;
	newitem->m_State = *s;
	newitem->m_Primitive = p;
	newitem->m_DelPrim = del;
	m_IMRecord.push_back(newitem);
}

void ImmediateMode::Render(unsigned int CamIndex, ShadowVolumeGen *shadowgen)
{
	///\todo: not using camera visibility in immediate mode...
	for(vector<IMItem*>::iterator i=m_IMRecord.begin(); i!=m_IMRecord.end(); ++i)
	{
		glPushMatrix();
		(*i)->m_State.Apply();
		// need to set the state to the primitive to update the parts of the state the
		// render call acts on. need to look at this.
		assert((*i)->m_Primitive!=NULL);
	    (*i)->m_Primitive->SetState(&(*i)->m_State);
		(*i)->m_Primitive->Prerender();
		(*i)->m_Primitive->Render();

		if (shadowgen && (*i)->m_Primitive->GetState()->Hints & HINT_CAST_SHADOW)
		{
			shadowgen->Generate((*i)->m_Primitive);
		}
		(*i)->m_State.Unapply();
		glPopMatrix();
	}
}

void ImmediateMode::Clear()
{
	for(vector<IMItem*>::iterator i=m_IMRecord.begin(); i!=m_IMRecord.end(); ++i)
	{
		if ((*i)->m_DelPrim)
		{
			delete (*i)->m_Primitive;
		}
		delete *i;
	}

	m_IMRecord.clear();
}

