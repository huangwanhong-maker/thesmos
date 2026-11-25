# 0003. The First Modules

- **Status:** Accepted, provisionally
- **Date:** 2026-11-25
- **Decided by:** Claude, on the design record's implementation (its Section 1.2), at the user's word to begin the implementation

## 1. Context

The design record states its definitions as an executable sketch in Python (152 statements). Its implementation is C++ in the infrastructure, held to the same checks. An independent reading for error found seventeen problems in the design on the day of the founding; those that bear on code are corrected here.

## 2. Decision

Eight header-only modules, each a target `thesmos::<module>`, in this order:

| Module | Content |
|---|---|
| `kernel` | Parties of declared kinds; control and provision; origins under a criterion |
| `mandates` | Chains that only narrow and their faults; envelopes charged along the chain; the faults of a declared relation of control; exposure after a revocation |
| `acts` | The five forces, the ground's kinds, the act's typed envelope and free body, its canonical text |
| `commitments` | The life of a commitment; obligations of one class, set-off and novation |
| `procedures` | Units of count (person, origin, operative, community), exact shares, rules, sensitivity to the partition of origins |
| `constitutions` | The written form read exactly; admission by nine conditions of form; what the fold reads; recognised against executable powers |
| `chronicle` | The fold on the constitution's own rules, bodies, units and tempi; seals by archeion; replay; the relational digest |
| `norms` | The four facets of a norm read from a chronicle, and their two marks |

The schema of the ground is stated as categories in `schema/ground.cat` and answered by kategoria; an illustrative constitution is kept in `examples/`.

Corrections carried from the reading for error:

- Proposition 6 is restated: an origin weighs one whatever its voters; copies change nothing when its voters vote alike, and move the share by at most 1/O when they divide.
- The slice is per resource, in that resource's unit, and enforced; a charge is made only under the act's own mandate.
- Persons and communities are units of count of their own.
- Control is a function: joint control is declared as control by one party, and cycles of control are faults.
- An amendment is refused when it would leave the constitution inadmissible; conversions are declared only by amendments, which are acts.

## 3. Alternatives Considered

- **Copying the sketch's simplified constitution into the fold.** Rejected: the fold reads the written form itself, so that a preset is both admitted and run.
- **Reading prose norms.** Deferred: norms here are predicates over the chronicle; reading them from prose is interpretation, apodeixis's and tekmerion's.

## 4. Consequences

- Entrenchment by level, preserved properties stated as checkable predicates, crossing acts with receipts between two chronicles, and the institutional grammar read by morphe's BNF are the next modules' work.
