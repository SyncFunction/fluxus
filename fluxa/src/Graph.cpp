// Copyright (C) 2006 David Griffiths <dave@pawfal.org>
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

#include <vector>
#include <math.h>
#include "Graph.h"
#include "ModuleNodes.h"
#include "Modules.h"

Graph::Graph(unsigned int NumNodes, unsigned int SampleRate) :
m_MaxPlaying(10),
m_NumNodes(NumNodes),
m_SampleRate(SampleRate)
{
	Init();
}

Graph::~Graph()
{
	Clear();
}

void Graph::Init()
{
	for (unsigned int type=0; type<NUMTYPES; type++)
	{
		NodeDescVec *descvec = new NodeDescVec;
		
		unsigned int count=m_NumNodes;
		
		if (type==TERMINAL) count=2000;
		
		for(unsigned int n=0; n<count; n++)
		{
			NodeDesc *nodedesc = new NodeDesc;
			
			switch(type)
			{
				case TERMINAL : nodedesc->m_Node = new TerminalNode(0); break;
				case SINOSC : nodedesc->m_Node = new OscNode((int)WaveTable::SINE,m_SampleRate); break;
				case SAWOSC : nodedesc->m_Node = new OscNode((int)WaveTable::SAW,m_SampleRate); break;
				case TRIOSC : nodedesc->m_Node = new OscNode((int)WaveTable::TRIANGLE,m_SampleRate); break;
				case SQUOSC : nodedesc->m_Node = new OscNode((int)WaveTable::SQUARE,m_SampleRate); break;
				case WHITEOSC : nodedesc->m_Node = new OscNode((int)WaveTable::NOISE,m_SampleRate); break;
				case PINKOSC : nodedesc->m_Node = new OscNode((int)WaveTable::PINKNOISE,m_SampleRate); break;
				case ADSR : nodedesc->m_Node = new ADSRNode(m_SampleRate); break;
				case ADD : nodedesc->m_Node = new MathNode(MathNode::ADD); break;
				case SUB : nodedesc->m_Node = new MathNode(MathNode::SUB); break;
				case MUL : nodedesc->m_Node = new MathNode(MathNode::MUL); break;
				case DIV : nodedesc->m_Node = new MathNode(MathNode::DIV); break;
				case POW : nodedesc->m_Node = new MathNode(MathNode::POW); break;
				case MOOGLP : nodedesc->m_Node = new FilterNode(FilterNode::MOOGLP,m_SampleRate); break;
				case MOOGBP : nodedesc->m_Node = new FilterNode(FilterNode::MOOGBP,m_SampleRate); break;
				case MOOGHP : nodedesc->m_Node = new FilterNode(FilterNode::MOOGHP,m_SampleRate); break;
				case FORMANT : nodedesc->m_Node = new FilterNode(FilterNode::FORMANT,m_SampleRate); break;
				case SAMPLER : nodedesc->m_Node = new SampleNode(m_SampleRate); break;
				case CRUSH : nodedesc->m_Node = new EffectNode(EffectNode::CRUSH,m_SampleRate); break;
				case DISTORT : nodedesc->m_Node = new EffectNode(EffectNode::DISTORT,m_SampleRate); break;
				case CLIP : nodedesc->m_Node = new EffectNode(EffectNode::CLIP,m_SampleRate); break;
				case DELAY : nodedesc->m_Node = new EffectNode(EffectNode::DELAY,m_SampleRate); break;				
				default: assert(0); break;
			}
			
			descvec->m_Vec.push_back(nodedesc);
		}
		
		m_NodeDescMap[(Type)type] = descvec;
	}
}

void Graph::Clear()
{
	m_RootNodes.clear();
	m_NodeMap.clear();
	
	for (map<Type,NodeDescVec*>::iterator i=m_NodeDescMap.begin(); 
		i!=m_NodeDescMap.end(); ++i)
	{
		for (vector<NodeDesc*>::iterator ni=i->second->m_Vec.begin();
			ni!=i->second->m_Vec.end(); ++ni)
		{
			delete (*ni)->m_Node;
		}
		i->second->m_Vec.clear();
	}
	
	m_NodeDescMap.clear();	
}

void Graph::Create(unsigned int id, Type t, float v)
{
	unsigned int index=m_NodeDescMap[t]->NewIndex();
	unsigned int oldid=m_NodeDescMap[t]->m_Vec[index]->m_ID;

//cerr<<"create id:"<<id<<" index:"<<index<<" type:"<<t<<" value:"<<v<<endl;
	
	map<unsigned int,GraphNode*>::iterator i=m_NodeMap.find(oldid);
	if (i!=m_NodeMap.end()) m_NodeMap.erase(i);
	
	list<unsigned int>::iterator remove;
	bool found=false;
	for (list<unsigned int>::iterator ri=m_RootNodes.begin();
		ri!=m_RootNodes.end(); ++ri)
	{
		if (*ri==oldid) 
		{
			remove=ri;
			found=true;
		}
	}
	if (found) m_RootNodes.erase(remove);
	
	m_NodeDescMap[t]->m_Vec[index]->m_ID=id;
	m_NodeDescMap[t]->m_Vec[index]->m_Node->Clear();
	m_NodeMap[id]=m_NodeDescMap[t]->m_Vec[index]->m_Node;
		
	if (t==TERMINAL)
	{
		TerminalNode *terminal = dynamic_cast<TerminalNode*>(m_NodeMap[id]);
		assert(terminal!=NULL);
		terminal->SetValue(v);
	}
}

void Graph::Connect(unsigned int id, unsigned int arg, unsigned int to)
{
//cerr<<"connect id "<<id<<" arg "<<arg<<" to "<<to<<endl;
	if (m_NodeMap[id]!=NULL && m_NodeMap[to]!=NULL)
	{
		m_NodeMap[id]->SetChild(arg,m_NodeMap[to]);
	}
}

void Graph::Play(float time, unsigned int id)
{
//cerr<<"play id "<<id<<endl;
	if (m_NodeMap[id]!=NULL)
	{
		m_NodeMap[id]->Trigger(time);
		m_RootNodes.push_back(id);
		
		while (m_RootNodes.size()>m_MaxPlaying)
		{
			m_RootNodes.erase(m_RootNodes.begin());
		}
	}
}

void Graph::Process(unsigned int bufsize, Sample &in)
{
	for(list<unsigned int>::iterator i=m_RootNodes.begin();
		i!=m_RootNodes.end(); ++i)
	{
		if (m_NodeMap[*i]!=NULL)
		{
			m_NodeMap[*i]->Process(bufsize);
			in.MulClipMix(m_NodeMap[*i]->GetOutput(),0.1);
		}
	}
}
