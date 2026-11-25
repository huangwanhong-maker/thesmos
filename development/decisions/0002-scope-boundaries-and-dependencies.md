# 0002. Scope, Boundaries and Dependencies

- **Status:** Accepted, provisionally
- **Date:** 2026-11-25
- **Decided by:** the user ("new foundation for institutions", 2026-11-25), on the design record of the infrastructure, *A Society of Human and AI Communities* (`development/ideas/2026-11-25-a-society-of-human-and-ai-communities/`)

## 1. Context

The pattern "a sealed record of typed acts held to declared rules" had been written five times in the infrastructure: the law domain's proceedings, the allocation domain's procedure, kybernesis's record, the diplomacy domain's rules and mandates, and the studio host's journal. The design of a society of communities of persons and AI agents needs it once more, in general: acts of declared force, mandates whose chains only narrow, commitments, constitutions as data, decision rules with their unit of count, and the facets of a norm. Its subject has a literature of its own (speech acts, Hohfeld's relations, social commitments, electronic institutions, the grammar of institutions, Ostrom's rules).

## 2. Decision

thesmos (θεσμός, what is laid down, an institution) is a foundation of its own: institutions computed with.

- **In scope:** parties of declared kinds and their origins; mandates, their chains, envelopes and exposure; acts and their forces; commitments, set-off and novation; decision rules, units of count and sensitivity; constitutions as data and their admission by form; the chronicle and its fold; the facets of a norm; the schema of the ground as categories.
- **Out of scope:**

| Topic | Where it belongs |
|---|---|
| A particular community, its presets, its circulation of value, returns, federation and claims | the infrastructure's domain `communities` |
| Archives, fixity and provenance in general | archeion |
| Categories and their verification | kategoria |
| Logic, argumentation and the interpretation of open terms | apodeixis, tekmerion |
| Voting power, claims problems and allocation rules in general | nomos |
| Networks, files, models, pages | the infrastructure's API and works |

- **Dependencies:** archeion (the sealed journal), symploke (JSON read exactly), morphe (exact rationals; later its grammars and flows), apodeixis (verdicts), calx (the report of its checks), and kategoria in the checks of the schema. Commit order: after archeion and symploke, before kybernesis.

## 3. Alternatives Considered

- **Modules inside kybernesis,** which plans a `constitution` module. Set aside at the user's choice: steering by evidence and the constitution of institutions are different subjects, and kybernesis may later declare thesmos.
- **A domain.** Rejected: nothing here names an object in nature; every science of society needs it.

## 4. Consequences

- The general part of the diplomacy domain's `rules` (thresholds as exact fractions, quorums, consensus) can move here, and the domain then use it; to be proposed in its own change.
- The repository is the user's to create; until then thesmos lives in the infrastructure's working tree.
