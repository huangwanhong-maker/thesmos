// The chronicle: the sealed sequence of a community's acts, and the fold that gives its relational state.
//
// Each community keeps the sequence of the acts it admitted, each sealed to the one before in archeion's hash-chained journal
// (the act's envelope as what the entry is about, its body as the entry's note). The standing relations among its parties
// (positions held, mandates and their revocations, commitments, grants, claims, proposals, assessments and what each mandate
// has been charged) are computed from that sequence by `step`, which reads the constitution in force and an act's envelope
// and never its body. A refused act changes nothing and is not entered.
//
// Proposition 1 (the design record of 2026-11-25, as corrected): given the declared parties with their relation of control, the
// constitution in force at the founding, and a chronicle that seals, the relational state is determined; two chronicles that
// differ only in bodies give the same relational state and different seals.
//
// Choice rules are read where they speak: a kind of act that some choice rule names may be made only by a holder of a position
// that the rules empower (by the author, or by the party that answers for it), and never by a holder whom a rule forbids; a kind
// no rule names is governed by its own conditions (a join by the boundary rule, a revocation by the chain, an acceptance by the
// commitment). Admissions, votes, amendments and conversions are decided by the constitution's own bodies, rules, units of count
// and tempi; an amendment that would leave the constitution inadmissible is refused.
//
// A community's holdings (decision 0006) are declared by its constitution and held from its founding. An act that consults, uses,
// deposits, modifies, lends or withdraws a holding is admitted only when the norms of access permit it under their declared
// resolution (holdings.hpp), and is refused with the norm that forbids it, the conditions it failed, or the absence of any norm.
// What it draws from a metered holding is taken from what is available and, when it is made under a mandate, charged along the
// whole chain in that holding's own unit (invariant G4, Proposition 9). Every access admitted is recorded: who, which holding, what
// was done, how much, for what purpose, and the norm that permitted it. What is deposited within a holding that declares review
// stands checked when a party of another origin than the depositor's attests it.
//
// A home issues the mandates its passports carry (decision 0007). A mandate that names its holder's credential names it by its digest;
// one whose term is stated in instants is out of its term for an act whose instant lies outside it; one that reserves has its ceilings
// charged, when it is issued, against the links a charge under its parent reaches, and what is later charged under it is charged from
// it down (Proposition 12). A record with no reserving mandate is charged along whole chains, as before.
//
// Under the taxonomy of behaviour (decision 0008), an act of a declared kind is read as an act of the kind of the ground it counts as:
// the reduct must be admitted, the declared kind's conditions hold, and the effect is the reduct's (Proposition 14); the act is kept as
// written and its reduct as its institutional reading. An act may carry the digest of the episode of conduct that constituted it, typed.
// A counts-as rule may make an act of access a transmission. Declared trigger rules are settled to each act's turn, the settlement kept
// only with an admitted act, so that every institutional effect is an admitted act's or a declared rule's over the record and its turns
// (invariant G9, Proposition 16). A revision of the part behaviour is an amendment, admitted when the revision is (Proposition 17).
//
// Under presentation and recognition (decision 0009), a founding may name its community and declare its parties, and a presentation
// declares its holder and the root principal of its chain, so that a chronicle replays from an empty registry (Proposition 21); what
// an act declares is staged before its author's authority is judged and withdrawn when the act is refused. `recognise` sets the level
// at which this community recognises another. A report by the office of status the part `passports` names may state another
// community's head, which the host then knows, and the status of a foreign mandate presented here. `present` verifies a passport
// against the heads the host's own record knows (Proposition 18), registers its links as mandates of another community and gives no
// position; an act under a foreign link stands revoked or suspended as last reported, its term in turns is not read here, and what is
// charged under it between two reports of its status is at most its slice (Proposition 19).
//
// Under disclosure agreements (decision 0010), `agree` ratifies an agreement for one side: for the principal by an act it answers for,
// under its own rule and never the community's choice rules; for the community by an office its choice rules empower to agree. The
// agreement stands in force from the turn both sides have ratified the same terms, and its record, a holding of kind "epistemic
// record", is held from then with the norms of access its terms declare. `denounce` ends it after its notice; it ends by law at its
// last turn too. A consult of the record is refused once its retention has run, or when it was erased, or by any but the principal
// when it was returned; nothing but a consult reaches it. An act that cites an epistemic record (its field "epistemic record") uses
// it for the purpose it states, which the agreement must admit and never forbid, after a consult of it for that purpose.
//
// Under items, routes and rulings (decision 0011), a request that names a type opens an item of that type on its route: an application
// (the declared kind apply), a question, a petition, or a type the constitution's routes declare; a request with no type is as it was.
// An application is decided by the position's boundary rule: at once when its entry is open, by rule, sponsored or after a conversion;
// by an admission from the body that decides, or from the office that assigns; by a draw from a committed seed. A question on a subject
// with a ruling in force is refused with the ruling cited, unless it asks the body that reviews rulings to review that ruling; a ruling
// enters the register, and an overruling comes only from the review body. A petition is met by a deposit checked within the holding
// its route names, adopted by a proposal under its route's decision, or carried by an office. Every item is answered on its route by
// a party who is not a party to it. Where the constitution declares its routes, a directive with no route is refused (G8'); where it
// declares none, routes are read from what it declares, so that every older record replays.
//
// Under posted work (decision 0012), a request that names a kind of work (its field "work") posts a work item, whose life the kind
// declares as data (thesmos's module work): taken by the taker's own commitment naming it, or by an office taking the taker's offer, only
// while the item is open and what must come first is accepted, by one the kind and the item find eligible, never by more than the kind's
// takers at once; delivered with an artefact, checked by the office the kind names where it declares a check, reviewed as the kind
// declares (by another origin, by another provider, or by a position, never from the deliverer's origin), and accepted by its office or
// decision only after a verdict the kind accepts; returned by any other verdict, delivered again within the turns the kind declares, after
// which (or at once, where it declares none) the item reopens; lapsed, reopening the item, when its term runs with nothing delivered; and
// withdrawn by its poster while no one holds it. A kind's life is amended as any rule is (change.work).

#pragma once

#include <thesmos/acts/acts.hpp>
#include <thesmos/commitments/commitments.hpp>
#include <thesmos/constitutions/constitutions.hpp>
#include <thesmos/disclosure/disclosure.hpp>
#include <thesmos/holdings/holdings.hpp>
#include <thesmos/passports/passports.hpp>
#include <thesmos/procedures/procedures.hpp>

#include <archeion/fixity/journal.hpp>
#include <archeion/fixity/sha256.hpp>

#include <apodeixis/kernel/verdict.hpp>

#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace thesmos::chronicle {

using acts::Act;
using commitments::Commitment;
using commitments::Status;
using constitutions::Constitution;
using kernel::Kind;
using kernel::Parties;
using mandates::Mandate;

struct Grant {
  std::string grantor, holder, resource;
  long long ceiling = 0, used = 0;
};
struct Claim {
  std::string claimant, against, status, finding;
};
struct Proposal {
  std::string id, decision, author;
  std::string petition;  // the petition it carries, if any (decision 0011)
  long long opened = 0;
  std::map<std::string, std::string> change;
  std::map<std::string, bool> votes;
  std::map<std::string, std::string> bearers;  // voter -> the party that answers for its vote
  std::string status = "open";

  std::vector<procedures::Ballot> ballots() const {
    std::vector<procedures::Ballot> out;
    for (const auto& [voter, yes] : votes) {
      const auto b = bearers.find(voter);
      out.push_back({voter, b == bearers.end() ? voter : b->second, yes});
    }
    return out;
  }
};
struct Assessment {
  std::string by, bearer, object, criterion, value;
};
// An item opened by a request of a type (decision 0011): who asked and who answers for it, when it opened and is due, its status, what
// it is about (an application's position, a question's subject, a petition's kind), and how it closed.
struct Item {
  std::string id, type, author, bearer;
  long long opened = 0, due = -1;
  std::size_t place = 0;        // the place in the chronicle of the act that opened it
  std::string status = "open";  // open; admitted, declined, ruled, referred, upheld, answered, met, adopted, carried
  std::string position, about, subject, kind;
  std::string reviews;          // a question that asks the review body to review a ruling: the ruling
  std::string deposit, proposal;  // a petition's deposit or proposal
  long long closed = -1;
  std::string closed_by, finding;
  std::string subject_key() const { return about + ": " + subject; }
};
// A ruling in the register (decision 0011): the question it answered, its subject and reading, who gave it, what it overruled, and
// whether it stands.
struct Ruling {
  std::string id, subject, reading, by, overrules, overruled_by;
  long long turn = 0;
  bool in_force = true;
};
// An access to a holding, as admitted: who made it and who answers for it, which holding, what was done, how much, for what purpose,
// and the norm that permitted it ("open access" when no norm spoke and the kind is open).
struct Access {
  long long turn = 0;
  std::string author, bearer, holding, aim, purpose, norm;
  long long amount = 0, counted = 0;  // counted: what it counts against a quota
  std::string to;                     // a loan's borrower
  long long until = -1;               // a loan's last turn
  std::string transmit;               // the service outside the community what was reached was sent to, if the act declared one
  std::string reached;                // the provider an operative's reading reached (decision 0013), if any
};

struct Outcome {
  bool admitted = false;
  std::string reason;
};

struct State {
  Parties parties;
  Constitution rules;
  std::map<std::pair<std::string, std::string>, long long> positions;  // (party, position) -> turn of entry
  mandates::Registry mandates;
  std::set<std::string> revoked;
  std::map<std::string, Commitment> commitments;
  std::map<std::string, Grant> grants;
  std::map<std::string, Claim> claims;
  std::map<std::string, Proposal> proposals;
  std::vector<Assessment> assessments;
  std::map<std::string, std::map<std::string, long long>> used;  // mandate -> resource -> charged along the chain
  std::map<std::string, thesmos::holdings::Holding> holdings;     // what the community holds, by identifier
  std::vector<Access> accesses;                                   // every access admitted, in order
  archeion::fixity::Journal journal;                              // the seals
  std::vector<Act> acts;                                          // the acts admitted, in order, as written
  std::vector<Act> readings;                                      // each act's institutional reading: its reduct to a kind of the ground
  std::set<std::string> lapsed;                                   // commitments handed over by the lapse a trigger rule declares
  // Presentation and recognition (decision 0009).
  std::string community;                                          // the community's identifier: its founding's, else its constitution's name
  bool community_declared = false;
  std::set<std::string> declared;                                 // parties declared by acts (a founding, a presentation)
  std::map<std::string, std::string> recognitions;                // another community -> the level at which this one recognises it
  std::map<std::pair<std::string, std::string>, std::string> recognised_for;  // (community, matter) -> level, for a matter
  std::map<std::string, std::vector<passports::Head>> heads;      // another community -> the heads of it the office of status reported
  struct Foreign {
    std::string home, status = "presented";                       // presented, standing, revoked or suspended, as last reported
    long long since = 0, reported = -1;                           // the turn of its presentation and of the last report of its status
  };
  std::map<std::string, Foreign> foreign;                         // a mandate of another community's chain, presented here
  std::map<std::string, std::map<std::string, long long>> window; // foreign mandate -> resource -> charged since its status was last reported
  std::map<std::string, std::string> presented;                   // holder -> the digest of the passport it presented
  // Disclosure agreements (decision 0010).
  struct Agreement {
    disclosure::Terms terms;
    std::string digest;                                           // of the terms both sides ratified
    long long opened = 0, principal = -1, community = -1;         // the turn of the first ratification, and of each side's
    long long denounced = -1;                                     // the turn of its denunciation, if any
    std::string denounced_by;
  };
  std::map<std::string, Agreement> agreements;                    // by identifier
  std::map<std::string, std::string> records;                     // the holding that keeps an agreement's record -> the agreement
  // Posted work (decision 0012).
  std::map<std::string, work::Item> work_items;                   // by identifier
  std::map<std::string, std::string> work_of;                     // a commitment that takes, or offers to take, a work item -> the item
  // Items, routes and rulings (decision 0011).
  std::map<std::string, Item> items;                              // by identifier
  std::map<std::string, Ruling> rulings;                          // the register of rulings, by the question each answered

  State() = default;
  State(Parties p, Constitution c) : parties(std::move(p)), rules(std::move(c)) { community = rules.name; }

  bool holds(const std::string& party, const std::string& position) const { return positions.count({party, position}) > 0; }
  bool member(const std::string& party, const std::string& body) const {
    const auto b = rules.bodies.find(body);
    if (b == rules.bodies.end()) return false;
    for (const auto& p : b->second.of)
      if (holds(party, p)) return true;
    return false;
  }
  std::string head() const { return journal.head(); }
};

// The turn from which an agreement stands in force (both sides ratified), or -1; the turn at which it ends (its last turn passed, or its
// denunciation's notice run), or -1 when nothing ends it; and whether it is in force at a turn (Definition 48).
inline long long in_force_since(const State::Agreement& g) { return g.principal >= 0 && g.community >= 0 ? std::max(g.principal, g.community) : -1; }
inline long long ends_at(const State::Agreement& g) {
  long long e = g.terms.to >= 0 ? g.terms.to + 1 : -1;
  if (g.denounced >= 0) e = e < 0 ? g.denounced + g.terms.notice : std::min(e, g.denounced + g.terms.notice);
  return e;
}
inline bool in_force(const State::Agreement& g, long long turn) {
  const long long since = in_force_since(g), end = ends_at(g);
  return since >= 0 && turn >= since && (end < 0 || turn < end);
}

// The route for a type of item (decision 0011): as the constitution's routes declare it, or, where it declares none, read from what it
// declares: questions and declared types to the route for claims, applications by the boundary rule; an empty route reaches no body.
inline constitutions::Route route_of(const Constitution& c, const std::string& type) {
  const auto& r = c.routes;
  if (r.declared) {
    if (type == "application") return r.applications;
    if (type == "question") return r.questions;
    const auto it = r.types.find(type);
    if (it != r.types.end()) return it->second;
    return {};
  }
  constitutions::Route d;
  d.declared = true;
  if (type == "application") return d;
  d.body = c.claims.route, d.review = c.claims.review, d.empty = c.claims.route.empty();
  return d;
}
// The route for a petition of a kind: as declared, or, where the constitution declares no routes, its operational amendment.
inline std::optional<constitutions::PetitionRoute> petition_route(const Constitution& c, const std::string& kind) {
  if (!c.routes.declared) return constitutions::PetitionRoute{"amendment", {}, "operational", {}, {}};
  const auto it = c.routes.petitions.find(kind);
  if (it == c.routes.petitions.end()) return std::nullopt;
  return it->second;
}
// The decision that adopts a petition on a decision route, or nothing.
inline std::string petition_decision(const Constitution& c, const constitutions::PetitionRoute& q) {
  if (q.by == "amendment") {
    const auto it = c.amendment.by_level.find(q.level.empty() ? "operational" : q.level);
    return it == c.amendment.by_level.end() ? std::string() : it->second;
  }
  if (q.by == "deposit" || q.by == "carried") return {};
  return q.by;
}
// A route's term in turns: a number of turns, or a declared tempo; -1 when it declares none.
inline long long term_of(const Constitution& c, const constitutions::Route& r) {
  if (r.term.empty()) return -1;
  if (r.term.find_first_not_of("0123456789") == std::string::npos && r.term.size() <= 9) return std::stoll(r.term);
  return constitutions::wait(c, r.term);
}
// ---- Posted work as the fold reads it (decision 0012) ---------------------------------------------------------------------------------

