# disclosure: Disclosure Agreements

Status: `draft` (decision 0010).

## 1. Concept

Epistemic behaviour (an agent's reasoning, a plan, a deliberation, a model call's prompt and answer) enters a community's record only by disclosure under an agreement between the agent's principal and the community, ratified by each side under its own rule and entered in the community's chronicle. The terms, written as `noetarium.disclosure/1`, declare the scope (the kinds of epistemic behaviour, the agents covered, the scenes, matters and turns), the purposes the record admits and those it never admits, its custody (the holding of kind `epistemic record`, its custodian, its retention), who may consult it (the principal, and the positions of a body), the cost and its ceiling, and how the agreement is withdrawn (by whom, after what notice, and whether what was recorded is then kept, returned or erased).

This module reads the terms and names their faults of form, gives the digest that identifies them, decides whether they cover an epistemic episode, and makes the norms of access to the record (decision 0006): a permission to consult for the purposes admitted and, where the terms forbid uses, a prohibition for those, for the principal (the attribute `party`) and each position of the body. What the terms say of a community (its offices, bodies and parties), when the agreement stands in force and ends, and what is recorded under it, are the chronicle's.

## 2. Content

`kinds`, `purposes`, `Terms`, `read`, `digest`, `faults`, `covers`, `norms`.

## 3. Checks

In `chronicle/tests/check_disclosure.cpp`: ratification and its refusals, coverage, consults and citations, denunciation and the ends of a record, and Propositions 24 and 25 on random runs.
