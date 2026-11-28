# passports: The Passport as a Document

Status: `draft` (decision 0007; moved beneath the chronicle by decision 0009).

## 1. Concept

A passport is the document a home issues for a mandate it has entered: every link of the mandate's chain as the act that issued it, each with its seal and its proof of inclusion under the home's head, the notaries' witnesses of that head, and the holder's declarations. Anyone who knows the home's head, or the head of a notary that witnessed it, verifies it from the document alone, as it is shown at an instant, at a host and with a credential (Proposition 11 of the theory note). Its written form is `noetarium.passport/1`.

The document stands beneath the chronicle so that a host's fold can verify a passport presented to it (Definition 41), against the trust its own record gives: the heads its office of status reported, and those of the notaries it recognises at the level of evidence. Issuing a passport and witnessing a head need a chronicle's state, and stay in `crossings`, which names the types of this module as its own.

## 2. Content

`Head`, `same`, `Witness`, `attestation`, `attests`, `Link`, `Passport` (with `mandate`), `Declarations`, `Trust`, `Presentation`, `verify`, `write`, `Reading`, `read_passport`.

## 3. Checks

This module's types are checked where passports are issued and presented: `crossings/tests/check_passports.cpp` (issued, witnessed, verified, written and read back, refused for every fault of the document) and `crossings/tests/check_presentation.cpp` (presented at a host and verified by its fold).
