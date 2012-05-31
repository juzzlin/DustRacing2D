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
#include "../mcobject.hh"
#include "../mcshape.hh"

class MCImpulseGeneratorImpl
{
    void generateImpulsesFromContact(
        MCObject & pa, MCObject & pb, const MCContact & contact,
        const MCVector3dF & linearImpulse,
        MCFloat restitution);

    void generateImpulsesFromDeepestContacts(MCObject & object);

    void resolvePosition(MCObject & object, MCFloat accuracy);

    void displace(MCObject & pa, MCObject & pb, const MCVector3dF & displacement);

    MCContact * getDeepestInterpenetration(const std::vector<MCContact *> & contacts);

    friend class MCImpulseGenerator;
};

MCContact * MCImpulseGeneratorImpl::getDeepestInterpenetration(
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

void MCImpulseGeneratorImpl::displace(
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

void MCImpulseGeneratorImpl::generateImpulsesFromContact(
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
        MCWorld::toMeters(armA);
        MCVector3dF armB = contactPoint - pb.location();
        MCWorld::toMeters(armB);

        // Linear component
        const MCFloat massScaling = invMassA / (invMassA + invMassB);

        // This ad-hoc scaling affects the balance between linear and angular components.
        MCFloat linearBalance = 1.0f;
        if (pa.shape())
        {
            MCFloat d = pa.shape()->radius();
            MCWorld::toMeters(d);
            MCFloat linearBalance = 1.0f - armA.lengthFast() / d;
            linearBalance = linearBalance < 0 ? 0 : linearBalance;
        }

        const MCFloat effRestitution = 1.0f + restitution;
        pa.addLinearImpulse(
            linearImpulse * effRestitution * massScaling * linearBalance);

        // Angular component
        const MCVector3dF rotationalImpulse =
            MCVector3dF(linearImpulse * pa.mass()) % armA;
        const MCFloat magnitude = rotationalImpulse.k() / pa.momentOfInertia();
        pa.addAngularImpulse(2.0f * 3.1415f * -magnitude * effRestitution * massScaling);
    }
}

void MCImpulseGeneratorImpl::resolvePosition(MCObject & object, MCFloat accuracy)
{
    auto iter(object.contacts().begin());
    for (; iter != object.contacts().end(); iter++)
    {
        const MCContact * contact = getDeepestInterpenetration(iter->second);
        if (contact)
        {
            MCObject & pa(object);
            MCObject & pb(contact->object());

            const MCVector3dF displacement(contact->contactNormal() * contact->interpenetrationDepth() * accuracy);

            displace(pa, pb, displacement);
            displace(pb, pa, -displacement);
        }
    }
}

void MCImpulseGeneratorImpl::generateImpulsesFromDeepestContacts(
    MCObject & object)
{
    auto iter(object.contacts().begin());
    for (; iter != object.contacts().end(); iter++)
    {
        const MCContact * contact = getDeepestInterpenetration(iter->second);
        if (contact)
        {
            MCObject & pa(object);
            MCObject & pb(contact->object());

            const MCFloat restitution(
                std::max(pa.restitution(), pb.restitution()));

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

    object.deleteContacts();
}

MCImpulseGenerator::MCImpulseGenerator()
: m_pImpl(new MCImpulseGeneratorImpl)
{}

MCImpulseGenerator::~MCImpulseGenerator()
{
    delete m_pImpl;
}

void MCImpulseGenerator::resolvePosition(MCObject & object, MCFloat accuracy)
{
    m_pImpl->resolvePosition(object, accuracy);
}

void MCImpulseGenerator::generateImpulsesFromDeepestContacts(
    MCObject & object)
{
    m_pImpl->generateImpulsesFromDeepestContacts(object);
}

