#include "Manifold.h"
#include <nclgl\Matrix3.h>
#include <nclgl\NCLDebug.h>
#include "PhysicsEngine.h"
#include <algorithm>

Manifold::Manifold() 
	: pnodeA(NULL)
	, pnodeB(NULL)
{
}

Manifold::~Manifold()
{

}

void Manifold::Initiate(PhysicsNode* nodeA, PhysicsNode* nodeB)
{
	contactPoints.clear();

	pnodeA = nodeA;
	pnodeB = nodeB;
}

void Manifold::ApplyImpulse()
{
	for (ContactPoint& contact : contactPoints)
	{
		SolveContactPoint(contact);
	}
}


void Manifold::SolveContactPoint(ContactPoint& c)
{

	Vector3 rA = c.relPosA;
	Vector3 rB = c.relPosB;

	// Velocity of the objects at the point of contact
	Vector3 vA = pnodeA->GetLinearVelocity() + Vector3::Cross(pnodeA->GetAngularVelocity(), rA);
	Vector3 vB = pnodeB->GetLinearVelocity() + Vector3::Cross(pnodeB->GetAngularVelocity(), rB);

	// Velocity of object B relative to object A
	Vector3 velocityBA = vB - vA;

	// Collision resolution
	float constraintMass =
		(pnodeA->GetInverseMass() + pnodeB->GetInverseMass()) +
		Vector3::Dot(c.colNormal,
			Vector3::Cross(pnodeA->GetInverseInertia()
				* Vector3::Cross(rA, c.colNormal), rA) +
			Vector3::Cross(pnodeB->GetInverseInertia() *
				Vector3::Cross(rB, c.colNormal), rB));

	if (constraintMass > 0.0f) {
		float jn = max(-Vector3::Dot(velocityBA,c.colNormal)+c.b_term,0.0f);
				float oldSumImpulseContact = c.sumImpulseContact;		c.sumImpulseContact = max(c.sumImpulseContact + jn, 0.0f);		jn = c.sumImpulseContact - oldSumImpulseContact;

				jn=jn/constraintMass;
				
				pnodeA->SetLinearVelocity(pnodeA->GetLinearVelocity()
				-c.colNormal*(jn*pnodeA->GetInverseMass()));
				pnodeB->SetLinearVelocity(pnodeB->GetLinearVelocity()
				+c.colNormal*(jn*pnodeB->GetInverseMass()));
				
				pnodeA->SetAngularVelocity(pnodeA->GetAngularVelocity()
				-pnodeA->GetInverseInertia()
				*Vector3::Cross(rA,c.colNormal*jn));
				pnodeB->SetAngularVelocity(pnodeB->GetAngularVelocity()
				+pnodeB->GetInverseInertia()
				*Vector3::Cross(rB,c.colNormal*jn));
	}

	// Friction
	Vector3 tangent = velocityBA -c.colNormal*Vector3::Dot(velocityBA,c.colNormal);

	float tangent_len = tangent.Length();

	if (tangent_len > 1e-6f)
	{
		tangent = tangent / tangent_len;
		float frictionalMass = (pnodeA->GetInverseMass()
			+ pnodeB->GetInverseMass()) + Vector3::Dot(tangent,
				Vector3::Cross(pnodeA->GetInverseInertia()
					*Vector3::Cross(rA, tangent), rA) +
				Vector3::Cross(pnodeB->GetInverseInertia()
					*Vector3::Cross(rB, tangent), rB));

		if (frictionalMass > 0.0f)
		{
			float frictionCoef =
				(pnodeA->GetFriction()*pnodeB->GetFriction());
			float jt = -Vector3::Dot(velocityBA, tangent)*frictionCoef;


			// Trying to ensure that:
			// 1. The collision doesn't solve itself backwards (push itself all the way through 
			// the other side of the object)
			// 2. The friction force doesn't exceed the main collision force (which changes over time)
			Vector3 oldImpulseFriction = c.sumImpulseFriction;
			c.sumImpulseFriction = c.sumImpulseFriction + tangent * jt;
			float len = c.sumImpulseFriction.Length();
			
			if (len > 0.0f && len > c.sumImpulseContact) {
				c.sumImpulseFriction = c.sumImpulseFriction / len * c.sumImpulseContact;
			}
			
			tangent = c.sumImpulseFriction - oldImpulseFriction;
			jt = 1.0f;
			// ****************
			jt = jt / frictionalMass;

			pnodeA->SetLinearVelocity(pnodeA->GetLinearVelocity()
				- tangent*(jt*pnodeA->GetInverseMass()));
			pnodeB->SetLinearVelocity(pnodeB->GetLinearVelocity()
				+ tangent*(jt*pnodeB->GetInverseMass()));

			pnodeA->SetAngularVelocity(pnodeA->GetAngularVelocity()
				- pnodeA->GetInverseInertia()
				*Vector3::Cross(rA, tangent*jt));
			pnodeB->SetAngularVelocity(pnodeB->GetAngularVelocity()
				+ pnodeB->GetInverseInertia()
				*Vector3::Cross(rB, tangent*jt));
		}
	}
}



