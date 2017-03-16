# Summary

Name: StreamHash4

Digest size: 512 bits

Performance: ~0.35 cpb on Intel Skylake

# StreamHash Family

The architecture of traditional hash functions, also known as [Merkle–Damgård
construction](https://en.wikipedia.org/wiki/Merkle%E2%80%93Damg%C3%A5rd_construction),
is based on iterative processing their state block and successive input blocks
with a one-way compression function.  The StreamHash family of hash functions
does not use a compression function.  Instead of a single state block, they use
a vector of independently processed state values.  Each input block is mixed
with each state vector value, and then transformed with an nonlinear function
(NLF).  A different NLF is used to transform each state vector value.

Security of the Merkle–Damgård construction is based on the attacker being
unable to control the output of the compression function.  Dissimilarly,
security of the StreamHash family is based the attacker being able to only
control one value of the state vector at a time.  Replacement of the complex,
multi-round compression function with a simple, invertible, non-linear
transformation results in material performance improvement, while maintaining
the required security properties.

# StreamHash4 Algorithm

StreamHash4 uses 4 streams, and the NLF based on the well-studied AES round:

![StreamHash4 Computation Flow](/docs/streamhash4_flow.png)

, where:
    NLFₙ(x) = aesenc(x, constₙ)

The initial digits of the π number are used as the const₁…const₄ values.

# Security

## Preimage Resistance

Computing preimage is presumed to be roughly equivalent to breaking 13 rounds of AES.

## Second Preimage Resistance

There are currently no known second preimage attacks easier than the generic bruteforce attack.

## Collision Resistance

There are currently no known collision attacks easier than the generic birthday attack.

