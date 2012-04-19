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

void MCImpulseGenerator::generateImpulses(MCObject & object, MCContact & contact)
{
    MCObject & pa(object);
    MCObject & pb(contact.object());

    const MCFloat restitution(
        std::min(pa.restitution(), pb.restitution()));

    const MCVector2dF velocityDelta(pb.velocity() - pa.velocity());

    const MCVector3dF linearImpulse(
        contact.contactNormal() * contact.contactNormal().dot(velocityDelta));

    const MCVector3dF displacement(
        contact.contactNormal() * contact.interpenetrationDepth());

    const MCFloat invMassA = pa.invMass();
    const MCFloat invMassB = pb.invMass();
    const MCFloat invInerA = pa.invMomentOfInertia();
    const MCFloat invInerB = pb.invMomentOfInertia();

    if (!pa.stationary()) {

        // Linear component
        const MCFloat massScaling = invMassA / (invMassA + invMassB);
        pa.displace(displacement * massScaling);
        pa.addLinearImpulse((linearImpulse + linearImpulse * restitution) * massScaling);

        // Angular component
        const MCVector3dF contactPoint(contact.contactPoint());
        const MCVector3dF arm = (contactPoint - pa.location());
        const MCVector3dF rotationalImpulse =
            MCVector3dF(linearImpulse * pa.mass()) % arm / pa.momentOfInertia();

        const MCFloat magnitude   = rotationalImpulse.k();
        const MCFloat inerScaling = invInerA / (invInerA + invInerB);
        pa.addRotationalImpulse((-magnitude - magnitude * restitution) * inerScaling);
        pa.setCenterOfRotation(pa.location());
    }

    if (!pb.stationary()) {

        // Linear component
        const MCFloat massScaling = invMassB / (invMassA + invMassB);
        pb.displace(-displacement * massScaling);
        pb.addLinearImpulse((-linearImpulse - linearImpulse * restitution) * massScaling);

        // Angular component
        const MCVector3dF contactPoint(contact.contactPoint());
        const MCVector3dF arm = (contactPoint - pb.location());
        const MCVector3dF rotationalImpulse =
            MCVector3dF(linearImpulse * pb.mass()) % arm / pb.momentOfInertia();

        const MCFloat magnitude   = rotationalImpulse.k();
        const MCFloat inerScaling = invInerB / (invInerA + invInerB);
        pb.addRotationalImpulse((magnitude + magnitude * restitution) * inerScaling);
        pb.setCenterOfRotation(pb.location());
    }

    // Remove contacts with pa from pb, because physically it was already handled here
    pb.deleteContacts(pa);
}
