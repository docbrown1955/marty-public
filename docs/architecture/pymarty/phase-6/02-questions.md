Status: ANSWERED
Updated: 2026-11-24

# Phase 6 questions and answers (step 2)

| # | Question | Answer (user, 2026-11-24) | Effect on spec |
|---|----------|---------------------------|----------------|
| Q1 | Binding mechanism: (a) ctypes on generated C layer, (b) pybind11 per library, (c) dlopen in `_pymarty` | (a) ctypes | Generated `.so` is the C ABI; Python loads it with ctypes. No new compiled module per library. |
| Q2 | Port scope for the 5 system tests: selected points or full scans | Full scans | Zmumu, gg_to_tt, EWGauge reproduce the full reference files. Marked `slow`. |
| Q3 | EWGauge quad precision: defer to phase 7 or support now | Defer | EWGauge port is excluded from phase 6 and listed as phase 7 work. |
| Q4 | Keep out-of-process fallback backend in phase 6 | No (user asked for an explanation first, then answered no) | Not built. Documented as an option if the spike fails. |

Prerequisite: phase 5 accepted by the user on 2026-11-24 (session decision "accept_and_start").

Open items carried into the spec: whether 1e-12 is reachable with generated `double` precision (spike measures it),
and whether in-process `dlopen` is safe with LoopTools and GSL (spike).
