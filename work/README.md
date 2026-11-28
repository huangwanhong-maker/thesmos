# work: Posted Work

Status: `draft` (decision 0012).

## 1. Concept

A community posts work whose life it declares as data. A kind of work, in the constitution's part `work`, says who posts it, how it is taken (by the taker's own commitment, or by an office's taking of the taker's offer, an award) and by how many at once, who is eligible (a position, and for a review another provider or another origin than the writer of the item reviewed), how long a taking runs, what its deliverable is with its check and the office that enters the check's result, how it is reviewed (none, by another origin, by another provider, or by a position) with the verdicts it gives and those it accepts, who accepts it (an office or a decision), within how many turns a returned delivery may be delivered again, and what must come first.

The life of an item is a state machine read from its record and the turn: open, or waiting for what comes first; taken, until something is delivered or its term lapses; delivered, awaiting its check, review or acceptance (a failed check leaves it taken); reviewed with a verdict the kind accepts; returned by any other verdict, delivered again within the declared turns, after which (or at once, where the kind declares none) the taking ends and the item reopens; accepted; withdrawn by its poster; overdue past its due turn. Every transition is an act of the chronicle; the lapse and the reopening are effects by law, functions of the record and the turns. An item lives under its kind's life as it stood when it was posted, so that an amendment of the life governs the items posted after it.

This module reads the kinds, names their faults, and computes the state of an item and of each taking; the acts and who may make them are the chronicle's.

## 2. Content

`Kind`, `Kinds`, `read_kind`, `read`, `Names`, `faults`; `Delivery`, `Review`, `Taking`, `Item`, `taking_state`, `active`, `state`, `overdue`.

## 3. Checks

`tests/check_work.cpp`, 9: the design's kinds read and their faults named; the life of an item step by step (lapse, check, return, redelivery, ending, acceptance, waiting, withdrawal, several takers, overdue); the life on 500 random records against an independent reading. The acts of the life in the fold are checked in `chronicle/tests/check_work_items.cpp`.