// A party's provider (its origin under provision): an operative's declared provider, any other party itself.
inline std::string provider_of(const State& s, const std::string& p) { return s.parties.has(p) ? kernel::origin(s.parties, p, kernel::Criterion::provision) : p; }
// The life an item lives under: its kind's as it stood when the item was posted, so that an amendment judges no past act anew.
inline const work::Kind* kind_of_work(const State&, const work::Item& i) { return &i.life; }
// Whether what must come first is accepted at a turn: every item named, and every item of each kind named that was not withdrawn (at
// least one), accepted.
inline bool first_done(const State& s, const work::Kind& k, const work::Item& i, long long turn) {
  auto accepted = [&](const work::Item& x) {
    const work::Kind* kk = kind_of_work(s, x);
    return kk && work::state(*kk, x, turn) == "accepted";
  };
  for (const auto& id : k.after_items) {
    const auto it = s.work_items.find(id);
    if (it == s.work_items.end() || !accepted(it->second)) return false;
  }
  for (const auto& kind : k.after_kinds) {
    bool any = false;
    for (const auto& [id, x] : s.work_items)
      if (x.kind == kind && id != i.id && !x.withdrawn) {  // a withdrawn item is not waited for
        any = true;
        if (!accepted(x)) return false;
      }
    if (!any) return false;
  }
  return true;
}
// A work item's state at a turn, as its kind declares its life.
inline std::string work_state(const State& s, const work::Item& i, long long turn) {
  const work::Kind* k = kind_of_work(s, i);
  return k ? work::state(*k, i, turn, first_done(s, *k, i, turn)) : std::string("unknown");
}
// The last delivery of an item that did not fail its check, or none.
inline const work::Delivery* last_delivery(const work::Item& i, const std::string& commitment = {}) {
  const work::Delivery* d = nullptr;
  for (const auto& x : i.deliveries)
    if (x.check != "failed" && (commitment.empty() || x.commitment == commitment)) d = &x;
  return d;
}
// Why a party may not take an item of a kind, or nothing: the kind's position, the item's reservation, and for a review the writer of
// the item reviewed, whose provider or origin the taker must not share where the kind says so.
inline std::string ineligible(const State& s, const work::Kind& k, const work::Item& i, const std::string& author, const std::string& who) {
  if (k.who != "anyone" && !s.holds(author, k.who) && !s.holds(who, k.who)) return "takers of " + k.name + " hold the position " + k.who;
  if (!i.eligible.empty() && !i.eligible.count(author) && !i.eligible.count(who)) return "the item " + i.id + " is reserved to others";
  if (k.other_provider || k.other_origin) {
    const auto r = s.work_items.find(i.of);
    if (r == s.work_items.end()) return "the item reviewed, " + i.of + ", is unknown";
    const work::Delivery* d = last_delivery(r->second);
    if (!d) return "the item reviewed has no delivery";
    if (k.other_provider && provider_of(s, author) == d->provider) return "the taker's provider is the writer's, " + d->provider;
    if (k.other_origin && (s.parties.has(author) ? kernel::origin(s.parties, author) : author) == d->origin) return "the taker's origin is the writer's";
  }
  return {};
}

// Whether an item is overdue at a turn: open, with a term, and past it.
inline bool overdue(const Item& i, long long turn) { return i.status == "open" && i.due >= 0 && turn > i.due; }
// The ruling in force on a subject, if any.
inline const Ruling* ruling_on(const State& s, const std::string& subject) {
  for (const auto& [id, r] : s.rulings)
    if (r.in_force && r.subject == subject) return &r;
  return nullptr;
}

// The levels of recognition, each including those before it (decision 0009, after the communities domain's federation): none; that the
// other exists; that its attestations count as evidence; that its envoys' undertakings bind it; that its decisions are honoured.
inline int level_rank(const std::string& level) {
  if (level == "none") return 0;
  if (level == "exists") return 1;
  if (level == "evidence") return 2;
  if (level == "undertakings") return 3;
  if (level == "decisions") return 4;
  return -1;
}
// The level at which this community recognises another: for a matter, the matter's own level where one is set, else the general one.
inline std::string recognition(const State& s, const std::string& of, const std::string& matter = {}) {
  if (!matter.empty())
    if (const auto it = s.recognised_for.find({of, matter}); it != s.recognised_for.end()) return it->second;
  const auto it = s.recognitions.find(of);
  return it == s.recognitions.end() ? std::string("none") : it->second;
}

// The party that answers for an act: the root principal of its mandate, or its author.
inline std::string bearer(const State& s, const Act& a) {
  if (!a.mandate.empty() && s.mandates.count(a.mandate)) return mandates::root_principal(s.mandates, a.mandate);
  return a.author;
}

// ---- Holdings as the fold sees them -----------------------------------------------------------------------------------------------

// A party's origin under control, or the party itself when it is not declared.
inline std::string origin_of(const State& s, const std::string& party) { return s.parties.has(party) ? kernel::origin(s.parties, party) : party; }

// The holdings a holding is kept within, nearest first.
inline std::vector<std::string> containers(const State& s, std::string within) {
  std::vector<std::string> out;
  std::set<std::string> seen;
  while (!within.empty() && seen.insert(within).second) {
    out.push_back(within);
    const auto it = s.holdings.find(within);
    if (it == s.holdings.end()) break;
    within = it->second.within;
  }
  return out;
}

// What of a metered holding is available at a turn: its quantity less what is lent and not yet due back.
inline long long available(const State& s, const std::string& id, long long turn) {
  const auto it = s.holdings.find(id);
  if (it == s.holdings.end()) return 0;
  long long lent = 0;
  for (const Access& x : s.accesses)
    if (x.holding == id && x.aim == "lend" && x.until >= turn) lent += x.amount;
  return it->second.quantity - lent;
}

inline holdings::Target target_of(const State& s, const holdings::Holding& h) {
  holdings::Target t;
  t.id = h.id, t.kind = h.kind, t.custodian = h.custodian, t.office = h.office, t.depositor = h.depositor;
  t.within = containers(s, h.within);
  t.metered = h.metered(), t.unit = h.unit, t.visibility = h.visibility;
  return t;
}

// What the record says of a party about to act at a turn: the party that answers for it, the positions it (or that party) and its
// origin hold, the mandate it acts under, the standards under which the record attests its need (the last assessment of it under
// each criterion by a party of another origin, when that assessment says "in need"), and what the party that answers drew under
// each norm of access in the turn.
inline holdings::Situation situation(const State& s, const std::string& author, const std::string& mandate, long long turn) {
  holdings::Situation x;
  x.party = x.bearer = author;
  if (!mandate.empty()) {
    if (const auto m = s.mandates.find(mandate); m != s.mandates.end()) {
      x.bearer = mandates::root_principal(s.mandates, mandate);
      x.mandated = true;
      x.mandate_kinds = m->second.kinds, x.mandate_matters = m->second.matters;
    }
  }
  if (s.parties.has(author)) x.kind = s.parties.at(author).kind;
  x.origin = origin_of(s, author);
  for (const auto& [key, since] : s.positions) {
    if (key.first == author) x.positions.insert(key.second);
    else if (key.first == x.bearer) x.bearer_positions.insert(key.second);
    if (key.first == x.origin) x.origin_positions.insert(key.second);
  }
  std::map<std::string, std::string> last;
  const std::string of = origin_of(s, x.bearer);
  for (const Assessment& a : s.assessments)
    if ((a.object == x.bearer || a.object == author) && origin_of(s, a.bearer) != of && origin_of(s, a.by) != of) last[a.criterion] = a.value;
  for (const auto& [criterion, value] : last)
    if (value == "in need") x.needs.insert(criterion);
  for (const Access& a : s.accesses)
    if (a.turn == turn && a.bearer == x.bearer) x.used[a.norm] += a.counted;
  return x;
}

