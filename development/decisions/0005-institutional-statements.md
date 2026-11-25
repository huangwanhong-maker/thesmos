# 0005. Institutional Statements

- **Status:** Accepted, provisionally
- **Date:** 2026-11-26
- **Decided by:** Claude, continuing the implementation at the user's word, on the module planned in decision 0003 and the design record's Section 5

## 1. Context

The register of norms measured a norm's facets from predicates written in code. A community writes its rules in words, and the constitution's choice rules already hold who may, must and must never make which kinds of act, with what calls for a duty, its term and what follows if it is not done.

## 2. Decision

- **A module `statements`.** A regulative statement in the parts of the grammar of institutions (Crawford and Ostrom 1995; Frantz and Siddiki 2021): attribute, deontic (may, must, must never), aim (a kind of act of the ground), object, the act that calls for a duty, its term in rounds, and an or-else. Its written form is read by a grammar in BNF on morphe's reader, the aims being the ground's kinds of act, and refused with where it stops and what could follow; a statement writes itself back in the same form.
- **Statements from a constitution.** The choice rules are read as statements; a rule's "when", "within" and "or else" are kept by the constitution's reader.
- **Statements as norms.** A duty with a condition and a term becomes a norm over a chronicle: each act of the condition is an episode, conforming when a holder of the attribute makes the aim on the same matter within the term. A prohibition's episodes are the acts it forbids. The four facets are then measured by `norms`.

## 3. Alternatives Considered

- **The full Institutional Grammar 2.0,** with constitutive statements, nesting and levels of expressiveness. Deferred: the regulative core is what the register of norms needs first.
- **Statements read by a language model.** Rejected for the record: a model may propose a statement, which is read by the grammar like anyone's.

## 4. Consequences

- Seven checks; 86 in all.
