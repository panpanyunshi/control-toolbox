/***********************************************************************************
Copyright (c) 2017, Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo,
Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of ETH ZURICH nor the names of its contributors may be used
      to endorse or promote products derived from this software without specific
      prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL ETH ZURICH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************/


#include <ct/models/Quadrotor/quadrotor_dynamics/QuadrotorDynamics.hpp>

#include <iostream>

using namespace std;

namespace ct {
namespace models {

using namespace quadrotor;

quadrotor::state_vector_t quadrotor_ode(const quadrotor::state_vector_t& x, const quadrotor::control_vector_t& u)
{
	assert(x == x);
	assert(u == u);

	// positions
	double qxQ = x(0);  // x
	double qyQ = x(1);  // y
	double qzQ = x(2);  // z

	// euler angles xyz
	double qph = x(3);
	double qth = x(4);
	double qps = x(5);

	// positions derivatives
	double dqxQ = x(6);  // x
	double dqyQ = x(7);  // y
	double dqzQ = x(8);  // z

	// euler angle derivatives xyz
	double dqph = x(9);
	double dqth = x(10);
	double dqps = x(11);

	// Applied force and momentums
	double Fz = u(0);
	double Mx = u(1);
	double My = u(2);
	double Mz = u(3);

	double t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13;

	t2 = 1.0 / mQ;
	t3 = std::cos(qth);
	t4 = std::sin(qth);
	t5 = 1.0 / Thxxyy;
	t6 = std::cos(qps);
	t7 = std::sin(qps);
	t8 = dqph * dqph;
	t9 = qth * 2.0;
	t10 = std::sin(t9);
	t11 = 1.0 / t3;
	t12 = Thzz * Thzz;
	t13 = t3 * t3;

	quadrotor::state_vector_t dx;
	dx.setZero();

	dx(0) = dqxQ;
	dx(1) = dqyQ;
	dx(2) = dqzQ;
	dx(3) = dqph;
	dx(4) = dqth;
	dx(5) = dqps;
	dx(6) = Fz * t2 * t4;
	dx(7) = -Fz * t2 * t3 * std::sin(qph);
	dx(8) = t2 * (mQ * 9.81E2 - Fz * t3 * std::cos(qph) * 1.0E2) * (-1.0 / 1.0E2);
	dx(9) = -t5 * t11 *
			(-Mx * t6 + My * t7 + Thzz * dqps * dqth - Thxxyy * dqph * dqth * t4 * 2.0 + Thzz * dqph * dqth * t4);
	dx(10) = t5 * (Mx * t7 + My * t6 - Thxxyy * t8 * t10 * (1.0 / 2.0) + Thzz * t8 * t10 * (1.0 / 2.0) +
					  Thzz * dqph * dqps * t3);
	dx(11) = (t5 * t11 * (Mz * Thxxyy * t3 + dqph * dqth * t12 - dqph * dqth * t12 * t13 + dqps * dqth * t4 * t12 -
							 Thxxyy * Thzz * dqph * dqth * 2.0 - Mx * Thzz * t4 * t6 + My * Thzz * t4 * t7 +
							 Thxxyy * Thzz * dqph * dqth * t13)) /
			 Thzz;


	// a hacky check to prevent integration from becoming unstable:
	if (x.norm() > 1e20)
		dx.setZero();

	return dx;
}
}
}
