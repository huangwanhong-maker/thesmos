// Crossings: acts that pass between chronicles, the receipts that prove them, the heads that notaries witness, and the record a
// party takes with it when it leaves.
//
// A chronicle's tree is the Merkle tree (archeion, after RFC 6962) whose leaves are its entries, each leaf the entry's seal with
// its envelope. A community's head at a moment is the pair (number of entries, root of the tree).
//
//   - A crossing act is entered in the sender's chronicle as any act is. The receiver enters an attestation naming the act's seal
//     and the digest of its envelope, and returns a receipt: its attestation's leaf with a proof of inclusion under its head. The
//     sender verifies the receipt against the receiver's head, so each side holds the other's proof of the exchange (the double
//     entry of the design record of 2026-11-25, Section 4.5; rule 8 of the schema of the ground).
//   - A notary keeps the heads it has witnessed. A later head is consistent with an earlier one when a proof of consistency shows
//     the earlier tree to be the beginning of the later; a community that rewrote an entry it had shown cannot give one.
//   - A party leaving takes the acts that concern it, each with its proof of inclusion under a witnessed head, and can show any of
//     them to anyone who knows that head (invariant G7, exit with the record).
//   - A passport (decision 0007) is the document noetarium.passport/1 that a home issues for a mandate it has entered: every link of
//     the mandate's chain as the act that issued it, each with its seal and its proof of inclusion under the home's head; the notaries'
//     witnesses of that head, each an attestation in the notary's own chronicle with its proof under the notary's head; and the
//     holder's declarations (its kind and, for an operative, its controller, provider and model). Anyone who knows the home's head, or
//     the head of a notary that witnessed it, verifies the passport from the document alone: every link is an act of the home, the
//     chain stands, its term holds at the instant of the check, it names the host where it is shown, and the credential shown is the
//     one whose digest the mandate names (Proposition 11 of the theory note). A passport grants no entry; entry is the host's.
//     Since decision 0009 the document, its verification and its written form are the module passports, beneath the chronicle, so
//     that a host's fold verifies a passport presented to it by the act `present`; this module issues passports and witnesses heads,
//     which need a chronicle's state, and names the document's types as its own.

#pragma once

#include <thesmos/chronicle/chronicle.hpp>
#include <thesmos/passports/passports.hpp>

#include <archeion/fixity/journal.hpp>
#include <archeion/fixity/sha256.hpp>

#include <symploke/jsonld/json.hpp>

#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::crossings {

using archeion::fixity::Digest;
using archeion::fixity::Inclusion;
using chronicle::State;

// The passport as a document, its witnesses and its verification are thesmos::passports's (beneath the chronicle, decision 0009),
// named here as they were.
using passports::Declarations;
using passports::Head;
using passports::Link;
using passports::Passport;
using passports::Presentation;
using passports::Reading;
using passports::Trust;
using passports::Witness;
using passports::attestation;
using passports::attests;
using passports::read_passport;
using passports::same;
using passports::verify;
using passports::write;

// ---- Trees and heads ----------------------------------------------------------------------------------------------------------

inline std::string leaf(const archeion::fixity::Entry& e) { return e.seal + "\x1e" + e.about; }

inline std::vector<std::string> leaves(const archeion::fixity::Journal& j) {
  std::vector<std::string> out;
  out.reserve(j.entries.size());
  for (const auto& e : j.entries) out.push_back(leaf(e));
  return out;
}


inline Head head(const std::string& community, const State& s) { return {community, s.journal.entries.size(), archeion::fixity::merkle_root(leaves(s.journal))}; }

inline std::string digest(const acts::Act& a) { return archeion::fixity::sha256_hex(acts::envelope(a)); }

// ---- Crossing acts and receipts -----------------------------------------------------------------------------------------------

struct Receipt {
  std::string sender, receiver;
  std::string act_seal, act_digest;         // the act as the sender entered it
  std::string receipt_seal, receipt_envelope;  // the receiver's attestation as it entered it
  Inclusion proof;                          // the attestation's leaf under the receiver's head
  Head at;                                  // the receiver's head when the receipt was given
};

// The receiver enters an attestation of the crossing act, authored by `attester` (a party of the receiver empowered to attest), and
// returns the receipt; or the reason the receiver's rules refuse it.
struct Received {
  bool admitted = false;
  std::string reason;
  Receipt receipt;
};

