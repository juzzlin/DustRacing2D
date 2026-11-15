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
#include "mcworld.hh"

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace {

//! Number of velocity iterations of the solver.
const int VELOCITY_ITERATIONS = 10;

//! Bodies approaching each other slower than this (in m/s) don't bounce.
//! This keeps resting contacts, e.g. a car pushing a wall, still.
const float RESTITUTION_THRESHOLD = 1.0f;

//! Overlap (in world units) that is allowed without position correction to avoid jitter.
const float LINEAR_SLOP = 0.05f;

//! Velocities of a body during the iterations of the solver.
struct Body
{
    MCVector2dF center;

    //! In world units per step.
    MCVector2dF velocity;

    //! In radians per step, so that it matches the linear velocity.
    float angularVelocity = 0;

    float invMass = 0;

    //! In world units, matching the linear quantities.
    float invMomentOfInertia = 0;

    MCVector2dF initialVelocity;

    float initialAngularVelocity = 0;
};

//! A contact point between two bodies.
struct Constraint
{
    //! Index of the first point of the same manifold, or -1 if this is the first one.
    int manifoldStart = -1;

    Body * a = nullptr;

    Body * b = nullptr;

    //! Pushes a out of b.
    MCVector2dF normal;

    MCVector2dF ra;

    MCVector2dF rb;

    float normalMass = 0;

    //! Separating velocity the solver aims for, due to restitution.
    float targetVelocity = 0;

    //! Accumulated impulse. Contacts can only push, so it stays positive.
    float impulse = 0;

    float tangentMass = 0;

    float friction = 0;

    //! Accumulated friction impulse. Limited by the normal impulse.
    float tangentImpulse = 0;
};

float cross(const MCVector2dF & a, const MCVector2dF & b)
{
    return a.i() * b.j() - a.j() * b.i();
}

//! \return cross product of a scalar angular velocity and a vector.
MCVector2dF cross(float w, const MCVector2dF & r)
{
    return { -w * r.j(), w * r.i() };
}

MCVector2dF pointVelocity(const Body & body, const MCVector2dF & r)
{
    return body.velocity + cross(body.angularVelocity, r);
}

void applyImpulse(Body & body, const MCVector2dF & r, const MCVector2dF & impulse)
{
    body.velocity += impulse * body.invMass;
    body.angularVelocity += body.invMomentOfInertia * cross(r, impulse);
}

float normalVelocity(const Constraint & constraint)
{
    return (pointVelocity(*constraint.a, constraint.ra) - pointVelocity(*constraint.b, constraint.rb)).dot(constraint.normal);
}

void applyNormalImpulse(Constraint & constraint, float delta)
{
    const MCVector2dF impulse = constraint.normal * delta;
    applyImpulse(*constraint.a, constraint.ra, impulse);
    applyImpulse(*constraint.b, constraint.rb, -impulse);
}

/*! Coulomb friction: stops the sliding of the contact point, but with an impulse that is at
 *  most the friction coefficient times the normal impulse. So friction never reverses the motion. */
void solveFriction(Constraint & constraint)
{
    if (constraint.friction <= 0 || constraint.tangentMass <= 0)
    {
        return;
    }

    const MCVector2dF tangent { -constraint.normal.j(), constraint.normal.i() };
    const float velocity = (pointVelocity(*constraint.a, constraint.ra) - pointVelocity(*constraint.b, constraint.rb)).dot(tangent);
    const float limit = constraint.friction * constraint.impulse;
    const float newImpulse = std::clamp(constraint.tangentImpulse - velocity * constraint.tangentMass, -limit, limit);
    const float delta = newImpulse - constraint.tangentImpulse;
    constraint.tangentImpulse = newImpulse;

    const MCVector2dF impulse = tangent * delta;
    applyImpulse(*constraint.a, constraint.ra, impulse);
    applyImpulse(*constraint.b, constraint.rb, -impulse);
}

void solveSingle(Constraint & constraint)
{
    const float newImpulse = std::max(constraint.impulse + (constraint.targetVelocity - normalVelocity(constraint)) * constraint.normalMass, 0.0f);
    const float delta = newImpulse - constraint.impulse;
    constraint.impulse = newImpulse;
    applyNormalImpulse(constraint, delta);
}

/*! Solves the two points of a manifold together as a linear complementarity problem, like the block
 *  solver of Box2D. Solving them one at a time would converge slowly and leave e.g. a box lying flat
 *  against a wall with a small, wrong spin.
 *  \return false if the problem is ill-conditioned and the points need to be solved one at a time. */
bool solvePair(Constraint & c1, Constraint & c2)
{
    const auto & a = *c1.a;
    const auto & b = *c1.b;
    const float rn1a = cross(c1.ra, c1.normal);
    const float rn1b = cross(c1.rb, c1.normal);
    const float rn2a = cross(c2.ra, c2.normal);
    const float rn2b = cross(c2.rb, c2.normal);
    const float k11 = a.invMass + b.invMass + a.invMomentOfInertia * rn1a * rn1a + b.invMomentOfInertia * rn1b * rn1b;
    const float k22 = a.invMass + b.invMass + a.invMomentOfInertia * rn2a * rn2a + b.invMomentOfInertia * rn2b * rn2b;
    const float k12 = a.invMass + b.invMass + a.invMomentOfInertia * rn1a * rn2a + b.invMomentOfInertia * rn1b * rn2b;
    const float determinant = k11 * k22 - k12 * k12;
    if (k11 * k11 >= 1000.0f * determinant)
    {
        return false;
    }

    // Find impulses x >= 0 so that the velocities v = K x + b' >= 0 and x_i * v_i = 0
    const float accumulated1 = c1.impulse;
    const float accumulated2 = c2.impulse;
    const float b1 = normalVelocity(c1) - c1.targetVelocity - (k11 * accumulated1 + k12 * accumulated2);
    const float b2 = normalVelocity(c2) - c2.targetVelocity - (k12 * accumulated1 + k22 * accumulated2);

    float x1 = 0;
    float x2 = 0;
    if (const float y1 = -(k22 * b1 - k12 * b2) / determinant, y2 = -(k11 * b2 - k12 * b1) / determinant; y1 >= 0 && y2 >= 0)
    {
        x1 = y1; // Both points push
        x2 = y2;
    }
    else if (const float y = -b1 / k11; y >= 0 && k12 * y + b2 >= 0)
    {
        x1 = y; // Only the first point pushes
    }
    else if (const float y = -b2 / k22; y >= 0 && k12 * y + b1 >= 0)
    {
        x2 = y; // Only the second point pushes
    }
    else if (b1 < 0 || b2 < 0)
    {
        return false; // No solution found, which is possible only due to rounding errors
    }

    c1.impulse = x1;
    c2.impulse = x2;
    applyNormalImpulse(c1, x1 - accumulated1);
    applyNormalImpulse(c2, x2 - accumulated2);
    return true;
}

} // namespace

