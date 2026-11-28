// The briefing: what a party that enters a community learns from the community's record.
//
// An agent that enters a community is told what to do by the community, never by itself: the context of the work lives in the
// record. For a party, acting in its own name or under a mandate, the briefing reads the chronicle's state and gives the
// community's mission; the positions the party holds; the kinds of act the rules and its mandate leave open to it; every holding on
// which a norm of access speaks to it (or which open access opens), with what it may do there and the norm that grants or refuses
// it; the duties the norms of access lay on it; and the open work the record shows (deposits awaiting a check it may give, offers it
// may take, commitments it owes, deliveries awaiting its acceptance, claims it may answer, proposals it may vote on). A leader is an
// office, and the holder of an office is briefed from the same record as anyone. The holdings part equals the enumeration of
// holdings::decide over every holding and aim (the theory note, Proposition 10). Since decision 0008 it also gives the kinds the
// community declares and what each counts as, its categories of conduct, and its rules by law.

#pragma once

#include <thesmos/chronicle/chronicle.hpp>

#include <string>
#include <vector>

namespace thesmos::chronicle {

struct Work {
  std::string kind;  // check, offer, owed, accept, answer, vote
  std::string id, what;
};

struct Briefing {
  std::string party, bearer, mission;
  std::string order;  // the community's order, a label (decision 0014)
  std::vector<std::string> positions;
  std::vector<std::string> acts;                         // kinds of act the choice rules (and the mandate) leave open to it
  std::vector<thesmos::holdings::Target> targets;        // the holdings considered, and the new holdings a deposit would make
  std::vector<thesmos::holdings::Entry> entries;         // each (target, aim) on which a norm speaks or open access applies
  std::vector<thesmos::holdings::Norm> duties;           // norms of access that bind it to act
  std::vector<Work> work;                                // the open work the record shows
  // The part behaviour (decision 0008): the kinds declared here and what each counts as, with its conditions; the categories of
  // conduct the community declares, each said decidable, readable by the fold or analytic; the rules by law that may act on it.
  std::vector<std::string> declared, conduct, by_law;
  // Items, routes and rulings (decision 0011): the items it opened, or that are opened for it, with their status; the rulings in force.
  std::vector<Item> items;
  std::vector<std::string> rulings;
  // Transmission and compacts (decision 0013): the provider the party's readings reach where operatives transmit to their provider, with
  // each holding's entry deciding it; the counts-as rules in force here; the compacts whose ratification stands for the community, each
  // with its reservations and any withdrawal under way.
  std::string reaches;
  std::vector<std::string> counts_as, compacts;
};

// The targets a briefing considers: every holding not withdrawn that the party may know of (by its visibility, for the positions the
// party and the party that answers for it hold), with the aims that apply to it, and for each unmetered holding the new holding a
// deposit within it would make (of its kind); for each kind the constitution declares for deposits, a new holding of that kind kept
// by its depositor.
inline std::vector<thesmos::holdings::Target> targets(const State& s, const std::string& depositor, const std::set<std::string>& positions) {
  using thesmos::holdings::Aim;
  std::vector<thesmos::holdings::Target> out;
  for (const auto& [id, h] : s.holdings) {
    if (h.withdrawn || !thesmos::holdings::sees(h.visibility, positions)) continue;
    auto t = target_of(s, h);
    t.aims = h.metered() ? std::vector<Aim>{Aim::consult, Aim::use, Aim::deposit, Aim::lend, Aim::withdraw}
                         : std::vector<Aim>{Aim::consult, Aim::use, Aim::modify, Aim::lend, Aim::withdraw};
    out.push_back(t);
    if (!h.metered()) {
      auto f = t;
      f.id.clear(), f.depositor.clear();
      f.fresh = true;
      f.within.insert(f.within.begin(), id);
      f.aims = {Aim::deposit};
      out.push_back(f);
    }
  }
  for (const auto& k : s.rules.holdings.kinds) {
    thesmos::holdings::Target f;
    f.kind = k, f.fresh = true, f.custodian = depositor, f.office = false, f.visibility = thesmos::holdings::Visibility::everyone();
    f.aims = {Aim::deposit};
    out.push_back(f);
  }
  return out;
}

// The briefing of a party at a turn (by default the turn of the last act), acting under a mandate or in its own name, with the
// purpose it declares (if any).
inline Briefing brief(const State& s, const std::string& party, const std::string& mandate = {}, const std::string& purpose = {}, long long turn = -1) {
  if (turn < 0) turn = s.acts.empty() ? 0 : s.acts.back().turn;
  Briefing b;
  const auto x = situation(s, party, mandate, turn);
  b.party = party, b.bearer = x.bearer, b.mission = s.rules.purpose, b.order = s.rules.order;
  std::set<std::string> held = x.positions;  // its own positions and those of the party that answers for it, as the choice rules read them
  held.insert(x.bearer_positions.begin(), x.bearer_positions.end());
  b.positions.assign(held.begin(), held.end());
  for (const auto& [kind, force] : acts::ground()) {
    if (!constitutions::mentions(s.rules, kind)) continue;
    bool may = constitutions::permits(s.rules, "anyone", kind), forbidden = false;
    for (const auto& p : held) {
      may = may || constitutions::permits(s.rules, p, kind);
      forbidden = forbidden || constitutions::forbids(s.rules, p, kind);
    }
    if (may && !forbidden && (!x.mandated || x.mandate_kinds.count(kind))) b.acts.push_back(kind);
  }
  // A declared kind is open when the kind of the ground it reduces to is, and, when the choice rules name it, they permit it.
  for (const auto& d : s.rules.behaviour.declared) {
    const std::string p = behaviour::primitive_of(s.rules.behaviour, d.kind);
    const bool ground_open = std::find(b.acts.begin(), b.acts.end(), p) != b.acts.end() || !constitutions::mentions(s.rules, p);
    bool may = !constitutions::mentions(s.rules, d.kind) || constitutions::permits(s.rules, "anyone", d.kind);
    for (const auto& q : held) may = may || constitutions::permits(s.rules, q, d.kind);
    if (ground_open && may && (!x.mandated || x.mandate_kinds.count(d.kind) || x.mandate_kinds.count(p))) b.acts.push_back(d.kind);
  }
  // The part behaviour: what the community declares beyond the ground, read from its constitution as the record leaves it.
  for (const auto& d : s.rules.behaviour.declared) {
    std::string line = d.kind + " counts as " + d.counts_as + " (" + std::string(acts::word(d.force)) + ")";
    for (const auto& w : d.valid_when) line += "; valid when " + w.text;
    b.declared.push_back(line);
  }
  for (const auto& c : s.rules.behaviour.conduct)
    b.conduct.push_back(c.name + (c.analytic ? " (analytic: read in analysis only)" : behaviour::fold_readable(c.where) ? " (decidable, read by the fold)" : " (decidable, read in the journal)"));
  for (const auto& g : constitutions::triggers(s.rules))  // the taxonomy's, and a position's term (decision 0014)
    if (!g.by_ground) b.by_law.push_back(g.rule + ": " + g.then + (g.then == "end position" ? " " + g.position + " held for more than " + std::to_string(g.held_for) + " turns" : std::string()));
  for (const auto& r : s.rules.behaviour.counts_as)
    if (r.where_declared.empty() || detail::declares(s.rules, r.where_declared))
      b.counts_as.push_back(r.rule + ": " + r.conduct + " counts as a " + r.status + " to " + behaviour::feature_name(r.value) +
                            (r.where_declared.empty() ? std::string() : ", as the community declares that " + r.where_declared));
  {
    Act reading;  // what a reading by the party would reach (decision 0013)
    reading.author = party, reading.kind = "consult", reading.turn = turn, reading.mandate = mandate;
    b.reaches = reach_of(s, reading);
  }
  for (const auto& r : s.rules.compacts)
    if (r.stands(turn)) {
      std::string line = "the compact " + r.id + ", ratified at turn " + std::to_string(r.since);
      const auto reserved = r.reserved_at(turn);
      for (std::size_t i = 0; i < reserved.size(); i++) line += (i == 0 ? "; reserving " : ", ") + reserved[i];
      if (r.withdrawn >= 0) line += "; withdrawn at turn " + std::to_string(r.withdrawn) + ", with effect from turn " + std::to_string(r.effective());
      b.compacts.push_back(line);
    }
  b.targets = targets(s, x.bearer, held);
  b.entries = thesmos::holdings::brief(s.rules.holdings, b.targets, x, purpose, b.reaches);
  if (x.mandated)  // the norms may permit what the party's mandate does not cover; the act would then be refused for want of authority
    for (auto& e : b.entries) e.covered = x.mandate_kinds.count(std::string(thesmos::holdings::word(e.aim))) > 0;
  for (std::size_t i : thesmos::holdings::duties(s.rules.holdings, b.targets, x)) b.duties.push_back(s.rules.holdings.norms[i]);
  // The open work the record shows.
  const std::string po = origin_of(s, party), bo = origin_of(s, x.bearer);
  for (const auto& [id, h] : s.holdings)
    if (const std::string of = origin_of(s, h.depositor), from = origin_of(s, h.deposited_by);
        h.pending && thesmos::holdings::sees(h.visibility, held) && po != of && po != from && bo != of && bo != from)
      b.work.push_back({"check", id, "a deposit of " + h.kind + (h.within.empty() ? "" : " within the " + h.within) + " by " + h.depositor + " awaits a check by another origin"});
  for (const auto& [id, c] : s.commitments) {
    if (s.work_of.count(id)) continue;  // a taking of posted work: its own entries below (decision 0012)
    const std::string what = std::to_string(c.amount) + (c.unit.empty() ? "" : " " + c.unit) + (c.matter.empty() ? "" : " on " + c.matter);
    if (c.status == Status::conditional && c.debtor != x.bearer && (c.creditor.empty() || c.creditor == x.bearer || c.creditor == party))
      b.work.push_back({"offer", id, "an offer of " + what + " by " + c.debtor + " awaits taking"});
    if (c.status == Status::active && c.debtor == x.bearer && (c.by == party || party == x.bearer))
      b.work.push_back({"owed", id, "owed to " + c.creditor + ": " + what + (c.due ? ", due at turn " + std::to_string(c.due) : "")});
    if (c.status == Status::presented && c.creditor == x.bearer) b.work.push_back({"accept", id, "a delivery of " + what + " by " + c.debtor + " awaits acceptance"});
  }
  for (const auto& [id, k] : s.claims) {
    if (k.status.rfind("open", 0) != 0 || s.rules.claims.route.empty()) continue;
    if (!s.member(party, s.rules.claims.route) && !s.member(x.bearer, s.rules.claims.route)) continue;
    if (k.claimant == party || k.claimant == x.bearer || k.against == party || k.against == x.bearer) continue;
    b.work.push_back({"answer", id, "a claim by " + k.claimant + " against " + k.against + " awaits an answer"});
  }
  // Items (decision 0011): its own, with their status; those it may answer on their routes; the rulings in force.
  const long long now = s.acts.empty() ? 0 : s.acts.back().turn;
  for (const auto& [id, i] : s.items) {
    if (i.author == party || i.bearer == party || i.author == x.bearer) b.items.push_back(i);
    if (may_answer(s, i, party, x.bearer))
      b.work.push_back({"answer", id, "a" + std::string(i.type == "application" || i.type == "item" ? "n " : " ") + i.type + " by " + i.author +
                                          (i.position.empty() ? "" : " for " + i.position) + (i.subject.empty() ? "" : " on " + i.subject_key()) +
                                          (i.kind.empty() ? "" : " of kind " + i.kind) + " awaits an answer" +
                                          (i.due >= 0 ? (overdue(i, now) ? ", overdue since turn " : ", due at turn ") + std::to_string(i.due) : std::string())});
  }
  for (const auto& [id, r] : s.rulings)
    if (r.in_force) b.rulings.push_back(r.subject + ": " + r.reading + " (ruling " + id + ")");
  // Posted work (decision 0012): what the party may take, what it holds and must deliver, what it may check, review and accept.
  for (const auto& [id, i] : s.work_items) {
    const work::Kind* k = kind_of_work(s, i);
    if (!k) continue;
    const std::string st = work_state(s, i, now);
    if (st == "open" && ineligible(s, *k, i, party, x.bearer).empty() && i.poster != x.bearer)
      b.work.push_back({"take", id, "posted work of kind " + k->name + (i.matter.empty() ? "" : " on " + i.matter) + " is open: take it by " +
                                        (k->taken_by == "commit" ? std::string("a commitment naming it") : std::string("an offer an office takes")) +
                                        (i.due >= 0 ? (now > i.due ? ", overdue since turn " : ", due at turn ") + std::to_string(i.due) : std::string())});
    for (const auto& t : i.takings) {
      const std::string ts = work::taking_state(*k, i, t, now);
      if ((t.taker == party || (t.bearer == x.bearer && party == x.bearer)) && (ts == "taken" || ts == "returned"))
        b.work.push_back({"deliver", t.commitment, "deliver " + id + (ts == "returned" ? ", returned by its review, again" : "") +
                                                       (ts == "taken" && k->term >= 0 ? " before turn " + std::to_string(t.since + k->term) : std::string())});
      const work::Delivery* d = last_delivery(i, t.commitment);
      if (ts == "delivered" && d && d->check == "pending" && (s.holds(party, k->checked_by) || s.holds(x.bearer, k->checked_by)))
        b.work.push_back({"check", id, "the delivery of " + id + " awaits the check " + k->check});
      bool reviewed = false;
      for (const auto& r : i.reviews) reviewed = reviewed || (d && r.delivery + 1 == i.deliveries.size() && &i.deliveries.back() == d);
      const bool may_review = d && !reviewed && d->check != "pending" && k->review_by != "none" && origin_of(s, party) != d->origin &&
                              (k->review_by != "another provider" || provider_of(s, party) != d->provider) &&
                              (k->review_by == "another origin" || k->review_by == "another provider" || s.holds(party, k->review_by) || s.holds(x.bearer, k->review_by));
      if (ts == "delivered" && may_review) b.work.push_back({"review", id, "the delivery of " + id + " awaits a review (" + k->review_by + ")"});
      const bool office = s.rules.positions.count(k->accepted_by) ? s.holds(party, k->accepted_by) || s.holds(x.bearer, k->accepted_by) : false;
      if (office && origin_of(s, party) != t.origin && (ts == "reviewed" || (ts == "delivered" && k->review_by == "none" && d && d->check != "pending")))
        b.work.push_back({"accept", t.commitment, "the work on " + id + " by " + t.taker + " awaits acceptance"});
    }
  }
  for (const auto& [id, p] : s.proposals) {
    if (p.status != "open" || p.votes.count(party)) continue;
    const auto d = s.rules.aggregation.find(p.decision);
    if (d != s.rules.aggregation.end() && (s.member(party, d->second.body) || s.member(x.bearer, d->second.body)))
      b.work.push_back({"vote", id, "a proposal before " + d->second.body + ", decided by " + p.decision + ", awaits its vote"});
  }
  return b;
}

}  // namespace thesmos::chronicle