inline Received receive(State& receiver, const std::string& receiver_id, const std::string& attester, long long turn, const std::string& sender_id,
                        const std::string& act_seal, const acts::Act& act) {
  acts::Act a;
  a.author = attester;
  a.kind = "attest";
  a.turn = turn;
  a.fields = {{"crossing", act_seal}, {"digest", digest(act)}, {"from", sender_id}, {"kind of act", act.kind}};
  const auto o = chronicle::step(receiver, a);
  if (!o.admitted) return {false, o.reason, {}};
  const auto& e = receiver.journal.entries.back();
  Receipt r;
  r.sender = sender_id;
  r.receiver = receiver_id;
  r.act_seal = act_seal;
  r.act_digest = digest(act);
  r.receipt_seal = e.seal;
  r.receipt_envelope = e.about;
  r.proof = archeion::fixity::prove(leaves(receiver.journal), receiver.journal.entries.size() - 1);
  r.at = head(receiver_id, receiver);
  return {true, "admitted", r};
}

// Whether a receipt proves that the receiver entered the sender's act: the act stands in the sender's chronicle with that seal and
// envelope, the receiver's attestation names both, and its leaf is under the receiver's head.
inline apodeixis::kernel::Verdict verify(const Receipt& r, const State& sender) {
  using apodeixis::kernel::Verdict;
  const std::string claim = r.receiver + " entered the act " + r.act_seal.substr(0, 12) + " of " + r.sender;
  std::size_t k = 0;
  while (k < sender.journal.entries.size() && sender.journal.entries[k].seal != r.act_seal) k++;
  if (k == sender.journal.entries.size()) return Verdict::no(claim, "the sender's chronicle holds no act with that seal");
  if (digest(sender.acts[k]) != r.act_digest) return Verdict::no(claim, "the act's envelope differs from the one the receipt names");
  if (r.receipt_envelope.find(r.act_seal) == std::string::npos || r.receipt_envelope.find(r.act_digest) == std::string::npos)
    return Verdict::no(claim, "the receiver's attestation does not name the act");
  if (!archeion::fixity::included(r.receipt_seal + "\x1e" + r.receipt_envelope, r.proof, r.at.root) || r.proof.size != r.at.size)
    return Verdict::no(claim, "the attestation is not under the receiver's head");
  return Verdict::yes(claim, "the act stands in the sender's chronicle and the receiver's attestation of it is under the receiver's head");
}

// ---- Notaries -------------------------------------------------------------------------------------------------------------------

// The proof, from a chronicle as it stands, that the head it showed at `earlier` entries is the beginning of its present head.
inline std::vector<Digest> consistency(const State& s, std::size_t earlier) { return archeion::fixity::prove_consistency(leaves(s.journal), earlier); }

inline apodeixis::kernel::Verdict consistent(const Head& earlier, const Head& later, const std::vector<Digest>& proof) {
  return archeion::fixity::verify_consistency(earlier.size, later.size, earlier.root, later.root, proof);
}

// ---- Exit with the record ---------------------------------------------------------------------------------------------------------

struct Item {
  std::size_t place = 0;
  std::string seal, envelope;
  Inclusion proof;
};
struct Portion {
  std::string party;
  Head at;
  std::vector<Item> items;
};

// Whether an act concerns a party: it authored it, answers for it, or is named in its envelope as a recipient, a creditor, a
// claimant's counterpart, the party admitted or converted.
inline bool concerns(const State& s, const acts::Act& a, const std::string& party) {
  if (a.author == party) return true;
  if (!a.mandate.empty() && s.mandates.count(a.mandate) && mandates::root_principal(s.mandates, a.mandate) == party) return true;
  for (const char* k : {"to", "whom", "against", "from"})
    if (a.field(k) == party) return true;
  for (const auto& [p, position] : a.holders)
    if (p == party) return true;
  return false;
}

inline Portion extract(const std::string& community, const State& s, const std::string& party) {
  Portion p;
  p.party = party;
  p.at = head(community, s);
  const auto data = leaves(s.journal);
  for (std::size_t k = 0; k < s.acts.size(); k++)
    if (concerns(s, s.acts[k], party)) p.items.push_back({k, s.journal.entries[k].seal, s.journal.entries[k].about, archeion::fixity::prove(data, k)});
  return p;
}

