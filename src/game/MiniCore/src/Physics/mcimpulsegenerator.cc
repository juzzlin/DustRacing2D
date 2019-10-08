// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcimpulsegenerator.hh"
#include "mccontact.hh"
#include "mcobject.hh"
#include "mcphysicscomponent.hh"
#include "mcmathutil.hh"
#include "mcshape.hh"

MCImpulseGenerator::MCImpulseGenerator()
{}

MCContact * MCImpulseGenerator::getDeepestInterpenetration(
    const std::vector<MCContact *> & contacts)
{
    float maxDepth = 0;
    MCContact * bestContact = nullptr;
    for (MCContact * contact : contacts)
    {
        if (contact->interpenetrationDepth() > maxDepth)
        {
            maxDepth = contact->interpenetrationDepth();
            bestContact = contact;
        }
    }
    return bestContact;
}

void MCImpulseGenerator::displace(
     MCObject & pa, MCObject & pb, const MCVector3dF & displacement)
{
    if (!pa.physicsComponent().isStationary())
    {
        const float invMassA    = pa.physicsComponent().invMass();
        const float invMassB    = pb.physicsComponent().invMass();
        const float massScaling = invMassA / (invMassA + invMassB);

        pa.displace(displacement * massScaling);
    }
}

void MCImpulseGenerator::generateImpulsesFromContact(
    MCObject & pa, MCObject & pb, const MCContact & contact,
    const MCVector3dF & normalVelocityDelta,
    float restitution)
{
    if (!pa.physicsComponent().isStationary())
    {

        const MCVector3dF & contactPoint(contact.contactPoint());

        // Linear component
		const float invMassA = pa.physicsComponent().invMass();
        const float invMassB = pb.physicsComponent().invMass();
        const float massScaling = invMassA / (invMassA + invMassB);
        const float effRestitution = 1.0f + restitution;
        pa.physicsComponent().addImpulse(normalVelocityDelta * effRestitution * massScaling, true);

        // Angular component
        const MCVector3dF armA = (contactPoint - pa.location()) * MCWorld::metersPerUnit(); // NOTE PHYSICS: conversion to physical units
        const MCVector3dF rotationalImpulse = normalVelocityDelta % armA;
        
		const float invInertiaA = pa.physicsComponent().invMomentOfInertia();
        const float invInertiaB = pb.physicsComponent().invMomentOfInertia();
        const float inertiaScaling = invInertiaA / (invInertiaA + invInertiaB);
		
		// TODO PHYSICS: clarify this calculation. It's not dimensions clean, since the rotationalImpulse is not a 
		// true rotational impluse. 
        pa.physicsComponent().addAngularImpulse(-rotationalImpulse.k() * effRestitution * massScaling, true);
    }
}

void MCImpulseGenerator::resolvePositions(std::vector<MCObject *> & objs, float accuracy)
{
    for (MCObject * object : objs)
    {
        auto iter(object->contacts().begin());
        for (; iter != object->contacts().end(); iter++)
        {
            const MCContact * contact = getDeepestInterpenetration(iter->second);
            if (contact)
            {
                MCObject & pa(*object);
                MCObject & pb(contact->object());

                const MCVector3dF displacement(
                    contact->contactNormal() * contact->interpenetrationDepth() * accuracy);

                displace(pa, pb, displacement);
                displace(pb, pa, -displacement);

                pb.deleteContacts(pa);
                pa.deleteContacts(pb);
            }
        }

        object->deleteContacts();
    }
}

void MCImpulseGenerator::generateImpulsesFromDeepestContacts(std::vector<MCObject *> & objs)
{
    for (MCObject * object : objs)
    {
        auto iter(object->contacts().begin());
        for (; iter != object->contacts().end(); iter++)
        {
            const MCContact * contact = getDeepestInterpenetration(iter->second);
            if (contact)
            {
                MCObject & pa(*object);
                MCObject & pb(contact->object());

                const float restitution(
                    std::min(pa.physicsComponent().restitution(), pb.physicsComponent().restitution()));

                const MCVector2dF velocityDelta(pb.physicsComponent().velocity() - pa.physicsComponent().velocity());
                const float projection = contact->contactNormal().dot(velocityDelta);

                if (projection > 0)
                {
                    const MCVector3dF normalVelocityDelta(
                        contact->contactNormal() *
                        contact->contactNormal().dot(velocityDelta));

                    generateImpulsesFromContact(pa, pb, *contact, normalVelocityDelta, restitution);
                    generateImpulsesFromContact(pb, pa, *contact, -normalVelocityDelta, restitution);
                }

                // Remove contact with pa from pb, because it was already handled here.
                pb.deleteContacts(pa);

                break;
            }
        }

        object->deleteContacts();
    }
}
