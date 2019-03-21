#include "sys.h"
#include "Gates.h"

namespace quantum {

namespace gates {

// Instantiation of specialized static constants.
QuBitField const sin<1, 4>::pi = sqrt_half;
QuBitField const sin<-1, 4>::pi = -sqrt_half;
QuBitField const cos<1, 4>::pi = sqrt_half;
QuBitField const cos<-1, 4>::pi = sqrt_half;
QuBitField const exp<1, 4>::i_pi{0, 0, 1, 1};   // (1 + i)·√½
QuBitField const exp<-1, 4>::i_pi{0, 0, 1, -1}; // (1 - i)·√½

namespace {

QMatrix::Scalar I_init[4] =
  {
    1, 0,
    0, 1
  };

QMatrix::Scalar X_init[4] =
  {
    0,  1,
    1,  0
  };

QMatrix::Scalar Y_init[4] =
  {
    0, -i,
    i,  0
  };

QMatrix::Scalar Z_init[4] =
  {
    1,  0,
    0, -1
  };

QMatrix::Scalar S_init[4] = // T^2
  {
    1,  0,
    0,  i
  };

QMatrix::Scalar S_inv_init[4] = // T^6
  {
    1,  0,
    0, -i
  };

QMatrix::Scalar T_init[4] =
  {
    1,  0,
    0,  exp<1,4>::i_pi
  };

QMatrix::Scalar T_inv_init[4] = // T^7
  {
    1,  0,
    0,  exp<-1,4>::i_pi
  };

QMatrix::Scalar H_init[4] =
  {
    cos<1,4>::pi, cos<-1,4>::pi,
    sin<1,4>::pi, sin<-1,4>::pi
  };

// Although normally a CNOT gate is drawn like this:
//
// |a⟩ ----o---- |a⟩
//         |
// |b⟩ ---(+)--- |a XOR b⟩
//
// see for example https://en.wikipedia.org/wiki/Controlled_NOT_gate#Operation,
// that image at the same time corresponds with an input state |a⟩ ⊗ |b⟩ = |ab⟩,
//                                                              ^___________^_____ control input.
// this program started with adopting what is being used by IBM on their site
// https://quantumexperience.ng.bluemix.net/qx/tutorial?sectionId=beginners-guide&page=006-Multi-Qubit_Gates~2F001-Multi-Qubit_Gates
// which show the following image:
//
// |00⟩ --> |00⟩
// |10⟩ --> |10⟩
// |01⟩ --> |11⟩
// |11⟩ --> |01⟩
//   ^        ^
//   |        |
//  control input.
//
// However, next IBM continuous with showing how to implement this
// with their composer. For example the |10⟩ --> |10⟩ corresponds to
// the circuit:                          ^
//                                       |
// qubit#                                |
//   |                                   |
//   v                                   |
// q[0] :  |0⟩ ------o---- |0⟩           |
//                   |                   |
// q[1] :  |0⟩ --X--(+)--- |1⟩      <----+
//                                       |
// in other words, they put the more significant bit at the bottom!?
// Surely this has been motivated by the desire to start counting
// from the top at 0, and subsequently associating this count with
// the bit index of the notation 10b = 2^1 + 2^0, where the latter
// is the qubit at the top thus (index 0).
//
// Clearly IBM made a mistake here, what they are drawing
// is a reversed CNOT. The CORRECT DRAWING - taking into account
// how they show things in the composer (aka, "upside down") is:
//
// q[0] :  |0⟩ --X--(+)--- |1⟩     }
//                   |             } -- in IBM notation, this is |01⟩.
// q[1] :  |0⟩ ------o---- |0⟩     }                              ^
//                                                                |_____________ control input.
// where the control input is now in the correct place (the left
// most qubit in the ket notation).
//
// This means that the table that was given is also wrong.
// As that is the table for the inverted CNOT. The table for
// the normal CNOT is (again, see https://en.wikipedia.org/wiki/Controlled_NOT_gate#Operation):
//
// |00⟩ --> |00⟩
// |01⟩ --> |01⟩
// |10⟩ --> |11⟩
// |11⟩ --> |10⟩
//  ^        ^
//  |        |
// control input.
//
// which then clearly corresponds to the following matrix:
//
QMatrixX::Scalar CX_init[16] =
  {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 1,
    0, 0, 1, 0
  };

} // namespace

std::array<QMatrix, number_of_gates> const gate = {
  QMatrix{X_init},
  QMatrix{Y_init},
  QMatrix{Z_init},
  QMatrix{S_init},
  QMatrix{S_inv_init},
  QMatrix{T_init},
  QMatrix{T_inv_init},
  QMatrix{H_init}
};

// Only needed in QState.cxx, but defined here to avoid static initialization fiasco.

// (SH)⁻¹
QMatrix const SH = gate[S] * gate[H];

// Identity matrix.
QMatrix const I{I_init};

// Controlled-NOT matrix.
QMatrixX const Controlled_X = Eigen::Matrix<QuBitField, 4, 4>{CX_init};

} // namespace gates

} // namespace quantum
