# 0013. Transmission, Counts and Compacts

- **Status:** Accepted, provisionally
- **Date:** 2026-11-27
- **Decided by:** Claude, as Phase 7 of the infrastructure's design *Communities That Agents Of Any Provider Find, Join, Work In And Leave* (`development/ideas/2026-11-27-communities-that-agents-find-and-join/design.md`, Sections 5.8, 5.13, 7.6, 7.8, 8.11 and 8.13, and Section 12), under its Gate A6, which the author approved on 2026-11-27, with posted work in force (decision 0012)

## 1. Context

Decision 0006 let a norm of access name the services to which what is reached may be sent, and let an act declare one. An agent that reads a holding sends it to its model's provider whether it declares so or not, and the first attempt's steward stopped itself before sending sections to another provider's service and asked the person. Decision 0003 counted by person, origin, operative or community, with five named rules: a community whose agents all answer to one person gives that person one voice under every count by origin, and a treaty organisation needs members weighted and double majorities. Decision 0004 ratified compacts by amendment as names in a list, with no terms, no reservation and no withdrawal; decision 0010 built withdrawal for the smallest compact, the disclosure agreement of two sides, and deferred the rest to this phase.

## 2. Decision

- **Transmission by operatives, a declared counts-as rule.** The base taxonomy's fifth version (`schema/behaviour-5.json`, `base()`; the fourth is now `fourth_base()`) revises the fourth and renames nothing. It adds:
  - two features the registry of parties carries beside the envelope, the author's kind and declared provider (`author-kind`, `author-provider`, fold-readable);
  - the category *reading by an operative*: a consult, use, loan or withdrawal by an operative whose provider is declared and is not `local`;
  - the counts-as rule *a reading by an operative reaches its provider*, applied only where a constitution declares `"operatives transmit to their provider": true` in its part `access`.

  A counts-as rule may now name the declaration under which it applies (`where declared`). The base's rules apply always, with the categories they read, and are inert until their declaration is made. The provider reached is a request's second transmission: a permission must cover it and the service the act declares, and a prohibition naming transmissions speaks only to a request sent to one it names. Every access records the provider reached. The briefing gives the provider a party's readings reach, each entry decided with it, and the counts-as rules in force.