void Manifold::PreSolverStep(float dt)
{
	std::random_shuffle(contactPoints.begin(), contactPoints.end());
	for (ContactPoint& contact : contactPoints)
	{
		UpdateConstraint(contact);
	}
}

void Manifold::UpdateConstraint(ContactPoint& c)
{
	//Reset total impulse forces computed this physics timestep 
	c.sumImpulseContact = 0.0f;
	c.sumImpulseFriction = Vector3(0.0f, 0.0f, 0.0f);
	c.b_term = 0.0f;
	// Baumgarte Offset : Adds energy to system to counter "constraint drift" -
	// Solving Errors that accumulate over time

	// This offset is slightly different to the one used to prevent constraint breaking
	// as we allow some minimum error (here an overlap of 1mm) before adding correctional energy
	// This allows us to keep generating a manifold for constantly colliding objects (instead of endless jitter)
	// This is a little hacky the slop is usually handled by  pretending the collision volume is larger than it is 
	// during the narrowphase so that manifolds are generated for colliding and almost colliding pairs

	const float baumgarteScalar = 0.1f;
	const float baumgarteSlop = 0.001f; 
	const float penetrationSlop = min(c.colPenetration + baumgarteSlop, 0.0f);
	
	c.b_term += -(baumgarteScalar / PhysicsEngine::Instance()->GetDeltaTime()) * penetrationSlop;
	//
	// Elasticity term : As elasticity is adding energy back into the system we can attach it to our
	// 'b' term which we already add to the 'jt' when solving the contact constraint
	const float elasticity = pnodeA->GetElasticity() * pnodeB->GetElasticity();
	//// Use total velocity going into the collision relative to the collision normal
	const float elasticityTerm = Vector3::Dot(c.colNormal,
		pnodeA->GetLinearVelocity()
		+Vector3::Cross(c.relPosA,pnodeA->GetAngularVelocity())
		-pnodeB->GetLinearVelocity()
		-Vector3::Cross(c.relPosB,pnodeB->GetAngularVelocity()));
	c.b_term += (elasticity * elasticityTerm) / contactPoints.size();
}

void Manifold::AddContact(const Vector3& globalOnA, const Vector3& globalOnB, const Vector3& normal, const float& penetration)
{
	//Get relative offsets from each object centre of mass
	// Used to compute rotational velocity at the point of contact.
	Vector3 r1 = (globalOnA - pnodeA->GetPosition());
	Vector3 r2 = (globalOnB - pnodeB->GetPosition());

	//Create our new contact descriptor
	ContactPoint contact;
	contact.relPosA = r1;
	contact.relPosB = r2;
	contact.colNormal = normal;
	contact.colNormal.Normalise();
	contact.colPenetration = penetration;

	contactPoints.push_back(contact);


	//What a stupid function!
	// - Manifold's normally persist over multiple frames, as in two colliding objects
	//   (especially in the case of stacking) will likely be colliding in a similar 
	//   setup the following couple of frames. So the accuracy therefore can be increased
	//   by using persistent manifolds, which will only be deleted when the two objects
	//   fail a narrowphase check. This means the manifolds can be quite busy, with lots of
	//   new contacts per frame, but to solve any convex manifold in 3D you really only need
	//   3 contacts (4 max), so tldr; perhaps you want persistent manifolds.. perhaps
	//   you want to put some code here to sort contact points.. perhaps this comment is even 
	//   more pointless than a passthrough function.. perhaps I should just stop typ
}

void Manifold::DebugDraw() const
{
	if (contactPoints.size() > 0)
	{
		//Loop around all contact points and draw them all as a line-loop
		Vector3 globalOnA1 = pnodeA->GetPosition() + contactPoints.back().relPosA;
		for (const ContactPoint& contact : contactPoints)
		{
			Vector3 globalOnA2 = pnodeA->GetPosition() + contact.relPosA;
			Vector3 globalOnB = pnodeB->GetPosition() + contact.relPosB;

			//Draw line to form area given by all contact points
			NCLDebug::DrawThickLineNDT(globalOnA1, globalOnA2, 0.02f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));

			//Draw descriptors for indivdual contact point
			NCLDebug::DrawPointNDT(globalOnA2, 0.05f, Vector4(0.0f, 0.5f, 0.0f, 1.0f));
			NCLDebug::DrawThickLineNDT(globalOnB, globalOnA2, 0.01f, Vector4(1.0f, 0.0f, 1.0f, 1.0f));

			globalOnA1 = globalOnA2;
		}
	}
}