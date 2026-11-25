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

#pragma once

#include <thesmos/acts/acts.hpp>
#include <thesmos/commitments/commitments.hpp>
#include <thesmos/constitutions/constitutions.hpp>
#include <thesmos/procedures/procedures.hpp>

#include <archeion/fixity/journal.hpp>
#include <archeion/fixity/sha256.hpp>

#include <apodeixis/kernel/verdict.hpp>

#include <map>
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
  long long opened = 0;
  std::map<std::string, std::string> change;
  std::map<std::string, bool> votes;
  std::string status = "open";
};
struct Assessment {
  std::string by, bearer, object, criterion, value;
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
  archeion::fixity::Journal journal;                              // the seals
  std::vector<Act> acts;                                          // the acts admitted, in order

  State() = default;
  State(Parties p, Constitution c) : parties(std::move(p)), rules(std::move(c)) {}

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

// The party that answers for an act: the root principal of its mandate, or its author.
inline std::string bearer(const State& s, const Act& a) {
  if (!a.mandate.empty() && s.mandates.count(a.mandate)) return mandates::root_principal(s.mandates, a.mandate);
  return a.author;
}

namespace detail {

inline std::string padded(long long turn) {
  std::string t = std::to_string(turn);
  return std::string(t.size() < 12 ? 12 - t.size() : 0, '0') + t;
}
inline std::string instant(const Act& a) { return a.when.empty() ? padded(a.turn) : a.when; }

inline std::string authority(const State& s, const Act& a) {
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
  if (!m.kinds.count(a.kind)) return "the mandate does not cover this kind of act";
  if (a.has("matter") && !m.matters.count(a.field("matter"))) return "the mandate does not cover this matter";
  if (a.turn < m.from || a.turn > m.to) return "the mandate is out of its term";
  return {};
}

inline std::string permitted(const State& s, const Act& a, const std::string& who) {
  if (!constitutions::mentions(s.rules, a.kind)) return {};
  bool empowered = constitutions::permits(s.rules, "anyone", a.kind);
  for (const auto& [key, since] : s.positions) {
    if (key.first != a.author && key.first != who) continue;
    if (constitutions::forbids(s.rules, key.second, a.kind)) return "a rule forbids the holder of " + key.second + " this kind of act";
    empowered = empowered || constitutions::permits(s.rules, key.second, a.kind);
  }
  return empowered ? std::string() : "no position held permits this kind of act";
}

// Whether a proposal may be decided at `turn`: its decision, rule and unit are declared, it has passed, and its tempo has run.
inline std::string decidable(const State& s, const Proposal& p, long long turn) {
  const auto d = s.rules.aggregation.find(p.decision);
  if (d == s.rules.aggregation.end()) return "the proposal names a decision the constitution lacks";
  const auto unit = procedures::unit_named(d->second.count_by);
  if (!unit) return "the decision " + p.decision + " declares no unit of count";
  if (!procedures::known_rule(d->second.rule)) return "the rule " + d->second.rule + " is not one the kernel knows";
  if (!procedures::passes(procedures::count(s.parties, p.votes, *unit), d->second.rule)) return "the proposal has not passed under the rule in force";
  if (turn < p.opened + constitutions::wait(s.rules, d->second.tempo)) return "the proposal is not ripe: its tempo is " + d->second.tempo;
  return {};
}

// Applies a change of rules; returns why it cannot be applied, or nothing.
inline std::string change_rules(Constitution& c, const std::map<std::string, std::string>& change) {
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
    applied = applied || !get("rule").empty() || !get("count by").empty();
  }
  if (!get("receiving").empty()) {
    c.receiving_per_round = std::stoll(get("receiving"));
    applied = true;
  }
  return applied ? std::string() : "the change names nothing the kernel can apply";
}

inline std::string effect(State& s, const Act& a, const std::string& who, bool apply) {
  const std::string& kind = a.kind;
  const Constitution& rules = s.rules;
  if (kind == "say" || kind == "acknowledge" || kind == "report" || kind == "attest" || kind == "request") return {};

  if (kind == "found") {
    if (!s.journal.entries.empty() || !s.positions.empty()) return "a community is founded once, by the first act of its chronicle";
    if (!kernel::answerable(s.parties.at(a.author).kind)) return "a founder answers for the founding";
    for (const auto& [party, position] : a.holders) {
      if (!rules.positions.count(position)) return "the founding names a position that the constitution lacks";
      if (!s.parties.has(party)) return "the founding names an unknown party";
      if (!constitutions::may_hold(rules, position, s.parties.at(party).kind)) return "the founding gives " + position + " to a kind of party that may not hold it";
    }
    if (apply)
      for (const auto& [party, position] : a.holders) s.positions[{party, position}] = a.turn;
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
    if (apply) s.positions[{a.author, position}] = a.turn;
    return {};
  }
  if (kind == "admit") {
    const std::string &position = a.field("position"), &whom = a.field("whom");
    const auto b = rules.boundary.find(position);
    if (b == rules.boundary.end() || b->second.entry.mode != "decided") return "entry to this position is not decided by admission";
    if (!s.parties.has(whom) || !constitutions::may_hold(rules, position, s.parties.at(whom).kind)) return "the party admitted may not hold the position";
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
    if (apply) s.positions[{whom, position}] = a.turn;
    return {};
  }
  if (kind == "leave") {
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
    }
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
      if (apply) c.status = Status::released;
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
    if (apply) it->second.status = it->second.status == Status::conditional ? Status::withdrawn : Status::cancelled;
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
    if (apply) {
      Proposal p;
      p.id = id;
      p.decision = decision;
      p.author = a.author;
      p.opened = a.turn;
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
    if (apply) it->second.votes[a.author] = a.field("value") == "yes";
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
    if (const std::string why = change_rules(next, it->second.change); !why.empty()) return why;
    if (const auto faults = constitutions::admission(next); !faults.empty())
      return "the amendment would leave the constitution inadmissible (condition " + std::to_string(faults.front().condition) + ": " + faults.front().reason + ")";
    if (apply) {
      s.rules = std::move(next);
      s.proposals[a.field("id")].status = "adopted";
    }
    return {};
  }
  return "the kind of act has no effect defined";
}

}  // namespace detail

