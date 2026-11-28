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
- holdings and the norms of access: what a community holds, with custodian and provenance; norms in the grammar of institutions resolved by declared specificity; visibility (public, members, offices) and transmission outside the community; access admitted by the fold and charged along chains; the briefing an entering party reads from the record and the profile an outsider reads (decision 0006);
- passports: mandates that name the communities where they may be used, their holder's credential by digest, a term in instants and a reservation; splits of a mandate for each host, reserving their ceilings so that the hosts together never pass the whole; the passport a home issues from its record (every link with its proof of inclusion, notaries' witnesses of the head), verified anywhere from the document, written as `noetarium.passport/1` (decision 0007), the document in its own module `passports` beneath the chronicle (decision 0009);
- presentation and recognition: parties declared by acts (a founding its founders, a presentation its agent and principal), so that a record replays from an empty registry; `recognise`, a community recognising another at a level (exists, evidence, undertakings, decisions), generally or for a matter, by an office the choice rules empower; a constitution's part `passports` (from whom it accepts passports, what it requires of their holders, its office of status), always public; the office of status's reports of other communities' heads and of foreign mandates' status; `present`, a passport verified by the host's fold against the heads its own record knows, its holder and principal declared holding no position, its links registered as mandates of another community; an act under a foreign link refused when the link stands revoked or suspended as last reported, its term read in instants, and what is charged under it between two reports at most its slice (decision 0009);
- a taxonomy of behaviour: five categories (normative act, conduct, event, state, observation) in the registers of the Generative Relational framework; normative acts classed by the source of their effect, every effect composed of effect primitives, a few produced by law; the base taxonomy as data (`schema/behaviour.json`); declared kinds by counts-as, categories of conduct in a closed language on morphe's BNF, counts-as rules to a status a norm reads, trigger rules limited to effects by law, standards of recognition; revisions admitted as functors, on kategoria's ontologies; the behaviour journal joined to the chronicle by the field `episode` (decision 0008);
- levels of description and epistemic behaviour by agreement: the base taxonomy's second version (`schema/behaviour-2.json`), a revision of the first along which records carry, describing one occurrence at five levels (E epistemic behaviour, O ontological conduct, M manifest conduct, S social conduct indexed by its structure, N normative conduct indexed by its order, in constituted acts and qualified conduct), with facets, nine bridges and practices; descriptions of occurrences at several levels at once; invariant G10; disclosure agreements (`disclosure`: terms `noetarium.disclosure/1`), ratified by each side under its own rule, their records held as holdings of kind `epistemic record` under norms of access, denounced after notice and then kept, returned or erased; epistemic episodes recorded in the behaviour journal only under an agreement that covers them, as their digest, and outside one only as the fact that reasoning took place (decision 0010);
- items, routes and rulings: a request that names a type opens an item on its route (applications, questions, petitions, or a type a community declares; `apply`, `question` and `petition` declared in the base taxonomy's third version, `schema/behaviour-3.json`), answered by the route's body or office within its term, never by a party to it; applications decided by the position's boundary rule (open, on application by rule, sponsored, after a conversion, by a body, assigned by an office, by lot from a committed seed), assignments ended by the office, `leave` with `all`; questions ruled into a register of rulings kept as precedent, a second question on a ruled subject refused with the ruling cited, reviews and overrulings only from the review body; petitions met by a checked deposit, adopted through a proposal, or carried; overdue marks; the part `routes`, always public, and invariant G8′ for constitutions that declare it (decision 0011);
- posted work (`work`): kinds of work whose life a constitution declares as data (who posts, takes by commitment or award, eligibility, terms, a deliverable's check, review by another origin, another provider or a position with verdicts, acceptance by an office or a decision, redelivery after a return, what comes first); the life of an item read from its record and the turn, lapse and reopening effects by law, an item living under the life it was posted under; the base taxonomy's fourth version (`schema/behaviour-4.json`) with items an effect primitive; holdings reviewed by another provider with verdicts that check; conditions on application that read posted work; the briefing's and profile's view of posted work (decision 0012);
- transmission, counts and compacts: the base taxonomy's fifth version (`schema/behaviour-5.json`), whose counts-as rule makes an operative's reading a transmission to its provider where a constitution declares that operatives transmit to their provider, counts-as rules applied under a declaration; counts by provider and by declared weight, thresholds at exact fractions and double majorities of members and weight; compacts (`compacts`: terms `noetarium.compact/1`), ratified, reserved and withdrawn by each party's own amendment, their standing read across the parties' records with each party kept apart, the disclosure agreement their case of two sides (decision 0013);
- orders, terms and seats: a community's order, a label its profile shows beside its name and registries read, which neither admission nor the fold reads; the registries it says it is listed in and its envoys as objects, data of its part `external`; a position's term, read as the trigger rule that ends a position held for more than its turns; a body's seats, the fold refusing every act that would give the body more members; terms and seats amended by `change.term` and `change.seats`; a step that settles without copying the state (decision 0014);
- later: constitutive statements; entrenchment by level; signatures.

It builds on these sibling libraries:

- **archeion**, for the sealed journal;
- **symploke**, for JSON read exactly;
- **morphe**, for exact rationals and the BNF reader of statements (later its flows);
- **apodeixis**, for verdicts;
- **calx**, for the report of its checks;
- **kategoria**, in the checks of the schema of the ground, and for its `ontologies`, on which the taxonomy of behaviour is admitted and revised (decision 0008).

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
9. **Effects by act or by rule (G9).** No institutional effect arises except by the admission of a normative act or by a declared trigger rule over the record and its turns. The fold reads no journal of conduct: conduct reaches the institutional state only through an act it constitutes or an act of evidence that records it (decision 0008).
10. **Freedom of thought (G10).** No norm attaches an effect to epistemic behaviour alone: no counts-as rule and no declared condition may read only features of level E, and epistemic behaviour enters the record only by disclosure under an agreement, its use as evidence only for a purpose the agreement admits (decision 0010).
11. **A route for every directive (G8′).** A constitution that declares its routes declares one for applications, questions, petitions and every type of item its declared kinds open, each possibly empty, and admission refuses it otherwise; one that declares none reads its routes from what it declares, so that every older record replays. No party to an item answers or decides it (decision 0011).

## 3. Design Philosophy

thesmos shares its governing principle with its sibling repositories: **modular, extensible, revisable**, and their status vocabulary (`planned`, `seed`, `draft`, `stable`, `deprecated`).

## 4. Repository Layout

```
thesmos/
├── CMakeLists.txt   one header-only target thesmos::<module> per module (thesmos_module); the siblings added once
├── kernel/ · mandates/ · acts/ · behaviour/ · commitments/ · procedures/ · holdings/ · work/ · compacts/ · disclosure/ · passports/ · constitutions/ · chronicle/ · norms/ · crossings/ · statements/
├── schema/          the schema of the ground stated as categories (ground.cat), checked by kategoria; the base taxonomy of behaviour, its first version (behaviour.json), its second (behaviour-2.json), its third (behaviour-3.json), its fourth (behaviour-4.json) and its fifth, in force (behaviour-5.json)
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

As of 2026-11-25, eight modules are written and checked ([decision 0003](development/decisions/0003-the-first-modules.md)), with the schema of the ground answered by kategoria. On 2026-11-26 `crossings` was added, votes are counted for the party that answers for them and a party to a claim is recused ([decision 0004](development/decisions/0004-crossings-and-votes-borne.md)); then `statements` ([decision 0005](development/decisions/0005-institutional-statements.md)); 86 checks. On 2026-11-27 `holdings`, the norms of access with visibility and transmission, access in the fold, the briefing and the profile ([decision 0006](development/decisions/0006-holdings-and-the-norms-of-access.md)); 133 checks. The same day passports: the parts of a mandate they read, splits and reservations, and passports issued, witnessed and verified ([decision 0007](development/decisions/0007-passports.md)); 158 checks. Then `behaviour`, the taxonomy of behaviour with its declared forms of extension, the base taxonomy as data on kategoria's ontologies, the behaviour journal joined to the chronicle, and the invariant G9 ([decision 0008](development/decisions/0008-a-taxonomy-of-behaviour.md)); 192 checks. Then presentation and recognition: the passport as a document in its own module beneath the chronicle, parties declared by acts, `recognise` in levels, the office of status and its reports, `present` verified against the host's own record, foreign links with their status and window ([decision 0009](development/decisions/0009-presentation-and-recognition.md)); 211 checks. Then the taxonomy's second version in levels of description, a revision of the first, with facets, bridges, practices and descriptions of occurrences at several levels, invariant G10, and disclosure agreements under which epistemic behaviour alone is recorded ([decision 0010](development/decisions/0010-levels-of-description-and-epistemic-behaviour-by-agreement.md)); 239 checks. Then items, routes and rulings: typed requests opening items on routes, `apply`, `question` and `petition` in the base's third version, applications decided by the boundary rule, rulings as precedent, petitions, recusal, and G8′ for constitutions that declare their routes ([decision 0011](development/decisions/0011-items-routes-and-rulings.md)); 266 checks. Then posted work: `work`, kinds of work as data and the life of an item read from its record, the acts of that life in the fold, review by another provider of work and of holdings, a kind's life amended, the base's fourth version ([decision 0012](development/decisions/0012-posted-work.md)); 292 checks. Then transmission, counts and compacts: an operative's reading counted as a transmission to its provider where a constitution declares it, in the base's fifth version; counts by provider and by weight, thresholds at fractions and double majorities; `compacts`, ratified, reserved and withdrawn by amendment and read across the parties' records ([decision 0013](development/decisions/0013-transmission-counts-and-compacts.md)); 330 checks; this completes the thesmos phases of the design of 2026-11-27. Then, for the infrastructure's Phase 8, orders, terms and seats: the order a label the profile shows and the fold never reads, the registries and envoys of the part `external` data, a position's term as its trigger rule, a body's seats read by the fold, and a step that settles without copying the state ([decision 0014](development/decisions/0014-orders-terms-and-seats.md)); 345 checks. The repository is published at `https://github.com/huangwanhong-maker/thesmos`.
