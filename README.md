# thesmos

*Institutions computed with.* thesmos (θεσμός, what is laid down: an institution) holds what every science of institutions needs: parties of declared kinds and their origins, mandates whose chains only narrow, acts of declared force, commitments and their life, constitutions written as data and admitted by form, decision rules with their unit of count, the chronicle that seals a community's acts and the fold that computes its relational state, and the facets of a norm.

It is one of the foundation libraries of [Noetarium, the Science and Humanity Infrastructure](https://github.com/huangwanhong-maker/Noetarium-The-Science-and-Humanity-Infrastructure), and a project in its own right. Status: `draft`, founded on 2026-11-25.

## 1. Concepts

- **The ground.** A community's acts each carry one of five forces (Searle's classes of illocution), a typed envelope that the fold reads, and a free body that persons and models read. Duties and powers follow from envelopes alone, so an instruction carried in a body changes nothing.
- **The chronicle.** The acts a community admitted, sealed in archeion's hash-chained journal. Positions, mandates, commitments, grants, claims and rules in force are computed from it. A refused act changes nothing; a replay gives the same state; an altered act is found.
- **Answerability.** An operative (an identity under which a program acts) acts only under a chain of mandates whose root principal is a person, an organisation or a community; each link narrows the one above; every charge is made against every link up to the root.
- **Constitutions as data.** A community's order in Ostrom's seven kinds of rules, with its bodies, scenes, conversions, route for claims, custody of means and rules of amendment, admitted by nine conditions of form. The fold reads the written form itself.
- **Counts by origin.** A decision declares its unit of count; by origin, the operatives of one controller count once, and a decision that turns on a declared relation of control is reported as turning.

The theory note is [docs/theory.md](docs/theory.md).

## 2. Modules

| Module | Status | Content |
|---|---|---|
| [kernel](kernel/README.md) | draft | parties, kinds, control and provision, origins |
| [mandates](mandates/README.md) | draft | chains, faults, envelopes, the faults of control, exposure after revocation |
| [acts](acts/README.md) | draft | forces, the ground's kinds, envelopes and bodies |
| [commitments](commitments/README.md) | draft | the life of a commitment; set-off and novation |
| [procedures](procedures/README.md) | draft | units of count, exact shares, rules, sensitivity |
| [constitutions](constitutions/README.md) | draft | the written form, admission, recognised against executable powers |
| [chronicle](chronicle/README.md) | draft | the fold, seals, replay, the relational digest; the schema of the ground |
| [norms](norms/README.md) | draft | the four facets of a norm and their marks |
| institutional grammar | planned | institutional statements read by morphe's BNF |
| crossings | planned | acts sent between chronicles, with receipts on both sides |

## 3. Building and Checking

Inside the infrastructure, from its root: `node development/tools/run-cpp.mjs foundations/thesmos thesmos`. thesmos uses the sibling libraries archeion, symploke, morphe, apodeixis, calx and, in its checks, kategoria (decision [0002](development/decisions/0002-scope-boundaries-and-dependencies.md)).

## 4. Licence

MIT, in [LICENSE](LICENSE); contributors and their terms in [CONTRIBUTORS.txt](CONTRIBUTORS.txt).
