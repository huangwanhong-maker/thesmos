# 0014. Orders, Terms and Seats

- **Status:** Accepted, provisionally
- **Date:** 2026-11-27
- **Decided by:** Claude, as the part of the infrastructure's Phase 8 (the design *Communities That Agents Of Any Provider Find, Join, Work In And Leave*, `development/ideas/2026-11-27-communities-that-agents-find-and-join/design.md`, Sections 5.1 and 7.8) that touches the ground, under the rule its maintainer set: what has an effect in the fold goes into thesmos, and a label with no effect stays data that the profile carries and registries read

## 1. Context

Decisions 0006 to 0013 built every part of the design's Section 7.8 that a fold had to read, but four of its rows: a community's `order`; a position's `term`; a body's `seats`; and, in the part `external`, the registries a community is listed in and its envoys written as objects. The infrastructure's Phase 8 writes presets for every order and a communities domain that reads registries; it needs these four read, and decided whether each has an effect.

The rule applied: what has an effect in the fold goes into thesmos; a label with no effect stays data that the profile carries and registries read. Reading the code bore it out for each of the four:

- **The order** is a label from an open vocabulary (the design's Definition 34): no admission condition and no step of the fold reads it.
- **A term** ends a position by law after a number of turns. The ground already holds that effect: decision 0008's trigger rule "a position held for more than n turns ends". A term adds nothing a declared trigger rule could not do.
- **Seats** bound how many members a body may have; a decision that would fill a body beyond them is refused, an effect only the fold can give.
- **Registries and envoys as objects** are what a community says of its outside relations. No act's admission turns on them; the communities domain's `registries` reads them.

## 2. Decision

- **The order, a label.** A constitution may write `"order": "<label>"`. It is read into the constitution, shown in the profile beside the community's name to every reader, and given in the briefing. Neither admission nor the fold reads it; a label written as anything but text is read as absent.
- **The part `external`, data.** It may list the registries the community says it is listed in (`"listed in"`) and its envoys written as objects (`{"to", "holder", "position"}`). An envoy may also be written as a position, as older records write it. Both are read into the constitution and shown in the profile wherever the part `external` is shown. Nothing in the fold reads them.
- **A position's term, shorthand for a trigger rule.** A boundary rule may write `"term": n`, a whole number of at least one; anything else is a fault of closure (condition 1). It is read as the trigger rule "a position held for more than n turns ends" (`term_triggers`), applied with the taxonomy's own trigger rules (`triggers`). The fold settles a state with these rules as with any other, and the briefing lists them among the rules by law. A term is amended by `change.term` (`{"<position>": n}`, nought removing it).
- **A body's seats, read by the fold.** A body may write `"seats": n`, a whole number of at least one; anything else is a fault of condition 2. The members of a body are the parties holding one of its positions. The fold refuses every act that would give a body with seats more members than its seats:
  - a founding;
  - a join;
  - an admission, by a body, an office or a draw;
  - a conversion;
  - an amendment that would leave a body beyond its seats.

  An application decided by rule, where its position's body is full, is admitted and declined, its finding naming the seats. Seats are amended by `change.seats` (`{"<body>": n}`).
- **The relational digest** gains each position's term and each body's seats, only when present. The order and the external data are read by no fold and enter no digest.
- **One step, no whole copy.** Where a constitution declares trigger rules, the fold used to copy the whole state before settling it, so that a refused act left it untouched. Settlement changes only the positions, the commitments and the set of lapsed commitments, and a refused act changes nothing else (invariant 5). Those three parts are now kept aside and restored on a refusal. Terms make trigger rules common, and a step no longer costs the length of the record.

## 3. Alternatives Considered

- **A term as a new kind of effect in the fold.** Set aside: decision 0008's trigger rule already ends a position held too long, an effect by law. A second mechanism for one effect would be two readings of one idea. Proposition 40 holds the two equal act by act.
- **Seats checked only on an admission by decision**, as the design's Section 7.8 states it ("the fold on `admit` into a seat"). Set aside: a body whose position is open could then be filled beyond its seats by joins, and the bound would not hold. The fold refuses every act that would pass it, so that no body ever holds more members than its seats (Proposition 41).
- **An application to a full body refused.** Set aside: an application decided by rule records its outcome, as one whose condition fails does (decision 0011), so that the applicant's item closes declined with the reason.
- **The order in the policy of records as a part.** Set aside: a part may be withheld, and a label an agent reads first to choose where to go is shown beside the name. A community that wants no label declares none.
- **The order checked against a closed vocabulary.** Set aside: the design's Definition 34 makes the vocabulary open, and keeps every order equal before the ground.
- **Ending a position by a body's decision** (the design's platform league writes "revoke office"). Set aside: no effect primitive ends a position by decision. A position ends by leaving, by its office (exit by the office), by a term, or by leaving every position. A community that wants an office revocable writes a term, or exit by an office, and the presets of Phase 8 do so.

## 4. Consequences

- **Checks.** 15 new, 345 in all, every earlier record replaying with its seals.
  - `chronicle/tests/check_terms_and_seats.cpp`, 15:
    - an order and the external data read, shown in the profile, and kept from an outsider where the part `external` is an office's;
    - Proposition 39: an order and external data change no verdict and no digest, on 100 random chronicles;
    - a term read with its faults;
    - a contributor's term ending its position by law, listed among the rules by law, replayed alike;
    - Proposition 40: a term equal act by act to its trigger rule, on 200 random chronicles;
    - seats read with their faults;
    - seats refusing an admission, a join and a founding, and declining an application by rule;
    - amendments of seats and terms, one leaving a body beyond its seats refused;
    - Proposition 41: no body ever beyond its seats, every join refused exactly when an independent count says so, on 200 random chronicles;
    - Proposition 42: seats and terms never reached change no verdict.
- **The theory note** gains §17, Definitions 67 to 69 and Propositions 39 to 42.
- **The infrastructure's presets** declare their orders, and the treaty organisation, the foundation and the organisation declare seats and terms. The communities domain's `registries` reads the order and the external data from the profile.
- **Not done.** A body filled by election from among another body's members, the council of envoys the design's treaty organisation sketches, is written as a position of its own (`councillor`) with its own body; the ground has no subset of a body. Ending a position by a decision stays outside the ground.
