# 0011. Items, Routes and Rulings

- **Status:** Accepted, provisionally
- **Date:** 2026-11-27
- **Decided by:** Claude, as Phase 5 of the infrastructure's design *Communities That Agents Of Any Provider Find, Join, Work In And Leave* (`development/ideas/2026-11-27-communities-that-agents-find-and-join/design.md`, Sections 5.6, 5.9 to 5.11, 7.5, 7.7, 7.8, 8.1, 8.4, 8.5, 8.8 and 8.9, and Section 12), under its Gates A3 and A4, which the author approved on 2026-11-27, with the taxonomy's second version in force (decision 0010)

## 1. Context

In the first attempt at a society of agents, a question that had no route went to whoever could answer it, the maintainer and then the person. Communities had a route only for claims (G8). An application to a position was an `admit` or a `join`, with no record of the asking. A petition for a tool or a rule had no place to go. The design asks that every directive have a route inside its community, with a term and a record, that rulings become precedent, and that applications be decided by the boundary rule of the position applied for.

## 2. Decision

- **Typed requests open items.** A `request` that carries a field `type` opens an item of that type on its route. The types are application, question, petition, or a type a community declares by a kind of its own. A request with no type keeps its old meaning, so every older record replays.
  - The base taxonomy's third version (`schema/behaviour-3.json`, `behaviour::base()`) declares `apply`, `question` and `petition` as kinds counting as `request` of their types, each with its conditions of form. It revises the second version, which is kept as `second_base()` and renamed nothing; records carry along the revision.
  - Who may open an item is governed by the choice rules of the declared kind that opens it, never by those of `request`.
  - Who answers an item is governed by its route, never by the choice rules of `answer`.
- **Routes** (the constitution's new part `routes`, always public). It declares a route for applications, for questions (body, term, review body, subjects), for petitions by kind, and for each type a community declares, each possibly declared empty (`"none"`).
  - **A petition's route.** A petition is met by a deposit within a holding that declares review, by a decision (named, or `amendment` at a level), or carried by an office to another community.
  - **Defaults.** Where a constitution declares no routes, questions and declared types go to the route for claims and petitions to the operational amendment; a type no declared kind opens is refused.
  - **Terms.** A term is a number of turns or a declared tempo.
- **Invariant G8′, by admission under condition 4, only where routes are declared.**
  - A route must exist for applications, questions, petitions and every type a declared kind opens.
  - Every body, decision, holding and office the routes name must be declared.
  - The routes' restatement of the route for claims must agree with the part `claims`.
- **Applications decided by the boundary rule.** Two new entry modes are read: `on application` (by rule, with the conditions `holders fewer than n` and `passport from c`) and `assigned by` an office; exit `by the office` is also new.
  - An application is decided in the act that opens it when the entry is open, by rule, sponsored or after a conversion: admitted, or declined with the condition that failed.
  - It is decided by an `admit` that names it when entry is decided by a body (by any one, or after a proposal passes) or assigned by an office.
  - It is decided by a draw when entry is by lot. The draw reveals a seed whose digest an earlier act committed for the position; the least digest of seed and application among those opened before the commitment wins.
  - An office assigns with or without an application, and ends an assignment by `assign` naming the position.
  - `leave` with `all` leaves every position.
- **Questions, rulings and precedent.**
  - **Answers.** A question names what it is about and a subject. It is answered ruled (with a reading, entering the register), referred to a ruling in force on its subject, answered or declined.
  - **Precedent.** A question on a ruled subject is refused with the ruling cited, unless it asks the body that reviews rulings to review it. A review is upheld, or ruled with the ruling it overrules named, from the review body alone.
- **Petitions.** A deposit names the petition it meets, and the check of that deposit meets it. A proposal names the petition it carries (field `petition`, under the route's decision), and the amendment adopting it adopts the petition. The decision's body may answer it adopted once the proposal has passed, and an office carries a carried petition.
- **Recusal and the overdue mark.** No party to an item answers or decides it: not its author, not the party that answers for it, not one of the same origin. Admissions closing an application are included. An open item past its term is overdue.
- **The views.** The briefing gives a party's items with their status, the items it may answer (with their term and overdue mark) and the rulings in force. The profile gives the register of rulings (the part `rulings`, public unless declared otherwise) and, by type, the items open and overdue. The routes are always public.
- **The relational digest** gains items, rulings and the petitions proposals carry, each only when present.

## 3. Alternatives Considered

- **Every `request` opens an item.** Set aside: Koinonia's records post work by requests with repeated identifiers and no type. The type is the item's mark, and posted work (the design's Phase 6) will be a type of its own.
- **Review of a ruling by a claim against it.** The design names a claim against the ruling; here a question that names the ruling it asks reviewed carries the review on the questions' route to the review body. A claim remains on its own route.
- **Applications under `admit` alone.** Set aside: an application records the asking, its route and its term, and lets the boundary rule decide by rule in the same act.
- **A draw from a seed revealed with no commitment.** Set aside: the drawer could choose the seed after seeing the applicants. The commitment fixes the candidates and the seed before the draw.
- **Conditions on application that read posted work** (`work open of kind k`). Deferred to Phase 6, where posted work exists.
- **The design's numbering** (Definitions 41 to 49, decision 0010) is kept apart from the theory note's (Definitions 51 to 55, Propositions 26 to 29), and its file `chronicle/tests/check_items.cpp` is used as named.

## 4. Consequences

- **Checks.** `chronicle/tests/check_items.cpp` adds 27, 266 in all. Every earlier record replays with its seals.
  - The third version is admitted and records carry (50 random records); a declared directive is kept under both descriptions.
  - Applications are decided by rule, open, by a body at once and by proposal, by an office, by lot from a committed seed, and after a conversion. Recusal is checked in answering and in admitting. An assignment is ended by the office, and a party leaves every position.
  - A ruling is registered, and a second question on its subject refused citing it. A question is referred only to a ruling on its own subject. An overruling is refused from the steward and on a question that asks no review, and admitted from the reviewers; a review may uphold a ruling.
  - Overdue marks appear on the profile and in the briefing, and an item is shown to its author.
  - A petition is met by a checked deposit, adopted through a proposal and its amendment, and carried; a petition of a kind with no route is refused.
  - G8′ is held by admission; routes declared empty are admitted and their directives refused; routes are read from an older constitution; a community's own directive opens items on its route.
  - Proposition 28 holds on 100 random chronicles, Proposition 29 on 100 and 100 draws, and Proposition 27 on 60.
  - The record replays.
- **The theory note** gains §14 (Definitions 51 to 55, invariant G8′, Propositions 26 to 29).
- **The infrastructure.** The API answers items and rulings in the state, the briefing and the profile, with one check at its edge. Koinonia shows each community's items and rulings. The communities domain's presets are unchanged and admitted; no preset declares routes yet.
- **Next.** Posted work (the design's Phase 6): a type of item with its kinds, takers, terms and review, the conditions on application that read it.