// Advances the state by one act, or refuses it with the reason and changes nothing.
inline Outcome step(State& s, const Act& a) {
  if (!acts::force_of(a.kind)) return {false, "the kind of act is unknown"};
  if (a.body.find('\x1f') != std::string::npos) return {false, "the body holds the unit separator"};
  const std::string when = detail::instant(a);
  if (!s.journal.entries.empty() && when < s.journal.entries.back().when) return {false, "the act is earlier than the last in the chronicle"};
  if (const std::string why = detail::authority(s, a); !why.empty()) return {false, why};
  const std::string who = bearer(s, a);
  if (const std::string why = detail::permitted(s, a, who); !why.empty()) return {false, why};
  if (const std::string why = detail::effect(s, a, who, false); !why.empty()) return {false, why};
  // Charges are made under the act's own mandate, against every link to the root, and within the mandate's slice.
  std::map<std::pair<std::string, std::string>, long long> extra;  // (mandate, resource) -> charged by this act
  for (const acts::Charge& c : a.charges) {
    if (c.mandate.empty() || c.mandate != a.mandate) return {false, "a charge is made under the act's own mandate"};
    if (c.amount < 0) return {false, "a charge is never negative"};
    const Mandate& own = s.mandates.at(c.mandate);
    long long in_act = 0;
    for (const acts::Charge& d : a.charges) in_act += d.resource == c.resource ? d.amount : 0;
    if (const auto sl = own.slice.find(c.resource); sl != own.slice.end() && in_act > sl->second)
      return {false, "the charge of " + c.resource + " exceeds the slice of " + c.mandate};
    for (const Mandate* link : mandates::chain(s.mandates, c.mandate)) {
      const auto ceiling = link->allowance.find(c.resource);
      long long& pending = extra[{link->id, c.resource}];
      const auto u = s.used.find(link->id);
      const long long used = u == s.used.end() || !u->second.count(c.resource) ? 0 : u->second.at(c.resource);
      if (ceiling == link->allowance.end() || used + pending + c.amount > ceiling->second)
        return {false, "the allowance of " + c.resource + " under " + link->id + " is exhausted"};
      pending += c.amount;
    }
  }
  detail::effect(s, a, who, true);
  for (const auto& [key, amount] : extra) s.used[key.first][key.second] += amount;
  s.journal.append(archeion::fixity::Entry{when, a.author, a.kind, acts::envelope(a), a.body, {}});
  s.acts.push_back(a);
  return {true, "admitted"};
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
    for (const auto& [voter, yes] : p.votes) v += "\x1e" + voter + (yes ? "+" : "-");
    put(t, "proposal", v);
  }
  for (const auto& x : s.assessments) put(t, "assessment", x.by + "\x1e" + x.bearer + "\x1e" + x.object + "\x1e" + x.criterion + "\x1e" + x.value);
  for (const auto& [id, m] : s.used)
    for (const auto& [r, n] : m) put(t, "used", id + "\x1e" + r + "\x1e" + std::to_string(n));
  for (const auto& [name, k] : s.rules.conversions) put(t, "conversion", name + "\x1e" + std::to_string(k.at_least) + "\x1e" + k.decided_by);
  for (const auto& [name, d] : s.rules.aggregation) put(t, "decision", name + "\x1e" + d.rule + "\x1e" + d.count_by);
  for (const auto& [name, b] : s.rules.boundary) put(t, "entry", name + "\x1e" + b.entry.mode);
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