namespace detail {

inline bool is_access(const Act& a) {
  return a.kind == "consult" || a.kind == "deposit" || a.kind == "modify" || a.kind == "lend" || a.kind == "withdraw" || (a.kind == "use" && a.has("holding"));
}
inline bool whole_number(const std::string& v, long long& out) {
  if (v.empty() || v.size() > 18) return false;
  std::size_t k = v[0] == '-' ? 1 : 0;
  if (k == v.size()) return false;
  for (std::size_t i = k; i < v.size(); i++)
    if (v[i] < '0' || v[i] > '9') return false;
  out = std::stoll(v);
  return true;
}

// What an access draws from a metered holding under the act's own mandate: charged along the chain in the holding's unit (G4).
inline std::optional<acts::Charge> drawn(const State& s, const Act& a) {
  if (a.mandate.empty() || !is_access(a)) return std::nullopt;
  const auto aim = holdings::aim_named(a.kind);
  if (!aim || !holdings::draws(*aim)) return std::nullopt;
  const auto it = s.holdings.find(a.field("holding"));
  long long amount = 0;
  if (it == s.holdings.end() || !it->second.metered() || !whole_number(a.field("amount"), amount) || amount <= 0) return std::nullopt;
  return acts::Charge{a.mandate, it->first, amount};
}

// Whether a declaration a counts-as rule applies under is made by the constitution in force (decision 0013).
inline bool declares(const Constitution& c, const std::string& declaration) {
  return declaration == "operatives transmit to their provider" && c.holdings.operatives_transmit;
}
// The provider an act's reading reaches (decision 0013): the value of the first counts-as rule to a transmission that applies under a
// declaration the constitution makes, whose category the act, read with what the registry carries of its author, lies in; else nothing.
inline std::string reach_of(const State& s, const Act& a) {
  for (const auto& rule : s.rules.behaviour.counts_as) {
    if (rule.status != "transmission" || rule.where_declared.empty() || !declares(s.rules, rule.where_declared)) continue;
    const behaviour::Conduct* c = s.rules.behaviour.category(rule.conduct);
    if (!c) continue;
    const behaviour::Features f = behaviour::features_of(a, s.parties);
    if (!behaviour::holds(c->where, f)) continue;
    const auto v = f.scalars.find(behaviour::feature_name(rule.value));
    if (v != f.scalars.end() && !v->second.empty()) return v->second;
  }
  return {};
}

// An act of access: what it bears on, whether the holding allows it, and whether the norms of access permit it.
inline std::string access(State& s, const Act& a, const std::string& who, bool apply) {
  using holdings::Aim;
  const Aim aim = *holdings::aim_named(a.kind);
  const std::string& id = a.field("holding");
  if (id.empty()) return "an act of access names its holding";
  long long amount = 0, until = -1;
  if (a.has("amount") && !whole_number(a.field("amount"), amount)) return "an amount is a whole number";
  if (amount < 0) return "an amount is never negative";
  const auto found = s.holdings.find(id);
  holdings::Holding made;  // the holding a deposit makes
  holdings::Target t;
  if (aim == Aim::deposit && found == s.holdings.end()) {
    made.within = a.field("within");
    if (!made.within.empty()) {
      const auto c = s.holdings.find(made.within);
      if (c == s.holdings.end() || c->second.withdrawn) return "a deposit is kept within a holding the community has";
      if (c->second.metered()) return "nothing is kept within a metered holding";
      made.kind = c->second.kind, made.custodian = c->second.custodian, made.office = c->second.office, made.review = c->second.review;
      made.verdicts = c->second.verdicts, made.checks_on = c->second.checks_on;  // decision 0012
      made.pending = !c->second.review.empty();
      made.visibility = c->second.visibility;
    } else
      made.custodian = who, made.office = false;
    if (!a.field("kind").empty()) made.kind = a.field("kind");
    if (made.kind.empty()) return "a deposit names the kind of what it deposits";
    made.id = id, made.unit = a.field("unit"), made.quantity = made.unit.empty() ? 0 : amount;
    made.provenance = {a.field("digest"), a.field("source"), a.field("licence")};
    if (made.metered() ? !made.provenance.digest.empty() && !holdings::is_digest(made.provenance.digest) : !holdings::is_digest(made.provenance.digest))
      return "a deposit names the digest of what it deposits: SHA-256, 64 lower-case hexadecimal digits";
    made.depositor = who, made.deposited_by = a.author, made.since = a.turn;
    t = target_of(s, made);
    t.fresh = true;
    t.depositor.clear();
  } else {
    if (found == s.holdings.end()) return "there is no holding " + id;
    const holdings::Holding& h = found->second;
    if (h.withdrawn) return "the holding " + id + " was withdrawn";
    // An agreement's record (decision 0010): consulted only, while its retention runs, by the principal alone once returned, and by no
    // one once erased.
    if (const auto rec = s.records.find(id); rec != s.records.end()) {
      const State::Agreement& g = s.agreements.at(rec->second);
      if (aim != Aim::consult) return "an epistemic record is consulted, and grows only by the disclosures its agreement covers";
      if (const long long end = ends_at(g); end >= 0 && a.turn >= end) {
        if (g.terms.then == "erased") return "the epistemic record " + id + " was erased when its agreement ended, at turn " + std::to_string(end);
        if (a.turn >= end + g.terms.retention) return "the epistemic record " + id + " was kept until turn " + std::to_string(end + g.terms.retention) + ", when its retention ran out";
        if (g.terms.then == "returned" && who != g.terms.principal && origin_of(s, a.author) != g.terms.principal)
          return "the epistemic record " + id + " was returned to its principal when its agreement ended";
      }
    }
    t = target_of(s, h);
    if (aim == Aim::deposit) {
      if (!h.metered()) return "the holding " + id + " exists: a new version of it is a modification";
      if (amount <= 0) return "a deposit to a metered holding names its amount";
    }
    if (holdings::draws(aim)) {
      if (h.metered()) {
        const long long free = available(s, id, a.turn);
        if (amount <= 0) return "an act that draws on a metered holding names its amount";
        if (amount > free) return "the holding " + id + " has " + std::to_string(free) + " " + h.unit + " available";
      } else if (aim != Aim::withdraw && h.pending)
        return "the holding " + id + " awaits its check by another origin";
    }
    if (aim == Aim::lend) {
      if (a.field("to").empty()) return "a loan names its borrower";
      if (!whole_number(a.field("until"), until) || until < a.turn) return "a loan names its last turn, no earlier than its own";
    }
    if (aim == Aim::modify) {
      if (h.metered()) return "a metered holding changes by deposit, use and withdrawal";
      if (!holdings::is_digest(a.field("digest"))) return "a modification names the digest of the new version: SHA-256, 64 lower-case hexadecimal digits";
      if (a.field("digest") == h.provenance.digest) return "the modification leaves the holding as it was";
    }
  }
  // A counts-as rule may make the act's conduct, read from its envelope, count as a transmission to a service outside the community
  // (decision 0008): where the act declares none, the first rule whose category it lies in names it.
  std::string transmit = a.field("transmit");
  if (transmit.empty())
    for (const auto& rule : s.rules.behaviour.counts_as) {
      const behaviour::Conduct* c = s.rules.behaviour.category(rule.conduct);
      if (rule.status != "transmission" || !c || !rule.where_declared.empty()) continue;
      const behaviour::Features f = behaviour::features_of(a);
      if (!behaviour::holds(c->where, f)) continue;
      const auto v = f.scalars.find(behaviour::feature_name(rule.value));
      if (v != f.scalars.end() && !v->second.empty()) {
        transmit = v->second;
        break;
      }
    }
  const std::string reached = reach_of(s, a);  // decision 0013
  const holdings::Request q{aim, amount, a.field("purpose"), transmit, reached};
  const holdings::Decision d = holdings::decide(s.rules.holdings, t, q, situation(s, a.author, a.mandate, a.turn));
  if (!d.permitted) return d.reason;
  // A deposit that meets a petition on its route (decision 0011): the petition open, met by a deposit within this holding's container.
  Item* petition = nullptr;
  if (a.has("petition")) {
    const auto it = s.items.find(a.field("petition"));
    const auto q = it == s.items.end() ? std::nullopt : petition_route(s.rules, it->second.kind);
    if (it == s.items.end() || it->second.type != "petition" || it->second.status != "open" || !q || q->by != "deposit" || aim != Aim::deposit || a.field("within") != q->within)
      return "the deposit meets no open petition whose route is a deposit within " + a.field("within");
    petition = &it->second;
  }
  if (!apply) return {};
  if (petition) petition->deposit = id;
  if (aim == Aim::deposit && found == s.holdings.end()) s.holdings[id] = made;
  else {
    holdings::Holding& h = s.holdings[id];
    if (aim == Aim::deposit) h.quantity += amount;
    if ((aim == Aim::use || aim == Aim::withdraw) && h.metered()) h.quantity -= amount;
    if (aim == Aim::withdraw && !h.metered()) h.withdrawn = true;
    if (aim == Aim::modify) {
      h.versions.push_back(h.provenance.digest);
      h.provenance.digest = a.field("digest");
      if (!a.field("source").empty()) h.provenance.source = a.field("source");
      if (!a.field("licence").empty()) h.provenance.licence = a.field("licence");
    }
  }
  s.accesses.push_back(Access{a.turn, a.author, who, id, a.kind, a.field("purpose"), d.open ? std::string("open access") : d.norm, amount, holdings::counted(t, q), a.field("to"), until,
                              transmit, reached});
  return {};
}

// The check of a deposit awaiting review: an attestation naming the holding, by a party of another origin than its depositor's.
inline std::string check(State& s, const Act& a, const std::string& who, bool apply) {
  const auto it = s.holdings.find(a.field("holding"));
  if (it == s.holdings.end()) return "there is no holding " + a.field("holding");
  holdings::Holding& h = it->second;
  if (!h.pending) return "the holding " + h.id + " awaits no check";
  const std::string of = origin_of(s, h.depositor), from = origin_of(s, h.deposited_by);
  for (const std::string& p : {a.author, who})
    if (origin_of(s, p) == of || origin_of(s, p) == from) return "a deposit is checked by a party of another origin than its depositor's";
  // Review by another provider, and verdicts that check or only record (decision 0012).
  if (h.review == "another provider") {
    const std::string mine = provider_of(s, a.author);
    if (mine == provider_of(s, h.depositor) || mine == provider_of(s, h.deposited_by)) return "a deposit within " + h.within + " is checked by a party of another provider than its depositor's";
  }
  if (!h.verdicts.empty() || !h.checks_on.empty()) {
    const auto c = s.holdings.find(h.within);
    const std::vector<std::string>& verdicts = !h.verdicts.empty() ? h.verdicts : c != s.holdings.end() ? c->second.verdicts : h.verdicts;
    const std::vector<std::string>& checks = !h.checks_on.empty() ? h.checks_on : c != s.holdings.end() ? c->second.checks_on : h.checks_on;
    const std::string& verdict = a.field("verdict");
    if (std::find(verdicts.begin(), verdicts.end(), verdict) == verdicts.end()) return "an attestation of " + h.id + " gives one of its verdicts";
    if (!checks.empty() && std::find(checks.begin(), checks.end(), verdict) == checks.end()) {  // a review recorded, the deposit still pending
      if (apply) s.assessments.push_back(Assessment{a.author, who, h.id, "review", verdict});
      return {};
    }
  }
  if (apply) {
    h.pending = false, h.checked_by = a.author;
    for (auto& [pid, item] : s.items)  // a petition met by this deposit (decision 0011)
      if (item.type == "petition" && item.status == "open" && item.deposit == h.id) item.status = "met", item.closed = a.turn, item.closed_by = a.author;
  }
  return {};
}

inline std::string padded(long long turn) {
  std::string t = std::to_string(turn);
  return std::string(t.size() < 12 ? 12 - t.size() : 0, '0') + t;
}
inline std::string instant(const Act& a) { return a.when.empty() ? padded(a.turn) : a.when; }

// Whether the author may act: known, an operative only under a mandate it holds, the chain standing, the mandate covering the act's kind
// (as written, or a kind it counts as: an objection is a claim), its matter and its terms.
inline std::string authority(const State& s, const Act& a, const std::vector<std::string>& kinds = {}) {
  if (!s.parties.has(a.author)) return "the author is unknown";
  const auto& party = s.parties.at(a.author);
  if (party.kind == Kind::operative && a.mandate.empty()) return "an operative acts only under a mandate";
  if (a.mandate.empty()) return {};
  const auto it = s.mandates.find(a.mandate);
  if (it == s.mandates.end()) return "the mandate is unknown";
  const Mandate& m = it->second;
  if (m.holder != a.author) return "the mandate is held by another";
  const auto faults = mandates::faults(s.parties, s.mandates, a.mandate, s.revoked);
  if (!faults.empty()) return faults.front();
  bool covered = m.kinds.count(a.kind) > 0;
  for (const auto& k : kinds) covered = covered || m.kinds.count(k) > 0;
  if (!covered) return "the mandate does not cover this kind of act";
  if (a.has("matter") && !m.matters.count(a.field("matter"))) return "the mandate does not cover this matter";
  // A link of another community's chain stands here as its home was last reported, and its term in turns counts its home's turns, so
  // that here its term is its term in instants (decision 0009).
  bool foreign = false;
  for (const Mandate* link : mandates::chain(s.mandates, a.mandate)) {
    const auto f = s.foreign.find(link->id);
    if (f == s.foreign.end()) continue;
    foreign = true;
    if (f->second.status == "revoked") return "the mandate " + link->id + " stands revoked at its home, as last reported here";
    if (f->second.status == "suspended") return "the mandate " + link->id + " is suspended here until its home can be reached";
  }
  if (!foreign && (a.turn < m.from || a.turn > m.to)) return "the mandate is out of its term";
  if (foreign && a.when.empty() && (!m.since.empty() || !m.until.empty())) return "an act under a mandate whose term is stated in instants names its instant";
  if (!a.when.empty() && !mandates::within_instants(m, a.when)) return "the mandate is out of its term in instants";  // decision 0007
  return {};
}

// The kind of directive whose choice rules govern a request of a type (decision 0011): the declared kind that opens items of that type
// (apply, question, petition, or a community's own), else request; so that who may post work and who may apply are ruled apart.
inline std::string directive_kind(const Constitution& c, const std::string& type) {
  for (const auto& d : c.behaviour.declared)
    if (const auto f = d.fixed.find("type"); f != d.fixed.end() && f->second == type && behaviour::primitive_of(c.behaviour, d.kind) == "request") return d.kind;
  return "request";
}

inline std::string permitted(const State& s, const Act& a, const std::string& who) {
  // The principal's side of a disclosure agreement is ratified or denounced under its own rule, never the community's (decision 0010).
  if ((a.kind == "agree" || a.kind == "denounce") && a.field("for") == "principal") return {};
  // An answer to an item is made on the item's route, which says who answers it (decision 0011).
  if (a.kind == "answer" && s.items.count(a.field("id"))) return {};
  // The acts of a work item's life are ruled by its kind, which says who posts, takes, delivers, checks, reviews, accepts and withdraws
  // (decision 0012).
  if ((a.kind == "request" && a.has("work") && !a.has("type")) || ((a.kind == "commit" || a.kind == "offer") && a.has("item")) ||
      ((a.kind == "take" || a.kind == "deliver" || a.kind == "accept") && s.work_of.count(a.field("id"))) || (a.kind == "attest" && a.has("item") && !a.has("holding")) ||
      (a.kind == "assess" && s.work_items.count(a.field("object"))) || (a.kind == "cancel" && a.has("item") && !a.has("id")))
    return {};
  const std::string kind = a.kind == "request" && a.has("type") ? directive_kind(s.rules, a.field("type")) : a.kind;
  if (!constitutions::mentions(s.rules, kind)) return {};
  bool empowered = constitutions::permits(s.rules, "anyone", kind);
  for (const auto& [key, since] : s.positions) {
    if (key.first != a.author && key.first != who) continue;
    if (constitutions::forbids(s.rules, key.second, kind)) return "a rule forbids the holder of " + key.second + " this kind of act";
    empowered = empowered || constitutions::permits(s.rules, key.second, kind);
  }
  return empowered ? std::string() : "no position held permits this kind of act";
}

// Whether a proposal may be decided at `turn`: its decision, rule and unit are declared, it has passed, and its tempo has run.
inline std::string decidable(const State& s, const Proposal& p, long long turn) {
  const auto d = s.rules.aggregation.find(p.decision);
  if (d == s.rules.aggregation.end()) return "the proposal names a decision the constitution lacks";
  const auto unit = procedures::unit_named(d->second.count_by);
  if (!unit) return "the decision " + p.decision + " declares no unit of count";
  const auto rule = procedures::read_rule(d->second.rule);
  if (!rule) return "the rule " + d->second.rule + " is not one the kernel knows";
  if (!procedures::decide(s.parties, p.ballots(), *unit, *rule, d->second.weights).passed) return "the proposal has not passed under the rule in force";
  if (turn < p.opened + constitutions::wait(s.rules, d->second.tempo)) return "the proposal is not ripe: its tempo is " + d->second.tempo;
  return {};
}

// The articles a change names: a JSON array of strings, or one article written plainly (decision 0013).
inline std::optional<std::vector<std::string>> articles_of(const std::string& written) {
  if (written.empty()) return std::vector<std::string>{};
  if (written.front() != '[') return std::vector<std::string>{written};
  try {
    const auto j = symploke::jsonld::parse(written);
    std::vector<std::string> out;
    for (const auto& x : j.items) {
      if (!x.is_string()) return std::nullopt;
      out.push_back(x.text);
    }
    return out;
  } catch (const std::exception&) {
    return std::nullopt;
  }
}

// Applies a change of rules at a turn, in the community named (for the compacts it ratifies); returns why it cannot be applied, or
// nothing.
inline std::string change_rules(Constitution& c, const std::map<std::string, std::string>& change, long long turn = 0, const std::string& community = {}) {
  auto get = [&](const char* k) {
    const auto it = change.find(k);
    return it == change.end() ? std::string() : it->second;
  };
  bool applied = false;
  if (!get("conversion").empty()) {
    constitutions::Conversion k;
    k.from = get("from");
    k.decided_by = get("decided by");
    k.to_position = {get("to position")};
    k.at_least = get("at least").empty() ? 1 : std::stoll(get("at least"));
    c.conversions[get("conversion")] = k;
    c.conversions_declared = true;
    applied = true;
  }
  if (!get("open").empty()) {
    auto& b = c.boundary[get("open")];
    b.has_entry = true;
    b.entry = constitutions::Entry{"open", {}, {}, {}};
    applied = true;
  }
  if (!get("decision").empty()) {
    const auto d = c.aggregation.find(get("decision"));
    if (d == c.aggregation.end()) return "the change names a decision the constitution lacks";
    if (!get("rule").empty()) d->second.rule = get("rule");
    if (!get("count by").empty()) d->second.count_by = get("count by");
    if (!get("weights").empty()) {  // the declared weights replaced (decision 0013): {"<member>": <weight>, ...}
      try {
        const auto j = symploke::jsonld::parse(get("weights"));
        std::vector<std::string> unread;
        auto w = constitutions::detail::weights(&j, unread, "the change");
        if (!unread.empty()) return unread.front();
        d->second.weights = std::move(w);
      } catch (const std::exception& e) {
        return std::string("the change of weights is not JSON: ") + e.what();
      }
    }
    applied = applied || !get("rule").empty() || !get("count by").empty() || !get("weights").empty();
  }
  if (!get("receiving").empty()) {
    c.receiving_per_round = std::stoll(get("receiving"));
    applied = true;
  }
  // Compacts (decision 0013): ratified with their terms and reservations, reservations withdrawn, and the community withdrawn, each by the
  // amendment its own rule decides; a compact ratified by its name alone, as older records did, admits no reservation and is withdrawn
  // with no notice.
  if (!get("compact").empty() && !get("withdraw reservation").empty()) return "a change ratifies a compact or withdraws reservations, not both";
  if (!get("compact").empty()) {
    const std::string& id = get("compact");
    if (constitutions::ratification(c, id, turn)) return "the compact is ratified already";
    compacts::Ratification r;
    r.id = id, r.party = community.empty() ? c.name : community, r.since = turn;
    if (!get("terms").empty()) {
      try {
        r.terms = compacts::read(get("terms"));
      } catch (const std::exception& e) {
        return std::string("the compact's terms are not JSON: ") + e.what();
      }
      if (const auto f = compacts::faults(r.terms); !f.empty()) return "the compact's terms do not stand: " + f.front();
      if (r.terms.id != id) return "the terms are those of the compact " + r.terms.id + ", not of " + id;
      if (!r.terms.parties.empty() && std::find(r.terms.parties.begin(), r.terms.parties.end(), r.party) == r.terms.parties.end())
        return "the compact " + id + " names its parties, and " + r.party + " is none of them";
      r.has_terms = true, r.digest = compacts::digest(r.terms);
    }
    const auto reserved = articles_of(get("reservations"));
    if (!reserved) return "the reservations are an article, or a JSON array of articles";
    for (const auto& x : *reserved) r.reservations.push_back({x, turn, -1});
    if (const auto f = compacts::reservation_faults(r); !f.empty()) return f.front();
    c.compacts.push_back(std::move(r));
    applied = true;
  }
  if (!get("withdraw reservation").empty()) {
    const std::string& id = get("withdraw reservation");
    compacts::Ratification* r = nullptr;
    for (auto it = c.compacts.rbegin(); it != c.compacts.rend() && !r; ++it)
      if (it->id == id && it->stands(turn)) r = &*it;
    if (!r) return "the community has no standing ratification of the compact " + id;
    const auto withdrawn = articles_of(get("reservations"));
    if (!withdrawn || withdrawn->empty()) return "a withdrawal of reservations names the articles withdrawn";
    for (const auto& x : *withdrawn) {
      bool found = false;
      for (auto& q : r->reservations)
        if (q.article == x && q.held(turn)) q.until = turn, found = true;
      if (!found) return "the community holds no reservation of " + x + " to the compact " + id;
    }
    applied = true;
  }
  if (!get("withdraw compact").empty()) {
    const std::string& id = get("withdraw compact");
    compacts::Ratification* r = nullptr;
    for (auto it = c.compacts.rbegin(); it != c.compacts.rend() && !r; ++it)
      if (it->id == id && it->stands(turn)) r = &*it;
    if (!r) return "the community has no standing ratification of the compact " + id;
    if (r->withdrawn >= 0) return "the community is withdrawing from the compact " + id + " already, with effect from turn " + std::to_string(r->effective());
    if (r->has_terms && !r->terms.withdrawal) return "the compact " + id + " admits no withdrawal";
    r->withdrawn = turn;
    applied = true;
  }
  if (!get("work").empty()) {  // kinds of work added or their life amended (decision 0012): {"<kind>": {…}, …}
    try {
      const auto j = symploke::jsonld::parse(get("work"));
      if (!j.is_object() || j.members.empty()) return "the change of work names kinds in an object";
      std::vector<std::string> unread;
      for (const auto& [name, x] : j.members) c.work.kinds[name] = work::read_kind(name, x, unread);
      if (!unread.empty()) return "the change of work does not read: " + unread.front();
      c.work.declared = true;
    } catch (const std::exception& e) {
      return std::string("the change of work is not JSON: ") + e.what();
    }
    applied = true;
  }
  // A position's term and a body's seats (decision 0014), each {"<name>": n}, n a whole number, 0 removing it.
  for (const std::string key : {"term", "seats"}) {
    if (get(key.c_str()).empty()) continue;
    try {
      const auto j = symploke::jsonld::parse(get(key.c_str()));
      if (!j.is_object() || j.members.empty()) return "the change of " + key + " names " + (key == "term" ? "positions" : "bodies") + " in an object";
      for (const auto& [name, x] : j.members) {
        if (!x.is_number() || !(x.number >= 0) || x.number > 1e15 || x.number != static_cast<double>(static_cast<long long>(x.number)))
          return "the change of " + key + " gives " + name + " no whole number";
        const long long n = static_cast<long long>(x.number);
        if (key == "term") {
          const auto b = c.boundary.find(name);
          if (b == c.boundary.end()) return "the change gives a term to " + name + ", which has no boundary rule";
          b->second.term = n, b->second.term_unread.clear();
        } else {
          const auto b = c.bodies.find(name);
          if (b == c.bodies.end()) return "the change gives seats to " + name + ", a body the constitution lacks";
          b->second.seats = n, b->second.seats_unread.clear();
        }
      }
    } catch (const std::exception& e) {
      return "the change of " + key + " is not JSON: " + e.what();
    }
    applied = true;
  }
  if (!get("behaviour").empty()) {  // a first part behaviour, or a revision of the one in force (decision 0008)
    behaviour::Part written;
    try {
      written = behaviour::read_part(std::string_view(get("behaviour")));
    } catch (const std::exception& e) {
      return std::string("the part behaviour is not JSON: ") + e.what();
    }
    const behaviour::Part next = behaviour::in_force(written, true);
    const auto faults = c.behaviour_declared ? behaviour::revision_faults(c.behaviour, next) : behaviour::faults(next);
    if (!faults.empty()) return "the taxonomy is not admitted: " + faults.front();
    c.behaviour_written = written, c.behaviour = next, c.behaviour_declared = true;
    applied = true;
  }
  return applied ? std::string() : "the change names nothing the kernel can apply";
}

// An act that cites an epistemic record uses it (decision 0010): for a purpose the record's agreement admits and never forbids, and only
// after a consult of it for that purpose by the act's author or the party that answers for it (Proposition 25).
inline std::string cited(const State& s, const Act& a, const std::string& who) {
  const std::string& id = a.field("epistemic record");
  const auto rec = s.records.find(id);
  if (rec == s.records.end()) return "the act cites " + id + ", which is no epistemic record of the community";
  const disclosure::Terms& t = s.agreements.at(rec->second).terms;
  const std::string& purpose = a.field("purpose");
  if (purpose.empty() || !t.admits.count(purpose) || t.never.count(purpose))
    return "the agreement " + t.id + " does not admit the use of its record " + (purpose.empty() ? std::string("for no stated purpose") : "for " + purpose);
  for (const Access& x : s.accesses)
    if (x.holding == id && x.aim == "consult" && x.purpose == purpose && (x.author == a.author || x.bearer == who)) return {};
  return "a use of the epistemic record " + id + " for " + purpose + " follows a consult of it for that purpose";
}

// A disclosure agreement ratified for one side (decision 0010): the terms stand and fit the community, a second ratification carries the
// terms of the first, and each side ratifies once, the principal by an act it answers for and the community by an office its choice
// rules empower to agree. When both sides have ratified, the agreement is in force and its record is held with its norms of access.
inline std::string agreement(State& s, const Act& a, const std::string& who, bool apply) {
  const Constitution& rules = s.rules;
  const std::string& side = a.field("for");
  if (side != "principal" && side != "community") return "a side ratifies an agreement for the principal or for the community";
  const disclosure::Terms t = disclosure::read(a.field("terms"));
  if (const auto f = disclosure::faults(t); !f.empty()) return "the agreement's terms do not stand: " + f.front();
  if (t.id != a.field("agreement")) return "the act names the agreement its terms declare, " + t.id;
  if (!s.parties.has(t.principal) || !kernel::answerable(s.parties.at(t.principal).kind)) return "the agreement's principal is a party known here that answers for itself";
  for (const auto& x : t.agents)
    if (!s.parties.has(x) || s.parties.at(x).kind != Kind::operative || origin_of(s, x) != t.principal) return "the agreement covers " + x + ", which is no operative its principal controls";
  if (!rules.positions.count(t.custodian) && !s.parties.has(t.custodian)) return "the agreement's record is kept by " + t.custodian + ", neither an office of the community nor a party it knows";
  if (!t.body.empty() && !rules.bodies.count(t.body)) return "the agreement lets the body " + t.body + " consult its record, and the community declares no such body";
  const std::string digest = disclosure::digest(a.field("terms"));
  const auto it = s.agreements.find(t.id);
  if (it == s.agreements.end()) {
    if (s.holdings.count(t.holding) || s.records.count(t.holding)) return "the agreement's record would be kept as " + t.holding + ", which the community holds already";
  } else {
    const State::Agreement& g = it->second;
    if (g.digest != digest) return "the agreement " + t.id + " stands on other terms: a side ratifies the terms the other ratified";
    if (const long long end = ends_at(g); end >= 0 && a.turn >= end) return "the agreement " + t.id + " has ended";
    if ((side == "principal" ? g.principal : g.community) >= 0) return "the " + side + " has ratified the agreement " + t.id + " already";
  }
  if (side == "principal" && who != t.principal) return "the principal ratifies an agreement for itself, by an act it answers for";
  if (side == "community" && !constitutions::mentions(rules, "agree")) return "an agreement is ratified for the community by an office the choice rules empower to agree";
  if (!apply) return {};
  State::Agreement& g = s.agreements[t.id];
  if (g.digest.empty()) g.terms = t, g.digest = digest, g.opened = a.turn;
  (side == "principal" ? g.principal : g.community) = a.turn;
  if (in_force_since(g) == a.turn) {  // in force from now: the record, held with the norms of access the terms declare
    holdings::Holding h;
    h.id = t.holding, h.kind = "epistemic record", h.custodian = t.custodian, h.office = rules.positions.count(t.custodian) > 0, h.since = a.turn;
    std::vector<std::string> positions;
    if (!t.body.empty())
      for (const auto& p : rules.bodies.at(t.body).of) positions.push_back(p);
    h.visibility = holdings::Visibility{holdings::Visibility::Level::offices, std::set<std::string>(positions.begin(), positions.end())};
    if (h.office) h.visibility.offices.insert(t.custodian);
    if (h.visibility.offices.empty()) h.visibility = holdings::Visibility::members();
    s.holdings[h.id] = h;
    s.records[h.id] = t.id;
    for (auto& n : disclosure::norms(t, positions)) s.rules.holdings.norms.push_back(std::move(n));
  }
  return {};
}

// A disclosure agreement denounced for one side, as its terms allow, while it stands in force: effective after its notice.
inline std::string denunciation(State& s, const Act& a, const std::string& who, bool apply) {
  const std::string& side = a.field("for");
  if (side != "principal" && side != "community") return "a side denounces an agreement for the principal or for the community";
  const auto it = s.agreements.find(a.field("agreement"));
  if (it == s.agreements.end()) return "there is no agreement " + a.field("agreement");
  State::Agreement& g = it->second;
  if (!in_force(g, a.turn)) return "the agreement " + g.terms.id + " is not in force";
  if (g.denounced >= 0) return "the agreement " + g.terms.id + " was denounced already";
  if (g.terms.by != "either" && g.terms.by != side) return "the agreement " + g.terms.id + " is denounced by the " + g.terms.by + " alone";
  if (side == "principal" && who != g.terms.principal) return "the principal denounces an agreement for itself, by an act it answers for";
  if (side == "community" && !constitutions::mentions(s.rules, "denounce")) {
    bool empowered = false;
    for (const auto& [key, since] : s.positions)
      if ((key.first == a.author || key.first == who) && constitutions::permits(s.rules, key.second, "agree")) empowered = true;
    if (!empowered) return "an agreement is denounced for the community by an office the choice rules empower to agree or to denounce";
  }
  if (apply) g.denounced = a.turn, g.denounced_by = side;
  return {};
}

// Whether an applicant meets an entry's conditions on application, and the first that fails (decision 0011).
inline std::string unmet_condition(const State& s, const constitutions::Entry& e, const std::string& position, const std::string& applicant) {
  for (const auto& k : e.conditions) {
    if (k.what == "holders fewer than") {
      long long n = 0;
      for (const auto& [key, since] : s.positions) n += key.second == position;
      if (n >= k.n) return "holders fewer than " + std::to_string(k.n) + ": the position has " + std::to_string(n);
    } else if (k.what == "passport from") {
      bool found = false;
      for (const auto& [id, f] : s.foreign)
        if (f.home == k.value && s.mandates.count(id) && s.mandates.at(id).holder == applicant) found = true;
      if (!found) return "passport from " + k.value + ": the applicant presented none";
    } else if (k.what == "work open of kind") {  // decision 0012
      const long long turn = s.acts.empty() ? 0 : s.acts.back().turn;
      bool open = false;
      for (const auto& [id, i] : s.work_items) open = open || (i.kind == k.value && work_state(s, i, turn) == "open");
      if (!open) return "work open of kind " + k.value + ": none is open";
    } else if (k.what == "work open for the applicant's provider") {
      const long long turn = s.acts.empty() ? 0 : s.acts.back().turn;
      bool open = false;
      for (const auto& [id, i] : s.work_items) {
        const work::Kind* kk = kind_of_work(s, i);
        if (!kk || !kk->other_provider || work_state(s, i, turn) != "open") continue;
        const auto r = s.work_items.find(i.of);
        const work::Delivery* d = r == s.work_items.end() ? nullptr : last_delivery(r->second);
        open = open || (d && provider_of(s, applicant) != d->provider);
      }
      if (!open) return "work open for the applicant's provider: none is open for it";
    }
  }
  return {};
}

// An application (decision 0011): the position's boundary rule decides it at once when its entry is open, by rule, sponsored or after a
// conversion, the act admitted either way and the application closed admitted or declined with the condition that failed; otherwise it
// stays open for the body that decides, the office that assigns, or a draw.
inline std::string application(State& s, const Act& a, const std::string& who, bool apply, Item& item) {
  const Constitution& rules = s.rules;
  const std::string& position = a.field("position");
  const auto b = rules.boundary.find(position);
  if (!rules.positions.count(position) || b == rules.boundary.end() || !b->second.has_entry) return "the position " + position + " is not entered under the boundary rules";
  if (!constitutions::may_hold(rules, position, s.parties.at(a.author).kind)) return "this kind of party may not hold the position " + position;
  if (s.holds(a.author, position)) return "the applicant holds the position already";
  for (const auto& [id, x] : s.items)
    if (x.type == "application" && x.status == "open" && x.author == a.author && x.position == position) return "the applicant has an open application for the position, " + id;
  item.position = position;
  const auto& e = b->second.entry;
  std::string decided, failed;
  if (e.mode == "open") decided = "the rule: open";
  else if (e.mode == "application") {
    failed = unmet_condition(s, e, position, a.author);
    decided = "the rule: on application";
  } else if (e.mode == "sponsored") {
    const std::string& sponsor = a.field("sponsor");
    bool holds = false;
    for (const auto& p : e.among) holds = holds || s.holds(sponsor, p);
    if (!holds || (sponsor != who && sponsor != kernel::origin(s.parties, a.author))) failed = "a holder of a sponsoring position answers for the applicant";
    decided = "the rule: sponsored";
  } else if (e.mode == "after") {
    const auto k = rules.conversions.find(e.after);
    long long discharged = 0;
    for (const auto& [id, c] : s.commitments) discharged += c.debtor == a.author && c.status == Status::discharged;
    if (k == rules.conversions.end() || std::find(k->second.to_position.begin(), k->second.to_position.end(), position) == k->second.to_position.end())
      failed = "the conversion " + e.after + " gives no such position";
    else if (discharged < k->second.at_least)
      failed = "the conversion " + e.after + " asks " + std::to_string(k->second.at_least) + " commitments discharged, and the applicant has " + std::to_string(discharged);
    decided = "the rule: after the conversion " + e.after;
  } else if (e.mode == "lot") {
    bool eligible = e.among.empty();
    for (const auto& p : e.among) eligible = eligible || s.holds(a.author, p) || s.holds(who, p);
    if (!eligible) {
      failed = "the lot is drawn among holders of a position the applicant does not hold";
      decided = "the rule: by lot";
    }
  } else if (e.mode != "decided" && e.mode != "assigned")
    return "entry to " + position + " is " + e.mode + ", which takes no application";
  if (!decided.empty() && failed.empty()) failed = constitutions::beyond_seats(s.positions, rules, {{a.author, position}});  // decision 0014
  if (!apply) return {};
  if (!decided.empty()) {
    item.status = failed.empty() ? "admitted" : "declined", item.closed = a.turn, item.closed_by = decided, item.finding = failed;
    if (failed.empty()) s.positions[{a.author, position}] = a.turn;
  }
  return {};
}

// A request that names a type opens an item on its route (decision 0011).
inline std::string open_item(State& s, const Act& a, const std::string& who, bool apply) {
  const Constitution& rules = s.rules;
  const std::string &type = a.field("type"), &id = a.field("id");
  if (id.empty() || s.items.count(id) || s.claims.count(id)) return "an item names an identifier of its own";
  Item item;
  item.id = id, item.type = type, item.author = a.author, item.bearer = who, item.opened = a.turn, item.place = s.acts.size();
  if (type == "application") {
    const constitutions::Route r = route_of(rules, type);
    if (r.empty) return "the community declares no route for applications";
    item.due = term_of(rules, r) < 0 ? -1 : a.turn + term_of(rules, r);
    if (const std::string why = application(s, a, who, apply, item); !why.empty()) return why;
  } else if (type == "question") {
    const constitutions::Route r = route_of(rules, type);
    if (!r.declared || r.empty || r.body.empty()) return "the community declares no route for questions";
    item.about = a.field("about"), item.subject = a.field("subject");
    if (item.about.empty() || item.subject.empty()) return "a question names what it is about and its subject";
    if (!r.subjects.empty() && std::find(r.subjects.begin(), r.subjects.end(), item.about) == r.subjects.end())
      return "questions here are about " + [&] {
        std::string out;
        for (const auto& x : r.subjects) out += (out.empty() ? "" : ", ") + x;
        return out;
      }() + ", not " + item.about;
    if (const Ruling* in_force = ruling_on(s, item.subject_key())) {
      if (a.field("reviews") != in_force->id)
        return "the subject is ruled already: ruling " + in_force->id + " reads \"" + in_force->reading + "\"; a claim against it goes to the body that reviews rulings";
      if (r.review.empty()) return "no body reviews rulings here";
      item.reviews = in_force->id;
    } else if (a.has("reviews"))
      return "the question reviews " + a.field("reviews") + ", which is no ruling in force on its subject";
    item.due = term_of(rules, r) < 0 ? -1 : a.turn + term_of(rules, r);
  } else if (type == "petition") {
    item.kind = a.field("kind"), item.about = a.field("about");
    const auto q = petition_route(rules, item.kind);
    if (!q) return "the community declares no route for petitions of kind " + item.kind;
  } else {
    if (!rules.routes.declared) {  // with no routes declared, a type is one a declared kind in force opens
      bool known = false;
      for (const auto& d : rules.behaviour.declared)
        if (const auto f = d.fixed.find("type"); f != d.fixed.end() && f->second == type && behaviour::primitive_of(rules.behaviour, d.kind) == "request") known = true;
      if (!known) return "no type of item " + type + " is declared here";
    }
    const constitutions::Route r = route_of(rules, type);
    if (!r.declared || r.empty || r.body.empty()) return "the community declares no route for items of type " + type;
    item.due = term_of(rules, r) < 0 ? -1 : a.turn + term_of(rules, r);
  }
  if (apply) s.items[id] = item;
  return {};
}

// Whether a party may answer an item: it is on the item's route, and it is not a party to the item (recusal, as for claims).
inline std::string on_route(const State& s, const Item& item, const std::string& author, const std::string& who, const std::string& body) {
  if (body.empty() || (!s.member(author, body) && !s.member(who, body))) return "the author is not on the route of " + item.id;
  for (const std::string& p : {author, who})
    if (p == item.author || p == item.bearer || (s.parties.has(p) && s.parties.has(item.bearer) && kernel::origin(s.parties, p) == kernel::origin(s.parties, item.bearer)))
      return "a party to the item may not answer it";
  return {};
}

// An answer to an item (decision 0011), with its outcome.
inline std::string answer_item(State& s, const Act& a, const std::string& who, bool apply, Item& item) {
  const Constitution& rules = s.rules;
  const std::string& outcome = a.field("outcome");
  if (item.status != "open") return "the item " + item.id + " is closed: " + item.status;
  auto close = [&](const std::string& status) {
    if (!apply) return;
    item.status = status, item.closed = a.turn, item.closed_by = a.author, item.finding = a.field("finding");
  };
  if (item.type == "application") {
    const auto& e = rules.boundary.at(item.position).entry;
    std::string body;
    if (e.mode == "decided") {
      const auto d = rules.aggregation.find(e.decided_by);
      body = d == rules.aggregation.end() ? std::string() : d->second.body;
    }
    if (e.mode == "assigned") {
      if (!s.holds(a.author, e.office) && !s.holds(who, e.office)) return "an application under assignment is answered by the office " + e.office;
    } else if (const std::string why = on_route(s, item, a.author, who, body); !why.empty())
      return why;
    if (outcome != "declined") return "an application is admitted by an admission, and an answer declines it";
    close("declined");
    return {};
  }
  if (item.type == "petition") {
    const auto q = petition_route(rules, item.kind);
    const std::string decision = q ? petition_decision(rules, *q) : std::string();
    std::string body;
    if (q && q->by == "carried") {
      if (!s.holds(a.author, q->carrier) && !s.holds(who, q->carrier)) return "a carried petition is answered by the office that carries it, " + q->carrier;
      if (outcome != "carried" && outcome != "declined") return "a petition carried is answered carried or declined";
      close(outcome);
      return {};
    }
    if (!decision.empty()) {
      const auto d = rules.aggregation.find(decision);
      body = d == rules.aggregation.end() ? std::string() : d->second.body;
    } else if (q && q->by == "deposit") {
      const auto h = s.holdings.find(q->within);
      if (h != s.holdings.end() && h->second.office) {
        if (!s.holds(a.author, h->second.custodian) && !s.holds(who, h->second.custodian)) return "a petition met by deposit is answered by the custodian of " + q->within;
      }
      if (outcome != "declined") return "a petition met by deposit is met by the check of its deposit, and an answer declines it";
      close("declined");
      return {};
    }
    if (const std::string why = on_route(s, item, a.author, who, body); !why.empty()) return why;
    if (outcome == "declined") {
      close("declined");
      return {};
    }
    if (outcome != "adopted") return "a petition is answered adopted or declined";
    const auto pr = s.proposals.find(item.proposal);
    if (pr == s.proposals.end() || pr->second.status != "open") return "a petition is adopted through the proposal that carries it, and none is open";
    if (const std::string why = decidable(s, pr->second, a.turn); !why.empty()) return why;
    if (apply) s.proposals[item.proposal].status = "adopted";
    close("adopted");
    return {};
  }
  // A question, or an item of a declared type.
  const constitutions::Route r = route_of(rules, item.type);
  const std::string body = item.reviews.empty() ? r.body : r.review;
  if (a.has("overrules") && item.reviews.empty()) return "an overruling comes from the body that reviews rulings, on a question that asks it to review";
  if (const std::string why = on_route(s, item, a.author, who, body); !why.empty()) return why;
  if (item.type != "question") {
    if (outcome != "answered" && outcome != "declined" && outcome != "referred") return "an item is answered, declined or referred";
    close(outcome);
    return {};
  }
  if (!item.reviews.empty()) {
    if (outcome == "upheld") {
      close("upheld");
      return {};
    }
    if (outcome != "ruled" || a.field("overrules") != item.reviews) return "a review of the ruling " + item.reviews + " upholds it, or rules and overrules it";
  }
  if (outcome == "ruled") {
    if (a.field("reading").empty()) return "a ruling states its reading";
    const Ruling* standing = ruling_on(s, item.subject_key());
    if (standing && standing->id != a.field("overrules")) return "the subject is ruled already by " + standing->id + ": the answer refers to it, or the review body overrules it";
    if (apply) {
      if (standing) s.rulings[standing->id].in_force = false, s.rulings[standing->id].overruled_by = item.id;
      s.rulings[item.id] = Ruling{item.id, item.subject_key(), a.field("reading"), a.author, standing ? standing->id : std::string(), {}, a.turn, true};
    }
    close("ruled");
    return {};
  }
  if (outcome == "referred") {
    const auto ref = s.rulings.find(a.field("finding"));
    if (ref == s.rulings.end() || !ref->second.in_force || ref->second.subject != item.subject_key()) return "a question is referred to a ruling in force on its subject";
    close("referred");
    return {};
  }
  if (outcome != "answered" && outcome != "declined") return "a question is ruled, referred, answered or declined";
  close(outcome);
  return {};
}

// Who answers an open item on its route (decision 0011): the members of a body, or the holders of an office.
struct Answerer {
  std::string body, office;
};
inline Answerer answerer_of(const State& s, const Item& item) {
  const Constitution& rules = s.rules;
  if (item.type == "application") {
    const auto b = rules.boundary.find(item.position);
    if (b == rules.boundary.end()) return {};
    if (b->second.entry.mode == "assigned") return {{}, b->second.entry.office};
    if (b->second.entry.mode == "decided")
      if (const auto d = rules.aggregation.find(b->second.entry.decided_by); d != rules.aggregation.end()) return {d->second.body, {}};
    return {};
  }
  if (item.type == "petition") {
    const auto q = petition_route(rules, item.kind);
    if (!q) return {};
    if (q->by == "carried") return {{}, q->carrier};
    if (q->by == "deposit") {
      const auto h = s.holdings.find(q->within);
      return h != s.holdings.end() && h->second.office ? Answerer{{}, h->second.custodian} : Answerer{};
    }
    const auto d = rules.aggregation.find(petition_decision(rules, *q));
    return d == rules.aggregation.end() ? Answerer{} : Answerer{d->second.body, {}};
  }
  const constitutions::Route r = route_of(rules, item.type);
  return {item.reviews.empty() ? r.body : r.review, {}};
}
// Whether a party (acting for a bearer) may answer an item: it is open, the party is among those who answer it, and it is no party to it.
inline bool may_answer(const State& s, const Item& item, const std::string& party, const std::string& bearer) {
  if (item.status != "open") return false;
  const Answerer w = answerer_of(s, item);
  const bool on = (!w.body.empty() && (s.member(party, w.body) || s.member(bearer, w.body))) || (!w.office.empty() && (s.holds(party, w.office) || s.holds(bearer, w.office)));
  if (!on) return false;
  for (const std::string& p : {party, bearer})
    if (p == item.author || p == item.bearer || (s.parties.has(p) && s.parties.has(item.bearer) && kernel::origin(s.parties, p) == kernel::origin(s.parties, item.bearer))) return false;
  return true;
}

// ---- The rules of posted work (decision 0012) -----------------------------------------------------------------------------------------

inline std::string post_work(State& s, const Act& a, const std::string& who, bool apply) {
  const Constitution& rules = s.rules;
  if (!rules.work.declared) return "the community declares no kinds of work";
  const auto k = rules.work.kinds.find(a.field("work"));
  if (k == rules.work.kinds.end()) return "no kind of work " + a.field("work") + " is declared";
  const std::string& id = a.field("id");
  if (id.empty() || s.work_items.count(id) || s.items.count(id)) return "a work item names an identifier of its own";
  bool poster = false;
  for (const auto& p : k->second.posted_by) poster = poster || s.holds(a.author, p) || s.holds(who, p);
  if (!poster) return "items of " + k->first + " are posted by holders of a position its kind names";
  if (a.has("spec") && !s.holdings.count(a.field("spec"))) return "the item's specification, " + a.field("spec") + ", is no holding of the community";
  if ((k->second.other_provider || k->second.other_origin) && !s.work_items.count(a.field("of"))) return "an item of " + k->first + " names the item it bears on";
  long long due = -1;
  if (a.has("due") && (!whole_number(a.field("due"), due) || due < a.turn)) return "an item's due turn is a whole number no earlier than its own";
  work::Item i;
  i.id = id, i.kind = k->first, i.life = k->second, i.poster = who, i.spec = a.field("spec"), i.matter = a.field("matter"), i.of = a.field("of"), i.posted = a.turn, i.due = due;
  if (a.has("eligible")) {
    const std::string& e = a.field("eligible");
    try {
      const auto j = symploke::jsonld::parse(e);
      if (j.is_array())
        for (const auto& x : j.items)
          if (x.is_string()) i.eligible.insert(x.text);
    } catch (const std::exception&) {
      i.eligible.insert(e);
    }
    if (i.eligible.empty()) i.eligible.insert(e);
  }
  if (apply) s.work_items[id] = i;
  return {};
}

// A taking: the taker's commitment naming the item (taken by commit), or the taker's offer an office will take (taken by award).
inline std::string take_work(State& s, const Act& a, const std::string& who, bool apply) {
  const auto it = s.work_items.find(a.field("item"));
  if (it == s.work_items.end()) return "there is no work item " + a.field("item");
  work::Item& i = it->second;
  const work::Kind* k = kind_of_work(s, i);
  if (!k) return "the kind of the item is no longer declared";
  if (a.kind == "commit" && k->taken_by != "commit") return "items of " + k->name + " are awarded: the taker offers, and an office takes the offer";
  if (a.kind == "offer" && k->taken_by != "award") return "items of " + k->name + " are taken by the taker's own commitment";
  const std::string& id = a.field("id");
  if (id.empty() || s.commitments.count(id)) return "the commitment exists already";
  if (const std::string st = work_state(s, i, a.turn); st != "open") return "the item " + i.id + " is " + st;
  if (const std::string why = ineligible(s, *k, i, a.author, who); !why.empty()) return why;
  if (!apply) return {};
  Commitment c;
  c.id = id, c.debtor = who, c.creditor = i.poster, c.matter = i.matter.empty() ? i.id : i.matter, c.amount = 1, c.by = a.author;
  c.status = a.kind == "offer" ? Status::conditional : Status::active;
  s.commitments[id] = c;
  s.work_of[id] = i.id;
  if (a.kind == "commit") i.takings.push_back({id, a.author, who, origin_of(s, a.author), provider_of(s, a.author), a.turn, false, false, -1});
  return {};
}

// An office takes a taker's offer on an item: an award.
inline std::string award_work(State& s, const Act& a, const std::string& who, bool apply) {
  const auto c = s.commitments.find(a.field("id"));
  if (c == s.commitments.end() || c->second.status != Status::conditional) return "there is no such offer";
  work::Item& i = s.work_items.at(s.work_of.at(c->first));
  const work::Kind* k = kind_of_work(s, i);
  if (!k) return "the kind of the item is no longer declared";
  bool office = false;
  for (const auto& p : k->posted_by) office = office || s.holds(a.author, p) || s.holds(who, p);
  if (!office) return "an offer on " + i.id + " is taken by an office that posts items of its kind";
  if (const std::string st = work_state(s, i, a.turn); st != "open") return "the item " + i.id + " is " + st;
  if (const std::string why = ineligible(s, *k, i, c->second.by, c->second.debtor); !why.empty()) return why;
  if (apply) {
    c->second.status = Status::active;
    i.takings.push_back({c->first, c->second.by, c->second.debtor, origin_of(s, c->second.by), provider_of(s, c->second.by), a.turn, false, false, -1});
  }
  return {};
}

inline work::Taking* taking_of(State& s, const std::string& commitment) {
  const auto w = s.work_of.find(commitment);
  if (w == s.work_of.end()) return nullptr;
  for (auto& t : s.work_items.at(w->second).takings)
    if (t.commitment == commitment) return &t;
  return nullptr;
}

// A delivery on a taking: while it is taken, or returned within its redelivery.
inline std::string deliver_work(State& s, const Act& a, const std::string& who, bool apply) {
  const auto c = s.commitments.find(a.field("id"));
  if (c == s.commitments.end() || (c->second.status != Status::active && c->second.status != Status::presented)) return "there is no active commitment to perform";
  if (c->second.debtor != who) return "performance is presented by the debtor's side";
  work::Taking* t = taking_of(s, c->first);
  if (!t) return "the offer on the item has not been taken";
  work::Item& i = s.work_items.at(s.work_of.at(c->first));
  const work::Kind* k = kind_of_work(s, i);
  if (!k) return "the kind of the item is no longer declared";
  const std::string st = work::taking_state(*k, i, *t, a.turn);
  if (st == "lapsed") return "the taking lapsed at turn " + std::to_string(t->since + k->term) + ", and the item reopened";
  if (st == "ended") return k->redeliver_within < 0 ? "the delivery was returned and the kind of work declares no redelivery: the item reopened"
                                                     : "the taking has ended: its redelivery ran out, and the item reopened";
  if (st == "delivered") return "the item is delivered and awaits its " + std::string(last_delivery(i, c->first) && last_delivery(i, c->first)->check == "pending" ? "check" : k->review_by == "none" ? "acceptance" : "review");
  if (st == "reviewed" || st == "accepted") return "the item is " + st;
  if (a.field("artefact").empty()) return "a delivery names its artefact";
  if (apply) {
    c->second.status = Status::presented, c->second.artefact = a.field("artefact");
    i.deliveries.push_back({c->first, a.field("artefact"), a.author, origin_of(s, a.author), provider_of(s, a.author), a.turn, k->check.empty() ? "none" : "pending"});
  }
  return {};
}

// The result of a kind's check on the last delivery, entered by the office the kind names: passed, or failed (the item stays taken).
inline std::string check_work(State& s, const Act& a, const std::string& who, bool apply) {
  const auto it = s.work_items.find(a.field("item"));
  if (it == s.work_items.end()) return "there is no work item " + a.field("item");
  const work::Kind* k = kind_of_work(s, it->second);
  if (!k || k->check.empty()) return "the kind of the item declares no check";
  if (!s.holds(a.author, k->checked_by) && !s.holds(who, k->checked_by)) return "the check " + k->check + " is entered by the office " + k->checked_by;
  if (it->second.deliveries.empty() || it->second.deliveries.back().check != "pending") return "no delivery of " + it->first + " awaits its check";
  const std::string& result = a.field("result");
  if (result != "pass" && result != "fail") return "a check's result is pass or fail";
  if (apply) it->second.deliveries.back().check = result == "pass" ? "passed" : "failed";
  return {};
}

// A review of the last delivery: by one the kind's review admits, never from the deliverer's origin, with a verdict of the kind's.
inline std::string review_work(State& s, const Act& a, const std::string& who, bool apply) {
  work::Item& i = s.work_items.at(a.field("object"));
  const work::Kind* k = kind_of_work(s, i);
  if (!k) return "the kind of the item is no longer declared";
  if (k->review_by == "none") return "items of " + k->name + " take no review";
  if (i.deliveries.empty()) return "the item has no delivery to review";
  const std::size_t place = i.deliveries.size() - 1;
  const work::Delivery& d = i.deliveries.back();
  if (d.check == "pending" || d.check == "failed") return "the last delivery has not passed its check";
  for (const auto& r : i.reviews)
    if (r.delivery == place) return "the last delivery is reviewed already";
  if (origin_of(s, a.author) == d.origin || origin_of(s, who) == d.origin) return "a delivery is never reviewed from its deliverer's origin";
  if (k->review_by == "another provider" && provider_of(s, a.author) == d.provider) return "a review by another provider than the writer's, " + d.provider;
  if (k->review_by != "another origin" && k->review_by != "another provider" && !s.holds(a.author, k->review_by) && !s.holds(who, k->review_by))
    return "items of " + k->name + " are reviewed by holders of " + k->review_by;
  const std::string& verdict = a.field("value");
  if (std::find(k->verdicts.begin(), k->verdicts.end(), verdict) == k->verdicts.end()) return "the verdict " + verdict + " is none the kind gives";
  if (apply) {
    i.reviews.push_back({a.author, origin_of(s, a.author), provider_of(s, a.author), verdict, a.turn, place});
    s.assessments.push_back(Assessment{a.author, who, i.id, a.field("criterion"), verdict});
  }
  return {};
}

// The acceptance of a taking: by the kind's office or decision, only after its check and a verdict the kind accepts.
inline std::string accept_work(State& s, const Act& a, const std::string& who, bool apply) {
  const auto c = s.commitments.find(a.field("id"));
  work::Taking* t = taking_of(s, c->first);
  if (!t) return "the offer on the item has not been taken";
  work::Item& i = s.work_items.at(s.work_of.at(c->first));
  const work::Kind* k = kind_of_work(s, i);
  if (!k) return "the kind of the item is no longer declared";
  const std::string st = work::taking_state(*k, i, *t, a.turn);
  const work::Delivery* d = last_delivery(i, c->first);
  if (k->review_by == "none" ? (st != "delivered" || !d || d->check == "pending") : st != "reviewed")
    return "the item " + i.id + " is " + st + (k->review_by == "none" ? "" : ": it is accepted after a verdict its kind accepts");
  const Constitution& rules = s.rules;
  if (rules.positions.count(k->accepted_by)) {
    if (!s.holds(a.author, k->accepted_by) && !s.holds(who, k->accepted_by)) return "items of " + k->name + " are accepted by the office " + k->accepted_by;
  } else {
    const auto dec = rules.aggregation.find(k->accepted_by);
    if (dec == rules.aggregation.end()) return "the kind's acceptance names a decision the constitution lacks";
    if (dec->second.rule == "any one") {
      if (!s.member(a.author, dec->second.body) && !s.member(who, dec->second.body)) return "items of " + k->name + " are accepted by a member of " + dec->second.body;
    } else {
      const auto p = s.proposals.find(a.field("proposal"));
      if (p == s.proposals.end() || p->second.status != "open" || p->second.decision != k->accepted_by) return "items of " + k->name + " are accepted by " + k->accepted_by + ": it needs a proposal";
      if (const std::string why = decidable(s, p->second, a.turn); !why.empty()) return why;
      if (apply) s.proposals[p->first].status = "adopted";
    }
  }
  for (const std::string& p : {a.author, who})  // a party never accepts its own work
    if (p == t->taker || p == t->bearer || origin_of(s, p) == t->origin) return "a taker never accepts its own work";
  if (apply) c->second.status = Status::discharged, t->accepted = true, t->accepted_at = a.turn;
  return {};
}

// A poster withdraws an item no one holds.
inline std::string withdraw_work(State& s, const Act& a, const std::string& who, bool apply) {
  const auto it = s.work_items.find(a.field("item"));
  if (it == s.work_items.end()) return "there is no work item " + a.field("item");
  if (it->second.poster != who && it->second.poster != a.author) return "an item is withdrawn by its poster";
  const std::string st = work_state(s, it->second, a.turn);
  if (st != "open" && st != "waiting") return "the item " + it->first + " is " + st + ": an item is withdrawn while no one holds it";
  if (apply) it->second.withdrawn = true;
  return {};
}

inline std::string effect(State& s, const Act& a, const std::string& who, bool apply) {
  const std::string& kind = a.kind;
  const Constitution& rules = s.rules;
  // Posted work (decision 0012).
  if (kind == "request" && a.has("work") && !a.has("type")) return post_work(s, a, who, apply);
  if ((kind == "commit" || kind == "offer") && a.has("item")) return take_work(s, a, who, apply);
  if (kind == "take" && s.work_of.count(a.field("id"))) return award_work(s, a, who, apply);
  if (kind == "deliver" && s.work_of.count(a.field("id"))) return deliver_work(s, a, who, apply);
  if (kind == "attest" && a.has("item") && !a.has("holding")) return check_work(s, a, who, apply);
  if (kind == "assess" && s.work_items.count(a.field("object"))) return review_work(s, a, who, apply);
  if (kind == "accept" && s.work_of.count(a.field("id")) && s.commitments.count(a.field("id"))) return accept_work(s, a, who, apply);
  if (kind == "cancel" && a.has("item") && !a.has("id")) return withdraw_work(s, a, who, apply);
  if (kind == "request" && a.has("type")) return open_item(s, a, who, apply);  // decision 0011
  if (kind == "answer" && s.items.count(a.field("id"))) return answer_item(s, a, who, apply, s.items[a.field("id")]);
  if (a.has("epistemic record"))
    if (const std::string why = cited(s, a, who); !why.empty()) return why;
  if (kind == "agree") return agreement(s, a, who, apply);
  if (kind == "denounce") return denunciation(s, a, who, apply);
  if (kind == "attest" && a.has("holding")) return check(s, a, who, apply);
  if (is_access(a)) return access(s, a, who, apply);
  // A report by the office of status (decision 0009): a head of another community it has found, and the status of a mandate of another
  // community's chain presented here, as its home showed it. A report by any other party is evidence and nothing more.
  if (kind == "report" && rules.passports.declared && (s.holds(a.author, rules.passports.status_by) || s.holds(who, rules.passports.status_by)) &&
      (a.has("head of") || a.has("mandate status"))) {
    passports::Head h;
    if (a.has("head of")) {
      long long size = 0;
      if (a.field("head of").empty() || !whole_number(a.field("size"), size) || size < 0 || !holdings::is_digest(a.field("root")))
        return "a report of a head names its community, its number of entries and its root, a SHA-256 digest";
      h = {a.field("head of"), static_cast<std::size_t>(size), archeion::fixity::from_hex(a.field("root"))};
    }
    const std::string& id = a.field("mandate status");
    if (a.has("mandate status")) {
      const auto f = s.foreign.find(id);
      if (f == s.foreign.end()) return "the community holds no mandate of another community named " + id;
      const std::string& st = a.field("status");
      if (st != "standing" && st != "revoked" && st != "suspended") return "a report of a mandate's status says it standing, revoked or suspended";
      if (a.has("head of") && a.field("head of") != f->second.home) return "a report of a mandate's status names the head of the mandate's home, " + f->second.home;
    }
    if (apply) {
      if (a.has("head of")) {
        auto& known = s.heads[h.community];
        if (std::none_of(known.begin(), known.end(), [&](const passports::Head& x) { return passports::same(x, h); })) known.push_back(h);
      }
      if (a.has("mandate status")) {
        auto& f = s.foreign[id];
        f.status = a.field("status"), f.reported = a.turn;
        s.window.erase(id);
      }
    }
    return {};
  }
  if (kind == "recognise") {
    const std::string &of = a.field("of"), &level = a.field("level");
    if (of.empty() || level_rank(level) < 0) return "a recognition names the community it recognises and a level: none, exists, evidence, undertakings or decisions";
    if (of == s.community) return "a community does not recognise itself";
    if (!constitutions::mentions(rules, "recognise")) return "recognition is made by an office the choice rules empower to recognise";
    if (apply) {
      if (a.field("matter").empty()) s.recognitions[of] = level;
      else s.recognised_for[{of, a.field("matter")}] = level;
    }
    return {};
  }
  if (kind == "present") return {};  // its declarations are staged before its author's authority is judged, and kept on its admission
  if (kind == "say" || kind == "acknowledge" || kind == "report" || kind == "attest" || kind == "request") return {};

  if (kind == "found") {
    if (!s.journal.entries.empty() || !s.positions.empty()) return "a community is founded once, by the first act of its chronicle";
    if (!kernel::answerable(s.parties.at(a.author).kind)) return "a founder answers for the founding";
    for (const auto& [party, position] : a.holders) {
      if (!rules.positions.count(position)) return "the founding names a position that the constitution lacks";
      if (!s.parties.has(party)) return "the founding names an unknown party";
      if (!constitutions::may_hold(rules, position, s.parties.at(party).kind)) return "the founding gives " + position + " to a kind of party that may not hold it";
    }
    for (const auto& [id, h] : rules.holdings.holdings)
      if (!h.office && !s.parties.has(h.custodian)) return "the holding " + id + " is kept by " + h.custodian + ", a party the community does not know";
    if (const std::string why = constitutions::beyond_seats(s.positions, rules, a.holders); !why.empty()) return why;  // decision 0014
    if (apply) {
      for (const auto& [party, position] : a.holders) s.positions[{party, position}] = a.turn;
      for (auto [id, h] : rules.holdings.holdings) {  // the holdings the constitution declares are held from the founding
        h.since = a.turn;
        s.holdings[id] = h;
      }
    }
    return {};
  }
  if (kind == "join") {
    const std::string& position = a.field("position");
    const auto b = rules.boundary.find(position);
    if (!rules.positions.count(position) || b == rules.boundary.end() || !b->second.has_entry) return "the position is not open";
    if (!constitutions::may_hold(rules, position, s.parties.at(a.author).kind)) return "this kind of party may not hold the position";
    const auto& entry = b->second.entry;
    if (entry.mode == "sponsored") {
      const std::string& sponsor = a.field("sponsor");
      bool holds = false;
      for (const auto& p : entry.among) holds = holds || s.holds(sponsor, p);
      if (!holds || (sponsor != who && sponsor != kernel::origin(s.parties, a.author))) return "the position is entered when a holder of the sponsoring position answers for the entrant";
    } else if (entry.mode != "open")
      return "the position is not open";
    if (const std::string why = constitutions::beyond_seats(s.positions, rules, {{a.author, position}}); !why.empty()) return why;  // decision 0014
    if (apply) s.positions[{a.author, position}] = a.turn;
    return {};
  }
  if (kind == "admit") {
    const std::string &position = a.field("position"), &whom = a.field("whom");
    const auto b = rules.boundary.find(position);
    const std::string mode = b == rules.boundary.end() ? std::string() : b->second.entry.mode;
    if (mode != "decided" && mode != "assigned" && mode != "lot") return "entry to this position is not decided by admission";
    if (!s.parties.has(whom) || !constitutions::may_hold(rules, position, s.parties.at(whom).kind)) return "the party admitted may not hold the position";
    if (const std::string why = constitutions::beyond_seats(s.positions, rules, {{whom, position}}); !why.empty()) return why;  // decision 0014
    // The application an admission closes (decision 0011): open, by the party admitted, for the position.
    Item* applied = nullptr;
    if (a.has("application")) {
      const auto it = s.items.find(a.field("application"));
      if (it == s.items.end() || it->second.type != "application" || it->second.status != "open" || it->second.author != whom || it->second.position != position)
        return "the admission names no open application by " + whom + " for " + position;
      applied = &it->second;
      for (const std::string& p : {a.author, who})  // a party to the application may not decide it (recusal)
        if (p == whom || (s.parties.has(p) && kernel::origin(s.parties, p) == kernel::origin(s.parties, whom))) return "a party to the application may not decide it";
    }
    if (mode == "assigned") {  // an office assigns, with or without an application
      if (!s.holds(a.author, b->second.entry.office) && !s.holds(who, b->second.entry.office)) return "the position " + position + " is assigned by the office " + b->second.entry.office;
      if (s.holds(whom, position)) return "the party holds the position already";
      if (apply) {
        s.positions[{whom, position}] = a.turn;
        if (applied) applied->status = "admitted", applied->closed = a.turn, applied->closed_by = a.author;
      }
      return {};
    }
    if (mode == "lot") {  // a draw from a seed committed before it: the open application whose digest with the seed is least
      const std::string& seed = a.field("seed");
      const std::string committed = archeion::fixity::sha256_hex(seed);
      long long at = -1;
      for (std::size_t k = 0; k < s.acts.size(); k++)
        if (s.acts[k].field("seed digest") == committed && s.acts[k].field("position") == position) at = static_cast<long long>(k);
      if (seed.empty() || at < 0) return "a draw reveals a seed whose digest an earlier act committed for the position";
      bool member = false;
      for (const auto& [key, since] : s.positions) member = member || key.first == a.author || key.first == who;
      if (!member) return "a draw is made by a member of the community";
      // The candidates: the applications for the position opened before the seed was committed, and open still.
      std::string best, winner;
      std::vector<std::string> candidates;
      for (const auto& [id, x] : s.items)
        if (x.type == "application" && x.status == "open" && x.position == position && x.place < static_cast<std::size_t>(at)) {
          candidates.push_back(id);
          const std::string d = archeion::fixity::sha256_hex(seed + "\x1e" + id);
          if (best.empty() || d < best) best = d, winner = id;
        }
      if (winner.empty()) return "no application was open when the seed was committed";
      if (s.items.at(winner).author != whom) return "the draw gives the position to " + s.items.at(winner).author + ", not to " + whom;
      if (apply) {
        s.positions[{whom, position}] = a.turn;
        for (const auto& id : candidates) {
          Item& x = s.items[id];
          x.status = id == winner ? "admitted" : "declined", x.closed = a.turn, x.closed_by = "the draw", x.finding = id == winner ? std::string() : "not drawn";
        }
      }
      return {};
    }
    const auto d = rules.aggregation.find(b->second.entry.decided_by);
    if (d == rules.aggregation.end()) return "the decision of admission is lacking";
    if (d->second.rule == "any one") {
      if (!s.member(a.author, d->second.body) && !s.member(who, d->second.body)) return "the author may not admit to this position";
    } else {
      const auto p = s.proposals.find(a.field("proposal"));
      if (p == s.proposals.end() || p->second.status != "open" || p->second.decision != b->second.entry.decided_by) return "this admission is decided by " + d->first + ": it needs a proposal";
      const auto ch = p->second.change;
      if (ch.count("admit") == 0 || ch.at("admit") != whom || ch.count("position") == 0 || ch.at("position") != position) return "the proposal admits another";
      if (const std::string why = decidable(s, p->second, a.turn); !why.empty()) return why;
      if (apply) s.proposals[p->first].status = "adopted";
    }
    if (apply) {
      s.positions[{whom, position}] = a.turn;
      if (applied) applied->status = "admitted", applied->closed = a.turn, applied->closed_by = a.author;
    }
    return {};
  }
  if (kind == "leave") {
    if (a.field("position") == "all") {  // leaving the community (decision 0011): every position, commitments staying as they are
      bool any = false;
      for (const auto& [key, since] : s.positions) any = any || key.first == a.author;
      if (!any) return "the author holds no position";
      if (apply)
        for (auto it = s.positions.begin(); it != s.positions.end();) it = it->first.first == a.author ? s.positions.erase(it) : std::next(it);
      return {};
    }
    if (!s.holds(a.author, a.field("position"))) return "the position is not held";
    if (apply) s.positions.erase({a.author, a.field("position")});  // commitments stay as they are
    return {};
  }
  if (kind == "mandate") {
    if (!a.grant) return "an act of mandate names the mandate it issues";
    const Mandate& m = *a.grant;
    if (m.principal != a.author) return "a mandate is issued by its principal";
    if (m.id.empty() || s.mandates.count(m.id)) return "the mandate exists already";
    if (m.parent.empty() && !kernel::answerable(s.parties.at(a.author).kind)) return "an operative issues no root mandate";
    if (!s.parties.has(m.holder)) return "the holder of the mandate is unknown";
    if (!m.credential.empty() && !holdings::is_digest(m.credential))
      return "a mandate names its holder's credential by its SHA-256 digest: 64 lower-case hexadecimal digits";
    mandates::Registry trial = s.mandates;
    trial[m.id] = m;
    const auto faults = mandates::faults(s.parties, trial, m.id, s.revoked);
    if (!faults.empty()) return faults.front();
    if (apply) s.mandates[m.id] = m;
    return {};
  }
  if (kind == "revoke") {
    const std::string& id = a.field("which");
    if (!s.mandates.count(id)) return "the mandate is unknown";
    bool principal = false;
    for (const Mandate* link : mandates::chain(s.mandates, id)) principal = principal || link->principal == a.author;
    if (!principal) return "only a principal of the chain revokes";
    if (apply) s.revoked.insert(id);
    return {};
  }
  if (kind == "offer" || kind == "commit") {
    const std::string& id = a.field("id");
    if (id.empty() || s.commitments.count(id)) return "the commitment exists already";
    if (kind == "commit" && a.field("to").empty()) return "a commitment names its creditor";
    if (apply) {
      Commitment c;
      c.id = id;
      c.debtor = who;
      c.creditor = a.field("to");
      c.matter = a.field("matter");
      c.unit = a.field("unit");
      c.amount = a.number("amount", 1);
      c.due = a.number("due", 0);
      c.by = a.author;
      c.status = kind == "offer" ? Status::conditional : Status::active;
      s.commitments[id] = c;
    }
    return {};
  }
  if (kind == "take") {
    const auto it = s.commitments.find(a.field("id"));
    if (it == s.commitments.end() || it->second.status != Status::conditional) return "there is no such offer";
    const std::string& to = it->second.creditor;
    if (!to.empty() && to != who && to != a.author) return "the offer is addressed to another";
    if (apply) it->second.creditor = who, it->second.status = Status::active;
    return {};
  }
  if (kind == "deliver" || kind == "hand over") {
    const auto it = s.commitments.find(a.field("id"));
    if (it == s.commitments.end() || (it->second.status != Status::active && it->second.status != Status::presented)) return "there is no active commitment to perform";
    if (it->second.debtor != who) return "performance is presented by the debtor's side";
    if (apply) {
      it->second.status = kind == "deliver" ? Status::presented : Status::handed_over;
      it->second.artefact = a.field("artefact");
      if (kind == "hand over")
        if (work::Taking* t = taking_of(s, it->first)) t->ended = true;  // decision 0012
    }
    return {};
  }
  if (kind == "assign" && a.has("position") && !a.has("id")) {  // an office ends an assignment (decision 0011)
    const std::string &position = a.field("position"), &whom = a.field("whom");
    const auto b = rules.boundary.find(position);
    if (b == rules.boundary.end() || b->second.exit != "by the office") return "exit from " + position + " is not by the office";
    if (!s.holds(a.author, b->second.exit_office) && !s.holds(who, b->second.exit_office)) return "the position " + position + " is ended by the office " + b->second.exit_office;
    if (!s.holds(whom, position)) return whom + " does not hold " + position;
    if (apply) s.positions.erase({whom, position});
    return {};
  }
  if (kind == "accept" || kind == "release" || kind == "assign") {
    const auto it = s.commitments.find(a.field("id"));
    if (it == s.commitments.end()) return "there is no such commitment";
    Commitment& c = it->second;
    if (c.creditor != who) return "only the creditor's side may do this";
    if (kind == "accept") {
      if (c.status != Status::presented) return "nothing has been presented";
      if (apply) c.status = Status::discharged;
    } else if (kind == "release") {
      if (c.status != Status::active && c.status != Status::presented && c.status != Status::handed_over && c.status != Status::cancelled) return "the commitment is closed";
      if (apply) {
        c.status = Status::released;
        if (work::Taking* t = taking_of(s, c.id)) t->ended = true;  // decision 0012
      }
    } else {
      if (c.status != Status::active && c.status != Status::presented) return "the commitment is closed";
      if (a.field("to").empty()) return "an assignment names the new creditor";
      if (apply) c.creditor = a.field("to");
    }
    return {};
  }
  if (kind == "cancel") {
    const auto it = s.commitments.find(a.field("id"));
    if (it == s.commitments.end() || it->second.debtor != who || (it->second.status != Status::active && it->second.status != Status::conditional)) return "only the debtor cancels an open commitment";
    if (apply) {
      it->second.status = it->second.status == Status::conditional ? Status::withdrawn : Status::cancelled;
      if (work::Taking* t = taking_of(s, it->first)) t->ended = true;  // decision 0012
    }
    return {};
  }
  if (kind == "grant") {
    const std::string& id = a.field("id");
    if (id.empty() || s.grants.count(id)) return "the grant exists already";
    if (apply) s.grants[id] = Grant{who, a.field("to"), a.field("resource"), a.number("ceiling", 0), 0};
    return {};
  }
  if (kind == "use") {
    const auto it = s.grants.find(a.field("id"));
    if (it == s.grants.end()) return "there is no such grant";
    if (it->second.grantor != who) return "use is recorded by the keeper of the resource";
    const long long amount = a.number("amount", 0);
    if (it->second.used + amount > it->second.ceiling) return "the grant is exhausted";
    if (apply) it->second.used += amount;
    return {};
  }
  if (kind == "assess") {
    if (apply) s.assessments.push_back(Assessment{a.author, who, a.field("object"), a.field("criterion"), a.field("value")});
    return {};
  }
  if (kind == "claim") {
    const std::string& id = a.field("id");
    if (id.empty() || s.claims.count(id)) return "the claim exists already";
    if (apply) s.claims[id] = Claim{who, a.field("against"), rules.claims.has_route && !rules.claims.route.empty() ? "open" : "open, with no route declared", {}};
    return {};
  }
  if (kind == "answer") {
    const auto it = s.claims.find(a.field("id"));
    if (it == s.claims.end()) return "there is no such claim";
    if (rules.claims.route.empty() || (!s.member(a.author, rules.claims.route) && !s.member(who, rules.claims.route))) return "the author is not on the route of claims";
    // A body recuses any member who is a party to the claim, or answers for the party it is against (the design record, Section 8.3).
    for (const std::string& p : {a.author, who}) {
      const std::string& claimant = it->second.claimant;
      const std::string& against = it->second.against;
      if (p == claimant || p == against || (s.parties.has(p) && s.parties.has(against) && kernel::origin(s.parties, p) == kernel::origin(s.parties, against)))
        return "a party to the claim may not answer it";
    }
    if (apply) it->second.status = "answered", it->second.finding = a.field("finding");
    return {};
  }
  if (kind == "convert") {
    const auto k = rules.conversions.find(a.field("rule"));
    if (k == rules.conversions.end()) return "no rule in force declares this conversion";
    const std::string& whom = a.field("whom");
    if (!s.parties.has(whom)) return "the party converted is unknown";
    long long discharged = 0;
    for (const auto& [id, c] : s.commitments) discharged += c.debtor == whom && c.status == Status::discharged;
    if (discharged < k->second.at_least) return "the condition of the conversion does not hold";
    for (const auto& p : k->second.to_position)
      if (!constitutions::may_hold(rules, p, s.parties.at(whom).kind)) return "the conversion gives " + p + " to a kind of party that may not hold it";
    {
      std::vector<std::pair<std::string, std::string>> entering;
      for (const auto& p : k->second.to_position) entering.push_back({whom, p});
      if (const std::string why = constitutions::beyond_seats(s.positions, rules, entering); !why.empty()) return why;  // decision 0014
    }
    if (apply)
      for (const auto& p : k->second.to_position) s.positions[{whom, p}] = a.turn;
    return {};
  }
  if (kind == "propose") {
    const std::string& id = a.field("id");
    if (id.empty() || s.proposals.count(id)) return "the proposal exists already";
    std::string decision = a.field("decision");
    if (decision.empty()) {
      const auto level = rules.amendment.by_level.find(a.has("level") ? a.field("level") : "operational");
      if (level == rules.amendment.by_level.end()) return "no rule of amendment is declared at this level";
      decision = level->second;
    }
    if (!rules.aggregation.count(decision)) return "the proposal names a decision the constitution lacks";
    if (a.has("petition")) {  // a proposal that carries a petition, under its route's decision (decision 0011)
      const auto it = s.items.find(a.field("petition"));
      if (it == s.items.end() || it->second.type != "petition" || it->second.status != "open" || !it->second.proposal.empty())
        return "the proposal carries no open petition that awaits one";
      const auto q = petition_route(rules, it->second.kind);
      if (!q || petition_decision(rules, *q) != decision) return "the petition " + it->second.id + " is decided by " + (q ? petition_decision(rules, *q) : std::string("no route")) + ", not by " + decision;
    }
    if (apply) {
      Proposal p;
      p.id = id;
      p.decision = decision;
      p.author = a.author;
      p.opened = a.turn;
      p.petition = a.field("petition");
      if (!p.petition.empty()) s.items[p.petition].proposal = id;
      for (const auto& [k, v] : a.fields)
        if (k.rfind("change.", 0) == 0) p.change[k.substr(7)] = v;
      s.proposals[id] = p;
    }
    return {};
  }
  if (kind == "vote") {
    const auto it = s.proposals.find(a.field("id"));
    if (it == s.proposals.end() || it->second.status != "open") return "there is no open proposal";
    const auto d = rules.aggregation.find(it->second.decision);
    if (d == rules.aggregation.end() || (!s.member(a.author, d->second.body) && !s.member(who, d->second.body))) return "the author is not a member of the body that decides";
    if (apply) {
      it->second.votes[a.author] = a.field("value") == "yes";
      it->second.bearers[a.author] = who;
    }
    return {};
  }
  if (kind == "amend") {
    const auto it = s.proposals.find(a.field("id"));
    if (it == s.proposals.end() || it->second.status != "open") return "there is no open proposal";
    bool amending = false;
    for (const auto& [level, decision] : rules.amendment.by_level) amending = amending || decision == it->second.decision;
    if (!amending) return "the proposal is decided by " + it->second.decision + ", which amends nothing";
    if (const std::string why = decidable(s, it->second, a.turn); !why.empty()) return why;
    Constitution next = rules;
    if (const std::string why = change_rules(next, it->second.change, a.turn, s.community); !why.empty()) return why;
    if (const auto faults = constitutions::admission(next); !faults.empty())
      return "the amendment would leave the constitution inadmissible (condition " + std::to_string(faults.front().condition) + ": " + faults.front().reason + ")";
    if (const std::string why = constitutions::beyond_seats(s.positions, next, {}, true); !why.empty()) return why;  // decision 0014
    if (apply) {
      s.rules = std::move(next);
      Proposal& p = s.proposals[a.field("id")];
      p.status = "adopted";
      if (!p.petition.empty()) {  // the petition it carries is adopted with it (decision 0011)
        Item& item = s.items[p.petition];
        item.status = "adopted", item.closed = a.turn, item.closed_by = a.author;
      }
    }
    return {};
  }
  return "the kind of act has no effect defined";
}

// Whether the constitution declares trigger rules the fold settles (the base's are applied by the ground itself).
inline bool has_triggers(const Constitution& c) {
  for (const auto& g : constitutions::triggers(c))  // the taxonomy's, and those a position's term is shorthand for (decision 0014)
    if (!g.by_ground) return true;
  return false;
}

// The effects by law of the trigger rules in force as the turns pass to `turn` (Definition 34): an active commitment of the matter
// named lapses, handed over, once more turns than declared have passed its due; a position held for more turns than declared ends.
// A function of the state, the rules and the turn alone; "overdue" is a mark read where it is needed (commitments::overdue).
inline void settle(State& s, long long turn) {
  for (const auto& g : constitutions::triggers(s.rules)) {
    if (g.by_ground) continue;
    if (g.then == "lapse")
      for (auto& [id, c] : s.commitments)
        if (c.status == Status::active && c.due > 0 && turn > c.due + g.past_due_by && (g.matter.empty() || c.matter == g.matter)) c.status = Status::handed_over, s.lapsed.insert(id);
    if (g.then == "end position")
      for (auto it = s.positions.begin(); it != s.positions.end();) {
        if (it->first.second == g.position && turn > it->second + g.held_for) it = s.positions.erase(it);
        else ++it;
      }
  }
}

// What an act declares before its author's authority is judged (decision 0009): the parties a founding or a presentation declares, and
// the links of a presented passport's chain, entered on trial and withdrawn when the act is refused, so that a refused act changes
// nothing (invariant 5).
struct Staged {
  std::vector<std::string> added_parties, added_mandates;  // what was not known before, withdrawn on a refusal
  std::vector<std::string> declared;                       // every party the act declares, known before or not
  std::string holder, digest;                              // a presentation's holder and the digest of its passport
};

inline void unstage(State& s, const Staged& st) {
  for (const auto& id : st.added_mandates) s.mandates.erase(id), s.foreign.erase(id);
  for (const auto& id : st.added_parties) s.parties.erase(id);
}

inline std::string stage_party(State& s, Staged& st, const kernel::Party& p) {
  const bool known = s.parties.has(p.id);
  if (const std::string why = s.parties.declare(p); !why.empty()) return why;
  if (!known) st.added_parties.push_back(p.id);
  st.declared.push_back(p.id);
  return {};
}

// A founding may name the community ("community") and declare its parties ("parties": [{"id", "kind", "controller", "provider"}]),
// so that the record holds them and no registry beside it is needed.
inline std::string stage_founding(State& s, const Act& a, Staged& st) {
  if (a.has("community") && a.field("community").empty()) return "a founding that names its community names it";
  if (!a.has("parties")) return {};
  symploke::jsonld::Json doc;
  try {
    doc = symploke::jsonld::parse(a.field("parties"));
  } catch (const std::exception& e) {
    return std::string("the parties a founding declares are not JSON: ") + e.what();
  }
  if (!doc.is_array()) return "the parties a founding declares are a list";
  for (const auto& x : doc.items) {
    const auto* id = x.get("id");
    const auto* kind = x.get("kind");
    const auto k = kind && kind->is_string() ? kernel::kind_named(kind->text) : std::nullopt;
    if (!id || !id->is_string() || !k) return "each party a founding declares has an identifier and a kind: person, organisation, operative or community";
    kernel::Party p{id->text, *k, {}, {}};
    if (const auto* c = x.get("controller"); c && c->is_string()) p.controller = c->text;
    if (const auto* v = x.get("provider"); v && v->is_string()) p.provider = v->text;
    if (const std::string why = stage_party(s, st, p); !why.empty()) return why;
  }
  return {};
}

// A presentation (Definition 41 of the theory note): the passport in the act's field "passport" read and verified against what the
// host's own record knows (the heads its office of status reported, the notaries it recognises at the level of evidence), accepted as
// the constitution's part "passports" declares, its holder and principal declared, and its chain's links registered as mandates of
// another community, each to stand as its home is next reported.
inline std::string stage_presentation(State& s, const Act& a, Staged& st) {
  const auto& px = s.rules.passports;
  if (!px.declared) return "this community admits no passports: its constitution declares none";
  const auto read = passports::read_passport(a.field("passport"));
  if (!read.read) return "the passport does not read: " + read.faults.front();
  const passports::Passport& p = read.passport;
  const Mandate& m = p.mandate();
  if (a.author != p.holder.id) return "a passport is presented by its holder, " + p.holder.id;
  if (a.mandate != m.id) return "a presentation is made under the passport's own mandate, " + m.id;
  if (a.when.empty() && (!m.since.empty() || !m.until.empty())) return "a presentation names its instant, since the passport's term is stated in instants";
  passports::Trust both, witnessed;
  for (const auto& [community, known] : s.heads) {
    for (const auto& h : known) both.homes.push_back(h);
    if (level_rank(recognition(s, community)) >= level_rank("evidence"))
      for (const auto& h : known) both.notaries.push_back(h), witnessed.notaries.push_back(h);
  }
  const passports::Presentation shown{a.when, s.community, {}};
  if (const auto v = passports::verify(p, both, shown); !v.holds()) return "the passport does not hold here: " + v.reason;
  if (px.accepted_from == "witnessed" && !passports::verify(p, witnessed, shown).holds())
    return "this community accepts a passport whose head a notary it recognises at the level of evidence witnessed, and no such notary witnessed this one's";
  if (px.accepted_from == "recognised" && level_rank(recognition(s, p.home)) < level_rank("evidence"))
    return "this community accepts passports from issuers it recognises at the level of evidence, and it does not so recognise " + p.home;
  if (px.accepted_from == "named" && std::find(px.named.begin(), px.named.end(), p.home) == px.named.end()) return "this community does not accept passports issued by " + p.home;
  for (const auto& r : px.require) {
    if (r == "provider" && p.holder.provider.empty()) return "this community requires a passport to declare its holder's provider";
    if (r == "model" && p.model.empty()) return "this community requires a passport to declare its holder's model";
  }
  for (const auto& l : p.chain)
    if (const auto f = s.foreign.find(l.act.grant->id); f != s.foreign.end() && (f->second.status == "revoked" || f->second.status == "suspended"))
      return "the mandate " + f->first + " stands " + f->second.status + " here, as its home was last reported";
  kernel::Party principal{p.principal.id, p.principal.kind, {}, {}};
  if (const std::string why = stage_party(s, st, principal); !why.empty()) return why;
  if (p.holder.kind == Kind::operative && p.holder.controller != p.principal.id && !s.parties.has(p.holder.controller))
    return "the holder's controller, " + p.holder.controller + ", is not known here: its own passport is presented first";
  if (const std::string why = stage_party(s, st, p.holder); !why.empty()) return why;
  for (const auto& l : p.chain) {
    const Mandate& g = *l.act.grant;
    if (const auto known = s.mandates.find(g.id); known != s.mandates.end()) {
      Act x, y;
      x.grant = known->second, y.grant = g;
      if (!s.foreign.count(g.id) || acts::envelope(x) != acts::envelope(y)) return "the mandate " + g.id + " is known here otherwise";
      continue;
    }
    s.mandates[g.id] = g;
    s.foreign[g.id] = State::Foreign{p.home, "presented", a.turn, -1};
    st.added_mandates.push_back(g.id);
  }
  st.holder = p.holder.id, st.digest = archeion::fixity::sha256_hex(a.field("passport"));
  return {};
}

// Withdraws what an act staged unless it is kept.
struct Unstage {
  State& s;
  const Staged& st;
  bool keep = false;
  ~Unstage() {
    if (!keep) unstage(s, st);
  }
};

// One act put to the rules on a state already settled to its turn.
inline Outcome admit(State& s, const Act& a) {
  // An act of a declared kind is read as an act of the kind of the ground it reduces to; the act is kept as written (decision 0008).
  const behaviour::Reduced red = behaviour::reduce(s.rules.behaviour, a);
  if (!red.ok) return {false, red.reason};
  const Act& r = red.act;
  if (a.body.find('\x1f') != std::string::npos) return {false, "the body holds the unit separator"};
  if (a.has("episode") && !holdings::is_digest(a.field("episode"))) return {false, "an act names the episode of conduct that constituted it by its SHA-256 digest"};
  const std::string when = instant(a);
  if (!s.journal.entries.empty() && when < s.journal.entries.back().when) return {false, "the act is earlier than the last in the chronicle"};
  // What a founding or a presentation declares is staged first, and withdrawn if the act is refused (decision 0009).
  Staged staged;
  Unstage guard{s, staged};
  if (r.kind == "found")
    if (const std::string why = stage_founding(s, r, staged); !why.empty()) return {false, why};
  if (r.kind == "present")
    if (const std::string why = stage_presentation(s, r, staged); !why.empty()) return {false, why};
  std::vector<std::string> kinds;
  for (const auto& [d, at] : red.levels) kinds.push_back(d->counts_as);
  if (const std::string why = authority(s, a, kinds); !why.empty()) return {false, why};
  const std::string who = bearer(s, a);
  if (const std::string why = permitted(s, r, who); !why.empty()) return {false, why};
  for (const auto& [d, at] : red.levels) {
    if (const std::string why = permitted(s, at, who); !why.empty()) return {false, why};
    behaviour::Features f = behaviour::features_of(at);
    f.holds = [&](const std::string& p) { return s.holds(a.author, p) || s.holds(who, p); };
    for (const auto& w : d->valid_when)
      if (!behaviour::holds(w, f)) return {false, "the act does not meet a condition of " + d->kind + ": " + w.text};
  }
  if (const std::string why = effect(s, r, who, false); !why.empty()) return {false, why};
  // Charges are made under the act's own mandate, against every link to the root, and within the mandate's slice; what an access
  // draws from a metered holding under a mandate is charged so too (G4).
  std::vector<acts::Charge> charges = a.charges;
  if (const auto c = drawn(s, r)) charges.push_back(*c);
  std::map<std::pair<std::string, std::string>, long long> extra;  // (mandate, resource) -> charged by this act
  // A reserving mandate's ceilings are charged, when it is issued, against the links a charge under its parent reaches (decision 0007;
  // Proposition 12): issued one for each host under one root, the ceilings of its children never pass the root's.
  if (r.kind == "mandate" && r.grant && r.grant->reserves)
    for (const auto& [resource, ceiling] : r.grant->allowance)
      for (const Mandate* link : mandates::reserved_against(s.mandates, *r.grant)) {
        const auto top = link->allowance.find(resource);
        long long& pending = extra[{link->id, resource}];
        const auto u = s.used.find(link->id);
        const long long used = u == s.used.end() || !u->second.count(resource) ? 0 : u->second.at(resource);
        if (top == link->allowance.end() || used + pending + ceiling > top->second)
          return {false, "the allowance of " + resource + " under " + link->id + " has no room for the ceiling " + r.grant->id + " reserves"};
        pending += ceiling;
      }
  for (const acts::Charge& c : charges) {
    if (c.mandate.empty() || c.mandate != a.mandate) return {false, "a charge is made under the act's own mandate"};
    if (c.amount < 0) return {false, "a charge is never negative"};
    const Mandate& own = s.mandates.at(c.mandate);
    long long in_act = 0;
    for (const acts::Charge& d : charges) in_act += d.resource == c.resource ? d.amount : 0;
    if (const auto sl = own.slice.find(c.resource); sl != own.slice.end() && in_act > sl->second)
      return {false, "the charge of " + c.resource + " exceeds the slice of " + c.mandate};
    // Under a mandate of another community's chain, what is charged between two reports of its status is at most its slice, so that a
    // revocation at its home costs this community at most one slice (Proposition 3; decision 0009).
    if (s.foreign.count(c.mandate))
      if (const auto sl = own.slice.find(c.resource); sl != own.slice.end()) {
        const auto w = s.window.find(c.mandate);
        const long long since = w == s.window.end() || !w->second.count(c.resource) ? 0 : w->second.at(c.resource);
        if (since + in_act > sl->second) return {false, "the charge of " + c.resource + " under " + c.mandate + " would pass its slice before its status is next reported"};
      }
    for (const Mandate* link : mandates::charged_links(s.mandates, c.mandate)) {  // the whole chain unless a link of it reserves
      const auto ceiling = link->allowance.find(c.resource);
      long long& pending = extra[{link->id, c.resource}];
      const auto u = s.used.find(link->id);
      const long long used = u == s.used.end() || !u->second.count(c.resource) ? 0 : u->second.at(c.resource);
      if (ceiling == link->allowance.end() || used + pending + c.amount > ceiling->second)
        return {false, "the allowance of " + c.resource + " under " + link->id + " is exhausted"};
      pending += c.amount;
    }
  }
  effect(s, r, who, true);
  for (const auto& [key, amount] : extra) s.used[key.first][key.second] += amount;
  for (const acts::Charge& c : charges)
    if (s.foreign.count(c.mandate)) s.window[c.mandate][c.resource] += c.amount;
  // What the act declared is kept (decision 0009).
  guard.keep = true;
  for (const auto& id : staged.declared) s.declared.insert(id);
  if (r.kind == "found" && r.has("community")) s.community = r.field("community"), s.community_declared = true;
  if (r.kind == "present") s.presented[staged.holder] = staged.digest;
  s.journal.append(archeion::fixity::Entry{when, a.author, a.kind, acts::envelope(a), a.body, {}});
  s.acts.push_back(a);
  s.readings.push_back(r);
  return {true, "admitted"};
}

}  // namespace detail

