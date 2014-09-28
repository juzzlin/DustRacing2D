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
#include "mcmathutil.hh"
#include "mcshape.hh"

MCImpulseGenerator::MCImpulseGenerator()
{}

MCContact * MCImpulseGenerator::getDeepestInterpenetration(
    const std::vector<MCContact *> & contacts)
{
    MCFloat maxDepth = 0;
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
    if (!pa.stationary())
    {
        const MCFloat invMassA    = pa.invMass();
        const MCFloat invMassB    = pb.invMass();
        const MCFloat massScaling = invMassA / (invMassA + invMassB);

        pa.displace(displacement * massScaling);
    }
}

void MCImpulseGenerator::generateImpulsesFromContact(
    MCObject & pa, MCObject & pb, const MCContact & contact,
    const MCVector3dF & linearImpulse,
    MCFloat restitution)
{
    const MCFloat invMassA = pa.invMass();
    const MCFloat invMassB = pb.invMass();

    if (!pa.stationary())
    {
        const MCVector3dF & contactPoint(contact.contactPoint());
        MCVector3dF armA = contactPoint - pa.location();

        // Linear component
        const MCFloat massScaling = invMassA / (invMassA + invMassB);
        const MCFloat effRestitution = 1.0 + restitution;
        pa.addImpulse(linearImpulse * effRestitution * massScaling);

        // Angular component
        const MCVector3dF rotationalImpulse = MCVector3dF(linearImpulse) * MCWorld::metersPerPixel() % armA;
        pa.addAngularImpulse(-rotationalImpulse.k() * effRestitution * massScaling);
    }
}

void MCImpulseGenerator::resolvePositions(std::vector<MCObject *> & objs, MCFloat accuracy)
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

void MCImpulseGenerator::generateImpulsesFromDeepestContacts(
    std::vector<MCObject *> & objs)
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

                const MCFloat restitution(
                    std::min(pa.restitution(), pb.restitution()));

                const MCVector2dF velocityDelta(pb.velocity() - pa.velocity());
                const MCFloat projection = contact->contactNormal().dot(velocityDelta);

                if (projection > 0)
                {
                    const MCVector3dF linearImpulse(
                        contact->contactNormal() *
                        contact->contactNormal().dot(velocityDelta));

                    generateImpulsesFromContact(pa, pb, *contact, linearImpulse, restitution);
                    generateImpulsesFromContact(pb, pa, *contact, -linearImpulse, restitution);
                }

                // Remove contact with pa from pb, because it was already handled here.
                pb.deleteContacts(pa);
            }
        }

        object->deleteContacts();
    }
}
