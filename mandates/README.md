# mandates: Chains That Only Narrow

Status: `draft` (decision 0003; the parts passports read, decision 0007).

## 1. Concept

A mandate empowers a holder within kinds of act, matters, an allowance per resource, a term and a slice per resource. A chain stands when its root can answer, each link was issued by its parent's holder (or is a split of its parent), each narrows its parent and none is revoked. Charged along the chain, every ceiling bounds the use beneath it (Proposition 2); a remote verifier lets at most min(B, σ) through after a revocation (Proposition 3).

For passports a mandate may also name the communities in which it may be used beyond its home, the digest of its holder's bearer credential, a term in instants beside its term in turns, and a reservation. Each is written into an envelope only when set, so older mandates keep their seals. A split is a part of a mandate issued by its own principal for its own holder, reserving its ceilings against the links above it; a charge beneath a reserving link is charged from that link down, so that a root's splits, one for each host, never pass the root (Proposition 12 of the theory note).

## 2. Content

`Mandate`, `Registry`, `chain`, `faults`, `root_principal`, `Envelopes` (with `reserve`), `control_faults`, `exposure`; for passports `is_instant`, `within_instants`, `names`, `split`, `charged_links`, `reserved_against`.

## 3. Checks

400 random chains, 200 random trees of envelopes, 2,000 cases of late revocation, the faults of control; the parts passports read, splits and reservations in `crossings/tests/check_passports.cpp`.