// Who answers an item, and whether a party may (decision 0011), for the views.
using detail::Answerer;
using detail::answerer_of;
using detail::may_answer;
// The provider an act's reading reaches (decision 0013), for the views.
using detail::reach_of;

// Advances the state by one act, or refuses it with the reason and changes nothing. Where the constitution declares trigger rules, the
// state is first settled to the act's turn, and the settlement is kept only with the act: an institutional effect arises only by an
// admitted act or a declared rule over the record and its turns (invariant G9, Proposition 16), so that a replay settles alike.
//
// Settlement changes only the positions, the commitments and the set of lapsed commitments (detail::settle), and a refused act leaves the
// state as it found it (invariant 5), so those three parts are kept aside and restored on a refusal; the state is not copied whole, so a
// constitution with terms (decision 0014) does not make each step cost the length of the record.
inline Outcome step(State& s, const Act& a) {
  if (!detail::has_triggers(s.rules)) return detail::admit(s, a);
  auto positions = s.positions;
  auto commitments = s.commitments;
  auto lapsed = s.lapsed;
  detail::settle(s, a.turn);
  const Outcome o = detail::admit(s, a);
  if (!o.admitted) s.positions = std::move(positions), s.commitments = std::move(commitments), s.lapsed = std::move(lapsed);
  return o;
}

