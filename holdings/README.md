# holdings: What a Community Holds, and the Norms of Access

Status: `draft` (decision 0006).

## 1. Concept

A community holds resources of many kinds (data, documents, tools, budgets, capacity, knowledge, briefings, the time of agents, references to accounts kept elsewhere). A holding has an identifier, a kind, a custodian (an office of the constitution or a named party), a provenance (a digest by archeion's rule, a source, a licence), possibly a container, and, when metered, a unit and a quantity. A holding may declare review: what is deposited within it stands checked only when a party of another origin attests it. A holding is public, members' (the default) or shown to named offices: its visibility says who may know of it and read its description, while reading its content is an act under the norms.

Norms of access are written in the grammar of institutions: an attribute (anyone, a kind of party, anyone who holds a position, a party whose origin holds a position, a position, the custodian or the depositor), a deontic (may, must, must not), an aim (consult, use, deposit, modify, lend, withdraw), an object (any holding, a kind, a holding with what is kept within it), conditions (under a mandate of given kinds and matters, within a quota a round, by need under a declared standard, for a declared purpose, sent or not to a service outside the community) and an or-else. A permission silent on transmission permits none; a prohibition silent on it forbids whatever is sent. In words: `contributor may deposit to the shelf.`, `member may use the writing pool within 2000 a round.`

The resolution is declared: the most specific norms decide (the object first, the nearer holding first; then the attribute, a party named (since decision 0010: the party, the operatives whose origin it is and whoever acts under its mandates), a role, a position held by the author itself, one held by the party that answers for it, an origin, anyone holding a position, a kind, anyone); a prohibition whose conditions hold prevails at that level; a permission whose conditions hold permits; otherwise the request is refused with the conditions failed. Where no norm speaks, the request is refused unless the constitution declares the kind open to the aim. The verdict depends on the set of norms alone (the theory note, Proposition 8).

The fold's use of holdings (acts of access admitted, charged along chains, recorded), the whole briefing and the profile are in the module `chronicle`.

## 2. Content

`Holding`, `Provenance`, `Visibility`, `sees`, `read_visibility`, `Aim`, `Deontic`, `Attribute`, `Object`, `Conditions`, `Norm`, `Register`, `Target`, `Situation`, `Request`, `Specificity`, `matches`, `unmet`, `decide`, `resolve`, `Entry`, `brief`, `duties`, `text`, `read`, `faults`, `digest`.

## 3. Checks

A worked set of norms decided as the resolution states (a holding before a kind, the holding itself before its container, a prohibition at equal specificity, a quota, a need, a mandate, a purpose, a hand bound more narrowly than its member, open access); transmission permitted where a norm says, never where it is silent, and forbidden by a prohibition of sending; visibility; Proposition 8 on 3,000 random registers (the verdict independent of the order of the norms, the norm cited the first of its class, equal to the definition read literally, unchanged by a less specific norm, refused by a prohibition at the deciding level, every refusal naming its norm or the absence of one); Proposition 10 on 1,000 random registers; norms in words; the written form read and breaches of form found under closure and the custody of means; digests by archeion's rule.

*Remark.* Since decision 0012 a holding may declare review by another provider (what is deposited within it stands checked only when a party whose provider differs from its depositor's attests it) and verdicts with those that check: an attestation with another verdict records the review and leaves the deposit pending.

*Remark.* Since decision 0013 the part `access` may declare `"operatives transmit to their provider": true`, under which the base taxonomy's counts-as rule makes an operative's reading (a consult, use, loan or withdrawal) reach its declared provider, unless that provider is `local`. The provider reached is a request's second transmission (`Request::reaches`) beside the one its act declares: a permission decides only when it covers each, and a prohibition naming transmissions speaks only to a request sent to one it names. The briefing's entries are decided with the provider the party's readings reach (`Entry::reaches`); `reads` names the aims that read.