MCImpulseGenerator::MCImpulseGenerator()
{
}

MCContact * MCImpulseGenerator::getDeepestInterpenetration(const std::vector<MCContact *> & contacts)
{
    float maxDepth = 0;
    MCContact * bestContact = nullptr;
    for (auto && contact : contacts)
    {
        if (contact->interpenetrationDepth() > maxDepth)
        {
            maxDepth = contact->interpenetrationDepth();
            bestContact = contact;
        }
    }
    return bestContact;
}

void MCImpulseGenerator::displace(MCObject & object, MCObject & other, const MCVector2dF & displacement)
{
    if (!object.physicsComponent().isStationary())
    {
        const float invMass = object.physicsComponent().invMass();
        const float otherInvMass = other.physicsComponent().isStationary() ? 0 : other.physicsComponent().invMass();
        const float massScaling = invMass / (invMass + otherInvMass);
        object.displace(MCVector3dF { displacement * massScaling });
    }
}

void MCImpulseGenerator::generateImpulses(std::vector<MCObject *> & objects, float timeStep)
{
    const float metersPerUnit = MCWorld::metersPerUnit();
    const float restitutionThreshold = RESTITUTION_THRESHOLD / metersPerUnit * timeStep;

    std::unordered_map<MCObject *, Body> bodies;
    const auto body = [&](MCObject & object) -> Body & {
        auto && [iter, inserted] = bodies.try_emplace(&object);
        if (inserted)
        {
            auto && physics = object.physicsComponent();
            auto && b = iter->second;
            b.center = MCVector2dF { object.location() };
            b.velocity = b.initialVelocity = MCVector2dF { physics.velocity() };
            b.angularVelocity = b.initialAngularVelocity = physics.angularVelocity() * timeStep;
            if (!physics.isStationary())
            {
                b.invMass = physics.invMass();
                // The moment of inertia is in kg m^2, so convert it to world units
                b.invMomentOfInertia = physics.momentOfInertia() > 0 ? metersPerUnit * metersPerUnit / physics.momentOfInertia() : 0;
            }
        }
        return iter->second;
    };

    // The contacts of a pair are stored to both objects with opposite normals: use each pair once
    std::vector<Constraint> constraints;
    for (auto && object : objects)
    {
        for (auto && [other, contacts] : object->contacts())
        {
            if (object > other && other->contacts().count(object))
            {
                continue;
            }

            const float restitution = std::min(object->physicsComponent().restitution(), other->physicsComponent().restitution());
            const float friction = std::sqrt(object->physicsComponent().contactFriction() * other->physicsComponent().contactFriction());
            const int firstOfManifold = static_cast<int>(constraints.size());
            for (auto && contact : contacts)
            {
                Constraint constraint;
                if (static_cast<int>(constraints.size()) > firstOfManifold)
                {
                    constraint.manifoldStart = firstOfManifold;
                }
                constraint.a = &body(*object);
                constraint.b = &body(*other);
                constraint.normal = contact->contactNormal();
                constraint.ra = contact->contactPoint() - constraint.a->center;
                constraint.rb = contact->contactPoint() - constraint.b->center;

                const float rna = cross(constraint.ra, constraint.normal);
                const float rnb = cross(constraint.rb, constraint.normal);
                const float k = constraint.a->invMass + constraint.b->invMass + constraint.a->invMomentOfInertia * rna * rna + constraint.b->invMomentOfInertia * rnb * rnb;
                if (k <= 0)
                {
                    continue;
                }
                constraint.normalMass = 1.0f / k;

                const MCVector2dF tangent { -constraint.normal.j(), constraint.normal.i() };
                const float rta = cross(constraint.ra, tangent);
                const float rtb = cross(constraint.rb, tangent);
                const float kt = constraint.a->invMass + constraint.b->invMass + constraint.a->invMomentOfInertia * rta * rta + constraint.b->invMomentOfInertia * rtb * rtb;
                constraint.tangentMass = kt > 0 ? 1.0f / kt : 0;
                constraint.friction = friction;

                const float approachVelocity = (pointVelocity(*constraint.a, constraint.ra) - pointVelocity(*constraint.b, constraint.rb)).dot(constraint.normal);
                if (approachVelocity < -restitutionThreshold)
                {
                    constraint.targetVelocity = -restitution * approachVelocity;
                }

                constraints.push_back(constraint);
            }
        }
    }

    for (int iteration = 0; iteration < VELOCITY_ITERATIONS; iteration++)
    {
        // Friction first, because the normal impulses are more important and the last word wins
        for (auto && constraint : constraints)
        {
            solveFriction(constraint);
        }

        for (size_t i = 0; i < constraints.size(); i++)
        {
            auto && constraint = constraints.at(i);
            const bool isPairOfPoints = constraint.manifoldStart < 0 && i + 1 < constraints.size() && constraints.at(i + 1).manifoldStart == static_cast<int>(i);
            if (isPairOfPoints && solvePair(constraint, constraints.at(i + 1)))
            {
                i++;
                continue;
            }

            solveSingle(constraint);
        }
    }

    // Hand the changes of the velocities to the bodies as impulses, so that e.g. damage models can react
    for (auto && [object, b] : bodies)
    {
        auto && physics = object->physicsComponent();
        if (!physics.isStationary())
        {
            physics.addImpulse(MCVector3dF { b.velocity - b.initialVelocity }, true);
            physics.addAngularImpulse((b.angularVelocity - b.initialAngularVelocity) / timeStep, true);
        }
    }

    for (auto && object : objects)
    {
        object->deleteContacts();
    }
}

void MCImpulseGenerator::resolvePositions(std::vector<MCObject *> & objects, float accuracy)
{
    for (auto && object : objects)
    {
        for (auto && [other, contacts] : object->contacts())
        {
            if (const auto deepestContact = getDeepestInterpenetration(contacts); deepestContact)
            {
                const float correction = std::max(deepestContact->interpenetrationDepth() - LINEAR_SLOP, 0.0f) * accuracy;
                const MCVector2dF displacement = deepestContact->contactNormal() * correction;
                displace(*object, *other, displacement);
                displace(*other, *object, -displacement);

                other->deleteContacts(*object);
                object->deleteContacts(*other);
                break;
            }
        }

        object->deleteContacts();
    }
}
