# 0004. Crossings, and Votes Counted for Those Who Answer

- **Status:** Accepted, provisionally
- **Date:** 2026-11-26
- **Decided by:** Claude, continuing the implementation at the user's word ("continuer implementation"), on the design record's Sections 4.5 and 7.5

## 1. Context

A community's chronicle was sealed and replayable, but an act that passes between two communities, the witnessing of heads by a notary, and a party's exit with its record were in the design and not in code. Building a league on the fold showed a second gap: a person who votes as a community's envoy, under that community's mandate, was counted as a person of their own.

## 2. Decision

- **A module `crossings`.** A chronicle's tree has the entries' seals with their envelopes as leaves (archeion's Merkle tree after RFC 6962), and its head is its size with the tree's root. The receiver of a crossing act enters an attestation naming the act's seal and digest and returns a receipt: that leaf with its proof of inclusion under its head. The sender verifies it against its own chronicle. A notary's witnessed head is consistent with a later one when archeion's proof of consistency holds. A leaving party takes the acts that concern it with their proofs of inclusion.
- **Ballots.** A vote counts for the party that answers for it, the root principal of the voter's mandate or the voter, and by origin is weighed by that party's origin. A proposal keeps each voter's bearer.
- **Compacts.** An amendment may ratify a compact (`change.compact`). The constitution keeps the compacts it ratified (read from `external.compacts`).
- **Recusal.** An answer to a claim is refused when its author, or the party that answers for it, is the claimant, the party the claim is against, or of that party's origin: a body recuses any member who is a party to the claim. A claim against a person who alone holds the route then lacks the capacity to be answered, and is referred as the community's constitution says.

## 3. Alternatives Considered

- **Receipts signed by keys.** Deferred: the ground has no keys yet. The receipt here proves inclusion under a head; signatures over canonical JSON belong to the node.
- **Votes counted by author alone.** Rejected: it would let a community's person envoy count as a separate origin.

## 4. Consequences

- The worked record's results are unchanged, since its lab agents' mandates and controllers lead to the same party.
- Ten checks of crossings; 78 in all.
