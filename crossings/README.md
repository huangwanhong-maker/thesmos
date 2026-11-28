# crossings: Acts Between Chronicles

Status: `draft` (decision 0004; passports, decision 0007; presentation, decision 0009).

## 1. Concept

A chronicle's tree has its entries (seal and envelope) as leaves, and its head is its size with the tree's root. A crossing act is entered by the sender; the receiver enters an attestation and returns a receipt, its leaf with a proof of inclusion under its head, which the sender verifies. A notary witnesses heads, and a later head stands only with a proof of consistency, so that a rewritten history is found. A leaving party takes the acts that concern it with their proofs.

A passport is the document a home issues for a mandate it has entered (the module `passports`, beneath the chronicle since decision 0009). This module issues it from the home's state, every link of the mandate's chain as the act that issued it with its proof of inclusion under the home's head, and has notaries witness the head. A host takes the passport in by the act `present`, which its fold verifies against the heads its own record knows (Definition 41 of the theory note).

## 2. Content

`leaf`, `leaves`, `head`, `digest`, `Receipt`, `receive`, `verify`, `consistency`, `consistent`, `Portion`, `extract`, `concerns`; for passports `witness`, `Witnessed`, `issue`, `Issued`, and, named from `passports`, `Head`, `Witness`, `attestation`, `attests`, `Link`, `Passport`, `Declarations`, `Trust`, `Presentation`, `verify`, `write`, `read_passport`.

## 3. Checks

A crossing act entered on both sides and its receipt verified; a forged or altered receipt refused; a notary's witnessed head consistent with a later one and a rewritten chronicle found; a party's portion of the record verified against the head, and an altered or misheaded one refused (`check_crossings.cpp`, 10). Passports (`check_passports.cpp`, 25): issued, verified, written and read back; witnessed by a notary; refused for a forged proof, an unwitnessed or unknown head, an expired term, a credential whose digest differs and a host not named; Propositions 11 to 13 on random homes and on seals computed by Python's hashlib. Presentation (`check_presentation.cpp`, 19): parties declared by a founding from an empty registry; the part `passports` admitted within closure and kept public; a passport presented at a host, refused until its office reported the home's head and the host recognised the home, then admitted with its holder and principal known parties holding no position; refused for a forged proof, another host, an expired term, a presenter not the holder, another mandate, a community admitting no passports, an issuer not named and a holder known otherwise; a witnessed head admitted once its notary is recognised; recognition and reports of status; Propositions 19 to 21 on random homes and hosts.
