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
           (int₁)  (int₂)  (int₃)  (int₄)

           const₁  const₂  const₃  const₄    (finalization)
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
