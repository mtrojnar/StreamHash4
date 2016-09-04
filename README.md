# Summary

Name: StreamHash4

Digest size: 512 bits

Performance: ~0.35 cpb on Intel Skylake

# StreamHash Family

The architecture of traditional hash functions is based on iterative
processing their state block and each input block with a compression function.
StreamHash functions use a vector of independently processed state values
instead of a single state block.  Each input block is mixed with each state
vector value, and then transformed with an nonlinear function (NLF).
Different NLFs are used for each value of the state vector.

Security of traditional hash functions are based on inability to control the
output of their compression functions.  Dissimilarly, security of the
StreamHash family is based on ability to only control one value of the state
vector at a time.  Replacing complex, multi-round compression functions with
simple, invertible, non-linear transformations results in material performance
improvement, while maintaining the required security properties.

# StreamHash4 Algorithm

StreamHash4 uses 4 streams, and the NLF based on the well-studied AES round:

           const₁  const₂  const₃  const₄
           (16 B)  (16 B)  (16 B)  (16 B)
              │       │       │       │
    input₁━━━┳┿━━━━━━┳┿━━━━━━┳┿━━━━━━┓│┏━━━128
    (16 B)   ▾▾      ▾▾      ▾▾      ▾▾▾
            [XOR]   [XOR]   [XOR]   [XOR]
              │       │       │       │
              ▾       ▾       ▾       ▾
           [NLF₁]  [NLF₂]  [NLF₃]  [NLF₄]
              │       │       │       │
    input₂━━━┳┿━━━━━━┳┿━━━━━━┳┿━━━━━━┓│┏━━━256
    (16 B)   ▾▾      ▾▾      ▾▾      ▾▾▾
            [XOR]   [XOR]   [XOR]   [XOR]
              │       │       │       │
              ▾       ▾       ▾       ▾
           [NLF₁]  [NLF₂]  [NLF₃]  [NLF₄]
              │       │       │       │
              ┊       ┊       ┊       ┊
              │       │       │       │
    inputₙ━━━┳┿━━━━━━┳┿━━━━━━┳┿━━━━━━┓│┏━━━total input bits
    (<=16 B) ▾▾      ▾▾      ▾▾      ▾▾▾
            [XOR]   [XOR]   [XOR]   [XOR]
              │       │       │       │
              ▾       ▾       ▾       ▾
           [NLF₁]  [NLF₂]  [NLF₃]  [NLF₄]
              │       │       │       │
              ▾       ▾       ▾       ▾
           (int₁)  (int₂)  (int₃)  (int₄)    (finalization)
              │       │       │       │
      int₁━━━┳┿━━━━━━┳┿━━━━━━┳┿━━━━━━┓│
             ▾▾      ▾▾      ▾▾      ▾▾
            [XOR]   [XOR]   [XOR]   [XOR]
              │       │       │       │
              ▾       ▾       ▾       ▾
           [NLF₁]  [NLF₂]  [NLF₃]  [NLF₄]
              │       │       │       │
      int₂━━━┳┿━━━━━━┳┿━━━━━━┳┿━━━━━━┓│
             ▾▾      ▾▾      ▾▾      ▾▾
            [XOR]   [XOR]   [XOR]   [XOR]
              │       │       │       │
              ▾       ▾       ▾       ▾
           [NLF₁]  [NLF₂]  [NLF₃]  [NLF₄]
              │       │       │       │
              ┊       ┊       ┊       ┊  (9 rounds skipped)
              │       │       │       │
      int₄━━━┳┿━━━━━━┳┿━━━━━━┳┿━━━━━━┓│
             ▾▾      ▾▾      ▾▾      ▾▾
            [XOR]   [XOR]   [XOR]   [XOR]
              │       │       │       │
              ▾       ▾       ▾       ▾
           [NLF₁]  [NLF₂]  [NLF₃]  [NLF₄]
              │       │       │       │
      int₁━━━┳┿━━━━━━┳┿━━━━━━┳┿━━━━━━┓│
             ▾▾      ▾▾      ▾▾      ▾▾
            [XOR]   [XOR]   [XOR]   [XOR]
              │       │       │       │
              ▾       ▾       ▾       ▾
                  (message digest)

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

