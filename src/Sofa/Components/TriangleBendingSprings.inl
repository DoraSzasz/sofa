//
// C++ Implementation: TriangleBendingSprings
//
// Description:
//
//
// Author: The SOFA team </www.sofa-framework.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "TriangleBendingSprings.h"
#include "MeshTopology.h"
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

namespace Sofa
{
using namespace Core;

namespace Components
{

template<class DataTypes>
TriangleBendingSprings<DataTypes>::TriangleBendingSprings(Core::MechanicalModel<DataTypes>* object)
    : Sofa::Components::StiffSpringForceField<DataTypes>(object)
    , stiffness( dataField(&stiffness,(Real)1.0,"stiffness","Stiffness of the bending springs") )
    , dampingRatio( dataField(&dampingRatio,(Real)0.0,"dampingRatio","Damping ratio of the bending springs. The actual damping coefficient is the stiffness times the damping ratio.") )
    , dof(NULL)
{}


template<class DataTypes>
TriangleBendingSprings<DataTypes>::~TriangleBendingSprings()
{}

template<class DataTypes>
void TriangleBendingSprings<DataTypes>::addSpring( unsigned a, unsigned b )
{
    const VecCoord& x = *dof->getX();
    Real s = stiffness.getValue();
    Real d = stiffness.getValue() * dampingRatio.getValue();
    Real l = (x[a]-x[b]).norm();
    this->SpringForceField<DataTypes>::addSpring(a,b, s, d, l );
    cout<<"=================================TriangleBendingSprings<DataTypes>::addSpring "<<a<<", "<<b<<endl;
}

template<class DataTypes>
void TriangleBendingSprings<DataTypes>::registerTriangle( unsigned a, unsigned b, unsigned c, std::map<IndexPair, unsigned>& edgeMap)
{
    cout<<"=================================TriangleBendingSprings<DataTypes>::registerTriangle "<<a<<", "<<b<<", "<<c<<endl;
    using namespace std;
    {
        IndexPair edge(a<b ? a : b,a<b ? b : a);
        unsigned opposite = c;
        if( edgeMap.find( edge ) == edgeMap.end() )
        {
            edgeMap[edge] = opposite;
        }
        else
        {
            // create a spring between the opposite
            this->addSpring(opposite,edgeMap[edge]);
        }
    }

    {
        IndexPair edge(b<c ? b : c,b<c ? c : b);
        unsigned opposite = a;
        if( edgeMap.find( edge ) == edgeMap.end() )
        {
            edgeMap[edge] = opposite;
        }
        else
        {
            // create a spring between the opposite
            this->addSpring(opposite,edgeMap[edge]);
        }
    }

    {
        IndexPair edge(c<a ? c : a,c<a ? a : c);
        unsigned  opposite = b;
        if( edgeMap.find( edge ) == edgeMap.end() )
        {
            edgeMap[edge] = opposite;
        }
        else
        {
            // create a spring between the opposite
            this->addSpring(opposite,edgeMap[edge]);
        }
    }

}



template<class DataTypes>
void TriangleBendingSprings<DataTypes>::init()
{
    dof = dynamic_cast<MechanicalObject<DataTypes>*>( this->getContext()->getMechanicalModel() );
    assert(dof);
    //cout<<"==================================TriangleBendingSprings<DataTypes>::init(), dof size = "<<dof->getX()->size()<<endl;

    // Set the bending springs

    std::map< IndexPair, unsigned > edgeMap;
    MeshTopology* topology = dynamic_cast<MeshTopology*>( this->getContext()->getTopology() );
    assert( topology );

    const MeshTopology::SeqTriangles& triangles = topology->getTriangles();
    cout<<"==================================TriangleBendingSprings<DataTypes>::init(), triangles size = "<<triangles.size()<<endl;
    for( unsigned i= 0; i<triangles.size(); ++i )
    {
        const MeshTopology::Triangle& face = triangles[i];
        {
            registerTriangle( face[0], face[1], face[2], edgeMap );
        }

    }

    const MeshTopology::SeqQuads& quads = topology->getQuads();
    cout<<"==================================TriangleBendingSprings<DataTypes>::init(), quad size = "<<topology->getQuads().size()<<endl;
    for( unsigned i= 0; i<quads.size(); ++i )
    {
        const MeshTopology::Quad& face = quads[i];
        {
            registerTriangle( face[0], face[1], face[2], edgeMap );
            registerTriangle( face[0], face[2], face[3], edgeMap );
        }

    }

    // init the parent class
    Sofa::Components::StiffSpringForceField<DataTypes>::init();
}


}

}