// The state as the trigger rules in force leave it at a turn, for views between acts (a briefing, a profile); the state itself is
// unchanged.
inline State settled(const State& s, long long turn) {
  State out = s;
  if (detail::has_triggers(s.rules)) detail::settle(out, turn);
  return out;
}

// The digest of everything the fold computes; the journal and the bodies are left out.
inline std::string relational_digest(const State& s) {
  using acts::detail::put;
  std::string t;
  for (const auto& [k, turn] : s.positions) put(t, "position", k.first + "\x1e" + k.second + "\x1e" + std::to_string(turn));
  for (const auto& [id, m] : s.mandates) put(t, "mandate", id);
  for (const auto& id : s.revoked) put(t, "revoked", id);
  for (const auto& [id, c] : s.commitments)
    put(t, "commitment", id + "\x1e" + c.debtor + "\x1e" + c.creditor + "\x1e" + c.matter + "\x1e" + c.unit + "\x1e" + std::to_string(c.amount) + "\x1e" +
                             std::to_string(c.due) + "\x1e" + std::string(commitments::word(c.status)) + "\x1e" + c.artefact + "\x1e" + c.by);
  for (const auto& [id, g] : s.grants) put(t, "grant", id + "\x1e" + g.grantor + "\x1e" + g.holder + "\x1e" + g.resource + "\x1e" + std::to_string(g.ceiling) + "\x1e" + std::to_string(g.used));
  for (const auto& [id, c] : s.claims) put(t, "claim", id + "\x1e" + c.claimant + "\x1e" + c.against + "\x1e" + c.status + "\x1e" + c.finding);
  for (const auto& [id, p] : s.proposals) {
    std::string v = id + "\x1e" + p.decision + "\x1e" + std::to_string(p.opened) + "\x1e" + p.status;
    for (const auto& [k, x] : p.change) v += "\x1e" + k + "=" + x;
    for (const auto& b : p.ballots()) v += "\x1e" + b.voter + "@" + b.bearer + (b.yes ? "+" : "-");
    put(t, "proposal", v);
  }
  for (const auto& x : s.assessments) put(t, "assessment", x.by + "\x1e" + x.bearer + "\x1e" + x.object + "\x1e" + x.criterion + "\x1e" + x.value);
  for (const auto& [id, m] : s.used)
    for (const auto& [r, n] : m) put(t, "used", id + "\x1e" + r + "\x1e" + std::to_string(n));
  for (const auto& [name, k] : s.rules.conversions) put(t, "conversion", name + "\x1e" + std::to_string(k.at_least) + "\x1e" + k.decided_by);
  for (const auto& [name, d] : s.rules.aggregation) {
    std::string v = name + "\x1e" + d.rule + "\x1e" + d.count_by;
    for (const auto& [member, w] : d.weights) v += "\x1e" + member + "=" + w.str();  // decision 0013, only when declared
    put(t, "decision", v);
  }
  for (const auto& [name, b] : s.rules.boundary) put(t, "entry", name + "\x1e" + b.entry.mode);
  // What decision 0014 adds, only when present: a position's term and a body's seats; the order and the external data are read by no fold.
  for (const auto& [name, b] : s.rules.boundary)
    if (b.term > 0) put(t, "term", name + "\x1e" + std::to_string(b.term));
  for (const auto& [name, b] : s.rules.bodies)
    if (b.seats > 0) put(t, "seats", name + "\x1e" + std::to_string(b.seats));
  for (const auto& [id, h] : s.holdings)
    put(t, "holding", id + "\x1e" + h.kind + "\x1e" + h.unit + "\x1e" + std::to_string(h.quantity) + "\x1e" + h.custodian + "\x1e" + (h.office ? "office" : "party") + "\x1e" +
                          h.within + "\x1e" + h.provenance.digest + "\x1e" + h.depositor + "\x1e" + (h.pending ? "pending" : "") + "\x1e" + h.checked_by + "\x1e" +
                          (h.withdrawn ? "withdrawn" : "") + "\x1e" + std::to_string(h.versions.size()) + "\x1e" + holdings::text(h.visibility));
  for (const Access& x : s.accesses)
    put(t, "access", std::to_string(x.turn) + "\x1e" + x.author + "\x1e" + x.bearer + "\x1e" + x.holding + "\x1e" + x.aim + "\x1e" + std::to_string(x.amount) + "\x1e" +
                         x.purpose + "\x1e" + x.norm + "\x1e" + x.to + "\x1e" + std::to_string(x.until) + "\x1e" + x.transmit + (x.reached.empty() ? std::string() : "\x1e" + x.reached));
  // What decision 0008 adds, written only when present, so that every older state keeps its digest.
  if (s.rules.behaviour_declared) put(t, "behaviour", behaviour::canonical(s.rules.behaviour_written));
  for (const auto& id : s.lapsed) put(t, "lapsed", id);
  // What decision 0009 adds, likewise only when present.
  if (s.community_declared) put(t, "community", s.community);
  for (const auto& id : s.declared)
    if (s.parties.has(id)) {
      const auto& p = s.parties.at(id);
      put(t, "declared", id + "\x1e" + std::string(kernel::word(p.kind)) + "\x1e" + p.controller + "\x1e" + p.provider);
    }
  for (const auto& [of, level] : s.recognitions) put(t, "recognition", of + "\x1e" + level);
  for (const auto& [k, level] : s.recognised_for) put(t, "recognition", k.first + "\x1e" + k.second + "\x1e" + level);
  for (const auto& [c, known] : s.heads)
    for (const auto& h : known) put(t, "head", c + "\x1e" + std::to_string(h.size) + "\x1e" + archeion::fixity::hex(h.root));
  for (const auto& [id, f] : s.foreign) put(t, "foreign", id + "\x1e" + f.home + "\x1e" + f.status + "\x1e" + std::to_string(f.since) + "\x1e" + std::to_string(f.reported));
  for (const auto& [id, m] : s.window)
    for (const auto& [r, n] : m) put(t, "window", id + "\x1e" + r + "\x1e" + std::to_string(n));
  for (const auto& [h, d] : s.presented) put(t, "presented", h + "\x1e" + d);
  // What decision 0012 adds, likewise only when present.
  for (const auto& [id, i] : s.work_items) {
    std::string v = id + "\x1e" + i.kind + "\x1e" + i.poster + "\x1e" + i.spec + "\x1e" + i.matter + "\x1e" + i.of + "\x1e" + std::to_string(i.posted) + "\x1e" +
                    std::to_string(i.due) + "\x1e" + (i.withdrawn ? "withdrawn" : "");
    for (const auto& e : i.eligible) v += "\x1e" "eligible " + e;
    for (const auto& x : i.takings)
      v += "\x1e" "taking " + x.commitment + " " + x.taker + " " + x.bearer + " " + std::to_string(x.since) + (x.ended ? " ended" : "") + (x.accepted ? " accepted " + std::to_string(x.accepted_at) : "");
    for (const auto& x : i.deliveries) v += "\x1e" "delivery " + x.commitment + " " + x.artefact + " " + x.by + " " + std::to_string(x.turn) + " " + x.check;
    for (const auto& x : i.reviews) v += "\x1e" "review " + x.by + " " + x.verdict + " " + std::to_string(x.turn) + " " + std::to_string(x.delivery);
    put(t, "work", v);
  }
  for (const auto& [c, id] : s.work_of) put(t, "work of", c + "\x1e" + id);
  // What decision 0011 adds, likewise only when present.
  for (const auto& [id, i] : s.items)
    put(t, "item", id + "\x1e" + i.type + "\x1e" + i.author + "\x1e" + i.bearer + "\x1e" + std::to_string(i.opened) + "\x1e" + std::to_string(i.place) + "\x1e" + std::to_string(i.due) + "\x1e" + i.status + "\x1e" +
                       i.position + "\x1e" + i.about + "\x1e" + i.subject + "\x1e" + i.kind + "\x1e" + i.reviews + "\x1e" + i.deposit + "\x1e" + i.proposal + "\x1e" +
                       std::to_string(i.closed) + "\x1e" + i.closed_by + "\x1e" + i.finding);
  for (const auto& [id, r] : s.rulings)
    put(t, "ruling", id + "\x1e" + r.subject + "\x1e" + r.reading + "\x1e" + r.by + "\x1e" + r.overrules + "\x1e" + r.overruled_by + "\x1e" + std::to_string(r.turn) + "\x1e" +
                         (r.in_force ? "in force" : "overruled"));
  for (const auto& [id, p] : s.proposals)
    if (!p.petition.empty()) put(t, "carries", id + "\x1e" + p.petition);
  // What decision 0013 adds, likewise only when present: a compact ratified with its terms, reservations or withdrawal.
  for (const auto& r : s.rules.compacts)
    if (r.has_terms || !r.reservations.empty() || r.withdrawn >= 0) {
      std::string v = r.id + "\x1e" + r.party + "\x1e" + r.digest + "\x1e" + std::to_string(r.since) + "\x1e" + std::to_string(r.withdrawn);
      for (const auto& q : r.reservations) v += "\x1e" + q.article + " " + std::to_string(q.since) + " " + std::to_string(q.until);
      put(t, "compact", v);
    }
  // What decision 0010 adds, likewise only when present.
  for (const auto& [id, g] : s.agreements)
    put(t, "agreement", id + "\x1e" + g.digest + "\x1e" + std::to_string(g.principal) + "\x1e" + std::to_string(g.community) + "\x1e" + std::to_string(g.denounced) + "\x1e" + g.denounced_by);
  return archeion::fixity::sha256_hex(t);
}