// Whether every item of a portion is under the head it names; a third party who knows that head can rely on each.
inline apodeixis::kernel::Verdict verify(const Portion& p, const Head& known) {
  using apodeixis::kernel::Verdict;
  const std::string claim = "the record " + p.party + " took is under the head of " + known.community;
  if (p.at.size != known.size || p.at.root != known.root) return Verdict::no(claim, "the portion names another head than the one known");
  for (const Item& i : p.items)
    if (!archeion::fixity::included(i.seal + "\x1e" + i.envelope, i.proof, known.root)) return Verdict::no(claim, "the act at place " + std::to_string(i.place + 1) + " is not under the head", i.place);
  return Verdict::yes(claim, "every act it took is under the head", p.items.size());
}

// ---- Passports issued and heads witnessed (decision 0007) ---------------------------------------------------------------------------

// A notary's witness given in its own chronicle, with what its rules said.
struct Witnessed {
  bool admitted = false;
  std::string reason;
  Witness witness;
};

// A notary witnesses a head: its party `attester` enters the attestation in the notary's chronicle, under the notary's own rules, and
// the witness is returned with its proof; or the reason those rules refuse it.
inline Witnessed witness(State& notary, const std::string& notary_id, const std::string& attester, long long turn, const Head& h, const std::string& when = {},
                         const std::string& mandate = {}) {
  const auto o = chronicle::step(notary, attestation(h, attester, turn, when, mandate));
  if (!o.admitted) return {false, o.reason, {}};
  Witness w;
  w.notary = notary_id, w.attester = attester, w.mandate = mandate, w.turn = turn, w.when = when;
  w.seal = notary.journal.entries.back().seal;
  w.proof = archeion::fixity::prove(leaves(notary.journal), notary.journal.entries.size() - 1);
  w.at = head(notary_id, notary);
  return {true, "admitted", w};
}

// Whether a witness attests the head `h`: its attestation of exactly that head stands under the notary's head it names.
struct Issued {
  bool admitted = false;
  std::string reason;
  Passport passport;
};


// The passport of the mandate `id` as the home `home` stands: the acts that issued every link of its chain, each with its seal and
// its proof under the home's present head, and the holder's declarations. Refused when the mandate does not stand at its home (a link
// widens, is revoked, or answers to nobody), names no credential, has ended its term in instants at `at`, or is held by an operative
// whose declared controller issued no link of its chain. Witnesses are added as notaries give them.
inline Issued issue(const std::string& home, const State& s, const std::string& id, const std::string& at = {}, const Declarations& d = {}) {
  const auto refuse = [](std::string why) { return Issued{false, std::move(why), {}}; };
  if (!s.mandates.count(id)) return refuse("the home has issued no mandate " + id);
  if (const auto f = mandates::faults(s.parties, s.mandates, id, s.revoked); !f.empty()) return refuse("the mandate does not stand at its home: " + f.front());
  const mandates::Mandate& m = s.mandates.at(id);
  if (m.credential.empty()) return refuse("a passport names its holder's credential by digest, and the mandate " + id + " names none");
  if (!at.empty() && !mandates::is_instant(at)) return refuse("a passport is issued at an instant, ISO 8601 in UTC (YYYY-MM-DDTHH:MM:SSZ)");
  if (!at.empty() && !m.until.empty() && m.until < at) return refuse("the mandate's term in instants ended at " + m.until);
  const auto links = mandates::chain(s.mandates, id);
  const kernel::Party& holder = s.parties.at(m.holder);
  if (holder.kind == kernel::Kind::operative && !passports::detail::controller_in_chain(holder.controller, links))
    return refuse(holder.controller.empty() ? "an operative's passport names its controller, and the home declares none for " + m.holder
                                            : "the controller the home declares for " + m.holder + ", " + holder.controller + ", issued no link of its chain");
  Passport p;
  p.issued = at, p.home = home, p.node = d.node, p.status_at = d.status_at, p.model = d.model, p.label = d.label;
  p.principal = s.parties.at(links.front()->principal);
  p.holder = holder;
  const auto data = leaves(s.journal);
  for (const mandates::Mandate* link : links) {
    std::size_t k = 0;
    while (k < s.acts.size() && !(s.acts[k].kind == "mandate" && s.acts[k].grant && s.acts[k].grant->id == link->id)) k++;
    if (k == s.acts.size()) return refuse("the act that issued " + link->id + " is not in the home's chronicle");
    acts::Act a = s.acts[k];
    a.body.clear();
    p.chain.push_back(Link{std::move(a), s.journal.entries[k].seal, archeion::fixity::prove(data, k)});
  }
  p.head = head(home, s);
  return {true, "issued", p};
}

// What a verifier knows: the heads of homes it knows itself, and the heads of notaries whose witness it accepts.
}  // namespace thesmos::crossings