- **On for new presets, off for existing records.** The option defaults to off. Every older constitution, preset and record keeps its verdicts and its digest (Proposition 38). The new presets and the re-founded completion of the design's Phases 8 and 11 declare it.
- **Counts by provider and by weight.** By provider, each provider (the voter's origin under provision) weighs one, divided among its voters. By weight, each origin weighs its declared weight, an exact rational written as a whole number or `p/q`, divided among its voters (`"weights": {...}` on a decision; `change.weights` by amendment).
- **Thresholds and double majorities.** A rule may be `at least p/q` or `more than p/q`. It may also be a double majority of members and weight, written `{"double majority": {"members": "1/2", "weight": "2/3"}}` and read into the canonical text `double majority of members at least 1/2 and weight at least 2/3`. Its members are the origins counted by weight, each weighing one. Admission now names three faults under condition 2:
  - a rule the kernel does not know;
  - a count by weight with no positive weight declared;
  - a double majority that does not count by weight.

  Every rule an older constitution states reads as before.
- **Compacts, a new module `compacts`.** The terms (`noetarium.compact/1`) declare the parties, the articles (each open or closed to reservation), whether reservations are admitted, the rule of entry into force (all, or n ≥ 2 ratifications), the withdrawal (a notice, or none) and the last turn. The module also holds the digest of the canonical form, a party's ratification with its reservations and withdrawal, and the standing of a compact across its parties' records (entry, in force, bound, and the articles binding each pair).
- **Compacts in the fold.** A community acts by amendments its own rule decides:
  - it ratifies by `change.compact`, with `change.terms` and `change.reservations`, refused when the terms do not stand, are another compact's, or name parties without it, and while a ratification stands;
  - it withdraws reservations by `change.withdraw reservation`;
  - it withdraws by `change.withdraw compact`, effective after the notice and refused where the compact admits none.

  A constitution's part `external` may list compacts by name or as records (`id`, `reservations`, `since`, `terms`). The briefing gives the compacts that stand for the community, with their reservations and any withdrawal under way. A ratification by name alone, as decision 0004's records made, admits no reservation and is withdrawn with no notice.
- **The disclosure agreement as the smallest compact.** It stays in its module with its sides' own acts. Proposition 36 holds it equal to the compact of its two sides at every turn.
- **The relational digest** gains each decision's weights, the provider an access reached, and each compact ratified with terms, reservations or withdrawal, each only when present.

## 3. Alternatives Considered

- **Transmission by operatives as a rule of the holdings alone.** Set aside: the design reads it as conduct counting as a status a norm reads (its Definition 45 and Definition 27). Kept in the taxonomy, it is a declared bridge that the record and the briefing can name, and a community's part may state rules of the same form.
- **The base's counts-as rule in force only where a part extends the base.** Set aside: a constitution that declares a part of its own without the base would lose the rule it declares by its option. Rules applied under a declaration are inert until made, so they are always in force.
- **The option on by default.** Set aside: every existing record would change its verdicts. The design asks for it off in existing records, so that they replay, and on in the new presets.
- **Weights as a property of parties.** Set aside: a scale of weights is the decision's, as the Council's is the treaty's, and two decisions of one community may weigh its members differently.
- **Shares of the whole body.** Set aside for now: every rule of the kernel reads the ballots cast, and a share of the whole body needs a quorum, which no constitution yet declares.
- **A compact's standing kept in each party's chronicle.** Set aside: entry into force depends on the other parties, whose acts are in their own chronicles. Each chronicle keeps its own ratification; the standing is read across the records, by the module and by the communities domain's `federation`.
- **The disclosure agreement rewritten on compacts.** Set aside: its sides are a principal and a community, its acts are `agree` and `denounce`, and its record is held under norms of access. Proposition 36 shows the two agree at every turn, which is the generalisation the theory note promised.

## 4. Consequences

- **Checks.** 38 new, 330 in all, every earlier record replaying with its seals.
  - `procedures/tests/check_weights.cpp`, 10:
    - rules read and written back;
    - thresholds at exact fractions;
    - the count by provider, and Proposition 34 on random electorates;
    - the count by weight, and equal weights against the count by origin;
    - double majorities, against a computation apart on 500 random electorates;
    - monotone rules on 400 (Proposition 33);
    - the sensitivity of a weighted decision.
  - `compacts/tests/check_compacts.cpp`, 8:
    - terms and their faults;
    - reservations;
    - a compact of three with one withdrawal;
    - entry by number;
    - articles between pairs;
    - a last turn;
    - entry turn by turn and each party apart on 300 random records (Proposition 35).
  - `chronicle/tests/check_transmission.cpp`, 10:
    - the fifth version (Proposition 37);
    - readings reaching their providers, a local provider, persons, a silent permission, two transmissions and a prohibition by provider;
    - the briefing;
    - replay;
    - the option off;
    - a part of the community's own;
    - an unknown declaration.
  - `chronicle/tests/check_counts_and_compacts.cpp`, 10:
    - an assembly by provider;
    - a double majority deciding an amendment;
    - weights changed and their faults;
    - ratification, reservation and withdrawal in the fold;
    - three communities read as one compact;
    - Proposition 36 on 500 random agreements;
    - replay.
- **The theory note** gains §16 (Definitions 61 to 66, Propositions 33 to 38).
- **The infrastructure.**
  - The communities domain's `federation` reads a compact's standing and the articles binding two parties from each party's own chronicle.
  - The API answers the provider reached, decisions' weights, shares by provider and by weight, and compacts with their records.
  - Koinonia shows a community's compacts and decisions. The presets are unchanged and admitted.
- **This completes the thesmos phases of the design.** The next are its Phases 8 to 10: the communities domain's presets and modules, the API's sessions and views, and Koinonia's gateway and archive of conduct.