// Replays a record from the declared parties and the constitution at the founding: every act must be admitted again and every
// seal must agree. Fails at the first act that does not, by its place (from 1).
inline apodeixis::kernel::Verdict replay(const Parties& parties, const Constitution& founding, const std::vector<Act>& record,
                                         const archeion::fixity::Journal& seals, State* out = nullptr) {
  using apodeixis::kernel::Verdict;
  const std::string claim = "the record of " + apodeixis::kernel::count(record.size(), "act") + " gives its state again";
  if (record.size() != seals.entries.size()) return Verdict::no(claim, "the record and its seals differ in length");
  State s(parties, founding);
  for (std::size_t k = 0; k < record.size(); k++) {
    const Outcome o = step(s, record[k]);
    if (!o.admitted) return Verdict::no(claim, "act " + std::to_string(k + 1) + " is refused by the rules: " + o.reason, k).with("at", std::to_string(k + 1));
    if (s.journal.entries.back().seal != seals.entries[k].seal) return Verdict::no(claim, "act " + std::to_string(k + 1) + " does not seal as recorded: it, or something before it, was changed", k).with("at", std::to_string(k + 1));
  }
  if (out) *out = s;
  return Verdict::yes(claim, "every act is admitted again and seals as recorded", record.size());
}

// ---- Views --------------------------------------------------------------------------------------------------------------------

// How many commitments of a debtor stand discharged.
inline long long discharged(const State& s, const std::string& debtor) {
  long long n = 0;
  for (const auto& [id, c] : s.commitments) n += c.debtor == debtor && c.status == Status::discharged;
  return n;
}

}  // namespace thesmos::chronicle
