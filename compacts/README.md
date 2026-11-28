# compacts: Agreements Among Communities

Status: `draft` (decision 0013).

## 1. Concept

A compact binds the communities that ratified it, each by its own ratification under its own rule, subject to its reservations, until its withdrawal takes effect (the design record of 2026-11-27, Sections 5.13 and 8.11; the Vienna Convention on the Law of Treaties, 1969, read as the ground reads a treaty and no further). Its terms, written `noetarium.compact/1`, declare its parties, its articles (each open or closed to reservation), whether reservations are admitted, its rule of entry into force (all its parties, or a number of ratifications), its withdrawal (a notice in turns, or none) and its last turn; their digest is that of the canonical form, so that two communities ratify the same compact when their digests agree.

A party's ratification is a record of its own chronicle: the compact, the terms it ratified with their digest, its reservations (each from a turn until withdrawn), the turn of ratification and of its withdrawal, effective after the notice. Read across the parties' records, the compact entered into force at the first turn its rule of entry held; it is in force while it has entered, its last turn has not passed and at least two parties are bound; a party is bound while its ratification stands and the compact is in force; an article binds two parties unless either reserved it, the reservation excluding it both ways. Each party's ratification and withdrawal are kept apart (Proposition 35), and the disclosure agreement of decision 0010 is the case of two sides (Proposition 36).

This module reads the terms, names their faults and those of reservations, and computes the standing; the acts that ratify and withdraw are amendments in each party's chronicle, and the standing across chronicles is read by whoever holds them (the communities domain's `federation`).

## 2. Content

`Article`, `Terms`, `read`, `faults`, `canonical`, `digest`, `Reservation`, `Ratification`, `reservation_faults`, `Record`, `standing` (of a party's ratifications, and of the compact), `Standing`, `bound`, `binds`.

## 3. Checks

`tests/check_compacts.cpp`: terms read and their faults named, their digest independent of how they are written; the faults of reservations; a compact of three entering into force when all have ratified, one party's withdrawal taking effect after the notice; entry by a number; articles between pairs with a reservation withdrawn; a last turn; on 300 random records the turn of entry found turn by turn and each party kept apart. The compacts in the fold, and Proposition 36, are checked in `chronicle/tests/check_counts_and_compacts.cpp`.
