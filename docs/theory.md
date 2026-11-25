# The Theory of thesmos

This note states what thesmos computes and what it guarantees. Its source is the design record *A Society of Human and AI Communities* (2026-11-25) in the infrastructure, corrected after an independent reading for error on the same day.

## 1. Parties and Origins

**Definition 1.** A *party* is an identified participant of one declared kind: person, organisation, operative (an identity under which a program acts) or community. Persons, organisations and communities are *answerable*; operatives are not.

**Definition 2.** *Control* is a declared partial function from operatives to parties (joint control is declared as control by one party, an organisation of the controllers); *provision* names an operative's provider and model. A relation of control with an unknown controller or a cycle of operatives is faulty and refused.

**Definition 3.** The *origin* of a party under control is the first party that is not an operative reached along control; an operative whose control is undeclared is its own origin, by declaration. Under provision, the origin of an operative is its provider.

*Remark.* Sameness of origin is sameness under a named criterion. Two operatives whose traces coincide stay distinct until a declared relation joins them.

## 2. Mandates

**Definition 4.** A *mandate* states a principal, a holder, kinds of act, matters, an allowance and a slice for each resource in that resource's own unit, a term in turns, whether it may be delegated, and its parent. A chain *stands* when (i) its root principal is answerable, (ii) each link was issued by the holder of its parent and the parent may be delegated, (iii) each link narrows its parent in kinds, matters, allowance and term, and (iv) no link is revoked.

**Proposition 2.** In a standing chain every link's scope lies within every link above it. If each charge is made against every link from the charged one to the root, the use beneath any link is at most its ceiling.

*Proof.* Inclusion and order are transitive; a charge is admitted only when every link above has room and is then added to each. ∎ With ceilings charged apart the bound fails: two children of 80 under a root of 100 admit 160.

**Proposition 3.** Let one verifier, remote from the place where a mandate's status is kept, check the status at fixed windows and admit at most σ of a resource between two checks; let a revocation be known at the place of keeping when it is entered there. Then what the verifier admits after a revocation is at most min(B, σ), B the allowance remaining.

*Proof.* The verifier learns of the revocation at its next check; before it lies part of one window, in which at most σ, and at most what remains, is admitted. ∎ The community's own fold keeps the status and knows a revocation at once.

## 3. Acts and the Chronicle

**Definition 5.** An *act* has an author, a kind of the ground with its force (expressive, assertive, directive, commissive, declarative), a turn, an optional mandate, a typed envelope of fields, and a free body.

**Definition 6.** A community's *chronicle* is the sequence of the acts it admitted, each sealed as archeion seals a journal's entries: the envelope's canonical text as what the entry is about, the body as its note. The *fold* `step` advances the relational state (positions, mandates, revocations, commitments, grants, claims, proposals, assessments, charges, rules in force) by one act, or refuses it and changes nothing.

**Proposition 1.** Given the declared parties with their relation of control, the constitution in force at the founding and a chronicle that seals, the relational state is determined. Two chronicles that differ only in bodies give the same relational state and different seals.

*Proof.* `step` is a function of the state and of the envelope; induction on the chronicle. A body enters the seal and stays out of `step`. ∎

## 4. Commitments, Set-off and Novation

**Definition 7.** A commitment is conditional (an offer), active, presented, handed over (the performer's authority ended and what remains is stated), discharged, released, cancelled (a breach) or withdrawn; overdue is a mark on an active commitment past its due turn. Its debtor is the party that answers for the act that made it.

**Proposition 5.** Let o(i, j) be whole-number obligations of one class in which set-off is agreed, and N the sum of the positive net obligations. (1) Cancelling a common amount around a cycle keeps every net obligation. (2) Cancelling cycles to exhaustion leaves an acyclic residual. (3) Its volume is at least N. (4) On it, replacing "i owes j, j owes k" by "i owes k" with the consent of the three, as far as it goes, ends with volume exactly N and every party purely a debtor or purely a creditor, net obligations kept.

*Proof.* (1) A cycle enters and leaves each of its parties once. (2) By construction. (3) Each party owes at least its positive net obligation. (4) Each replacement lowers the volume by its amount, at least one unit, so the process ends; it keeps net obligations as (1) does and adds an edge only along an existing path, so the residual stays acyclic; at the end no party both owes and is owed, so each debtor owes exactly its net obligation. ∎ Rational amounts are scaled to whole numbers. Cancelling cycles in some order can clear less than the greatest circulation, a problem of minimum-cost flow.

## 5. Counts and Rules

**Definition 8.** A rule of aggregation declares its unit of count. By operative, each voter counts one. By origin, each origin weighs one, divided equally among its voters. By person or by community, the same over origins of that kind alone.

**Proposition 6 (as corrected).** (a) Every origin weighs one whatever the number of its voters. (b) When an origin whose voters vote alike replaces them by any number of voters that vote as they did, the share in favour is unchanged. (c) When an origin's voters divide, adding voters to it moves its own weight in favour within [0, 1], and so moves the share by at most 1/O, O the number of origins counted.

*Proof.* The share is the mean over origins of each origin's share of voters in favour; an origin's term lies in [0, 1] and is unchanged when its voters vote alike. ∎ The statement first written in the design omitted the condition of (b); a divided origin that adds a voter in favour moves the share from 3/4 to 5/6, within the bound of (c).

## 6. Constitutions

**Definition 9.** A *constitution* declares positions with the kinds of party that may hold them, boundary rules of entry and exit, choice rules (who may, must and must never make which kinds of act, in which scenes), bodies, decisions with a body, a rule, a unit of count and a tempo, scenes, conversions between registers, receiving capacity, forms of circulation, a route for claims, the custody of means with any reserve, preserved properties, a rule of amendment for each level with what is entrenched, a policy of records, and tempi.

**Admission.** A constitution is admitted when nine conditions of form hold: closure; decisions with bodies, rules and units, and bodies that can be filled; amendment at each level and entrenchment named; a route for claims; conversions listed; custody declared, with every executable power recognised or reserved with scope and term; preserved properties declared; receiving capacity and forms of circulation; a policy of records. The fold refuses an amendment whose result would not be admitted.

## 7. The Schema of the Ground

The ground's rules of answerability are stated as equations of a category (`schema/ground.cat`): a root is its own root; the party that answers for a chain is its root's principal and is answerable; an act under a mandate is made by its holder and answered for by its root's principal; a commitment's debtor is the party that answers for the act that made it; a crossing act is sent from the sender's chronicle and entered in the receiver's. A record is a set-valued functor on this category; kategoria answers whether a record is one, and finds a record that breaks a rule by the equation it breaks.

## 8. The Facets of a Norm

Regularity (the share of applicable episodes that conform) and operation (the decisions that invoke the norm) are measured from a chronicle; adoption and justification are acts. A norm invoked and never adopted is marked *operative, unadopted*; one adopted and never invoked *adopted, inoperative*.
