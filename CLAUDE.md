# CLAUDE.md

This file guides Claude Code in the thesmos repository.

thesmos is one of the foundation libraries of [Noetarium, the Science and Humanity Infrastructure](https://github.com/huangwanhong-maker/Noetarium-The-Science-and-Humanity-Infrastructure), which includes it as the git submodule `foundations/thesmos` once its repository exists. It is a project in its own right and must make sense without the infrastructure.

## 1. Purpose and Scope

thesmos (θεσμός, what is laid down, an institution) is institutions computed with:

- parties of declared kinds (person, organisation, operative, community), the declared relations of control and provision, and origins under a criterion;
- mandates: chains that only narrow, envelopes charged along the chain, slices, revocation and its exposure;
- acts: five forces, the ground's kinds, a typed envelope and a free body;
- commitments and their life, after Singh; obligations of one class cleared by set-off and shortened by novation;
- decision rules with their unit of count, exact shares and sensitivity to the partition of origins;
- constitutions as data in Ostrom's seven kinds of rules, read exactly and admitted by nine conditions of form;
- the chronicle: a community's acts sealed in archeion's journal, the fold that gives its relational state, replay;
- the four facets of a norm (regularity, adoption, operation, justification);
- crossings: acts that pass between chronicles with receipts, heads witnessed by notaries, exit with the record (decision 0004);
- institutional statements: rules in words read by a grammar in BNF on morphe, a constitution's rules as statements, measured as norms (decision 0005);
- later: constitutive statements; entrenchment by level; signatures.

It builds on these sibling libraries:

- **archeion**, for the sealed journal;
- **symploke**, for JSON read exactly;
- **morphe**, for exact rationals and the BNF reader of statements (later its flows);
- **apodeixis**, for verdicts;
- **calx**, for the report of its checks;
- **kategoria**, in the checks of the schema of the ground.

Out of scope:

| Topic | Where it belongs |
|---|---|
| A particular community, its presets, circulation of value, returns, federation and claims | the infrastructure's domain `communities` |
| Archives, categories, logic and interpretation in general | archeion, kategoria, apodeixis, tekmerion |
| Voting power, claims problems, allocation rules in general | nomos |
| Files, networks, models, pages | the infrastructure's API and works |

The boundary is [decision 0002](development/decisions/0002-scope-boundaries-and-dependencies.md).

## 2. Invariants

1. **Dependencies.** thesmos depends only on archeion, symploke, morphe, apodeixis, calx and kategoria among project libraries, and never on the infrastructure's domains, edges or works.
2. **Envelopes, never bodies.** The fold reads an act's typed envelope and never its body.
3. **Answerability.** Every act is answered for by a person, an organisation or a community; an operative acts only under a chain that ends at one.
4. **Rules as data.** A constitution is data; every conversion between registers is an act under a declared rule; an amendment that leaves the constitution inadmissible is refused.
5. **Refused acts change nothing.** A refused act leaves the state and the journal as they were, and says why.
6. **Pure.** No I/O beyond strings a caller has read, no global state, no network.
7. **No particular objects.** Its examples are invented and say so.
8. **Reuse and citation.** What a sibling holds is used through its public headers; every rule cites its source.

## 3. Design Philosophy

thesmos shares its governing principle with its sibling repositories: **modular, extensible, revisable**, and their status vocabulary (`planned`, `seed`, `draft`, `stable`, `deprecated`).

## 4. Repository Layout

```
thesmos/
├── CMakeLists.txt   one header-only target thesmos::<module> per module (thesmos_module); the siblings added once
├── kernel/ · mandates/ · acts/ · commitments/ · procedures/ · constitutions/ · chronicle/ · norms/ · crossings/ · statements/
├── schema/          the schema of the ground stated as categories (ground.cat), checked by kategoria
├── examples/        an illustrative constitution (workshop.json), invented
├── docs/            the theory note
└── development/     decisions · ideas · release notes
```

Each module directory holds `README.md`, `include/thesmos/<module>/` and `tests/check_*.cpp`, told where the schema and examples are kept (`THESMOS_DATA`). The modules and their order are in [decision 0003](development/decisions/0003-the-first-modules.md).

## 5. Working Procedures

- Decisions are numbered records in `development/decisions/`, never rewritten; ideas are `development/ideas/YYYY-MM-DD-slug.md`.
- **Release notes.** At each commit, add to `development/release-notes/YYYY-MM-DD.md` a concise summary in plain prose of what is new; not between commits.
- Before restructuring or changing an invariant (§2), propose the change and wait for the user's agreement. Keep each change to one concept.
- Writing style, status vocabulary, and decision, idea and release-note conventions are shared with every repository of the infrastructure.
- Inside the infrastructure, commit in thesmos first, then the pointer; siblings in dependency order: calx and morphe, kategoria, apodeixis, archeion, symploke, then thesmos.

## 6. Conventions

- **Turns** count a community's time; an act without an instant is placed by its turn. Instants are ISO 8601 in UTC.
- **Kinds of act** are the ground's (`acts::ground()`), written as the written form writes them ("hand over").
- **Labels** are formal nouns; nothing presupposes a discipline.
- **Writing style.** Declarative numbered headings, never questions; concepts before structure; *Rationale.*, *Remark.*, *Caveat.*; British spelling.

## 7. Technology

C++20, header-only targets `thesmos::<module>`, CMake 3.20 or later, checks on calx's `verification`; GCC in WSL on the development machine. MIT-licensed.

## 8. Current State

As of 2026-11-25, eight modules are written and checked ([decision 0003](development/decisions/0003-the-first-modules.md)), with the schema of the ground answered by kategoria. On 2026-11-26 `crossings` was added, votes are counted for the party that answers for them and a party to a claim is recused ([decision 0004](development/decisions/0004-crossings-and-votes-borne.md)); then `statements` ([decision 0005](development/decisions/0005-institutional-statements.md)); 86 checks. The repository is published at `https://github.com/huangwanhong-maker/thesmos`.
