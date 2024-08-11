# quantum
Play with quantum gates

## Usage

Edit <code>src/quantum.cxx</code> and make a circuit using C++ code, but in a way that looks
much like the normal way quantum circuits are composed. For example,

<pre>
    q[1] - H - co(0) - H - measure(0);
    q[0]     - CX(0) - S - H - T_inv - H - measure(1);
</pre>

Here each <code>q[...]</code> is a qubit, <code>H</code> is the Hademar gate, etc
(see [Gates.h](https://github.com/CarloWood/quantum/blob/master/src/Gates.h) for a list of all
standard gates). <code>co(int)</code> is the control input of a controlled CNOT while
<code>CX(int)</code> with the same int, is the corresponding controlled input. Finally,
<code>measure(int)</code> measures the classical bit <code>int</code>. You must specify
the number of qubits and classical bits at the moment you create <code>q</code>.

Note that there can only be a single minus sign between the gates.

## Wave function collapse and shots

<b>THIS IMPLEMENTATION DOES NOT COLLAPSE THE WAVE FUNCTION</b>.

This is important to me, because I don't believe in wave function collapses.
That only appears to be the case when you yourself get entangled with the quantum
superposition that you try to measure.

Therefore, you can continue to put more gates after a <code>measure(int)</code>
as well: the state was not changed (it WAS however entangled with the measured
"classical" bit).

As a result, you do not need any "shots" (run the circuit many times to get approximate
statistics). The statistics on chance of the measurements are simply calculated in parallel,
using the normal quantum computer power of doing things in parallel.

## Accuracy, round off errors and noise.

<b>THIS IMPLEMENTATION HAS NO NOISE, AND NO ROUND OFF ERRORS.</b>

This implementation uses a field extension to the rationals, for which GMP is used
for infinite accuracy. Hence, the results are infinitely precise.

For example, the above circuit results in the following output:

<pre>
0₁0₀: (1/4 + 1/4·i - 1/2·i·√½)·|0₁0₀⟩   Chance: 1/4 - 1/4·√½
1₁0₀: (1/4 + 1/4·i + 1/2·i·√½)·|0₁1₀⟩   Chance: 1/4 + 1/4·√½
0₁1₀: (1/4 - 1/4·i + 1/2·√½)·|1₁0₀⟩     Chance: 1/4 + 1/4·√½
1₁1₀: (1/4 - 1/4·i - 1/2·√½)·|1₁1₀⟩     Chance: 1/4 - 1/4·√½
</pre>

where in the left column, before the colon, you see the possible classical
measurements for classical bits 0 and 1, and on the right the exact chance.
In the middle you see the corresponding (collapsed) wave function of
the circuit that belongs to that reality.

## Installation

As usual, I only support linux. If you use something else then you're on your own;
although, I mostly use standard C++ - so porting shouldn't be hard for another
developer.

To install this project out-of-the-box you will need to have GNU
[autotools](https://en.wikipedia.org/wiki/GNU_Build_System_autotools) installed.
In order to see debug output, you need to have [libcwd](https://github.com/CarloWood/libcwd)
compiled and installed.

This project uses git submodules. To clone this project, have the above installed and simply run:

<pre>
<b>git clone --recursive https://github.com/CarloWood/quantum.git</b>
<b>cd quantum</b>
<b>./autogen.sh</b>
</pre>

After that it is the usual,

<pre>
<b>./configure</b>
<b>make</b>
</pre>

assuming you're familiar with GNU autotool projects.
