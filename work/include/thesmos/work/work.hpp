// Posted work (decision 0012): kinds of work a constitution declares as data, and the life of a work item read from its record.
//
// The part "work" of a constitution's written form declares kinds of work:
//
//   "work": {"kinds": {"section": {
//       "posted by": ["steward"],                       who posts an item of the kind
//       "taken by": "commit" | "award",                 the taker's own commitment, or an office's taking of the taker's offer
//       "takers": 1,                                    how many may take an item at once
//       "eligible": {"who": "writer", "provider other than": "the writer of the item reviewed",
//                    "origin other than": "the writer of the item reviewed"},
//       "term": 6,                                      turns a taking runs before it lapses, unless delivered
//       "deliverable": {"within": "sections", "kind": "document", "check": "section standard", "checked by": "steward"},
//       "review": {"by": "another provider" | "another origin" | "<position>" | "none", "verdicts": ["adopt", "minor", "revise"],
//                  "accepts": ["adopt", "minor"]},
//       "accepted by": "<position>" | "<decision>",
//       "returned": {"redeliver within": 2, "then": "reopen"},
//       "after": {"kinds": ["section"], "items": [], "state": "accepted"}}}}
//
// The life of an item is a state machine read from its acts (the design's Section 8.6), so that every transition is an act of the
// chronicle and every rule of the life is data a community may amend: posted, it is open (or waiting while what must come first is not
// accepted); taken by a commitment on it, it runs until its term and then lapses, reopening the item; delivered with an artefact, it
// awaits the kind's check (a failed check leaves it taken), then the kind's review; a review whose verdict the kind accepts lets the
// office accept it; a verdict outside those returns it, and the taker may deliver again within the declared turns, after which the
// item reopens; a poster withdraws an item no one has taken. A kind that declares no redelivery leaves a returned item to reopen at once.
// An item lives under its kind's life as it stood when the item was posted: an amendment of the life governs the items posted after it,
// and judges no past act anew.
//
// What a kind names of the community (positions, holdings, decisions) is the constitution's to admit (faults); what the record says of
// an item is the chronicle's; this module reads the kinds, names their faults of form, and computes an item's state from its record.

#pragma once

#include <symploke/jsonld/json.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace thesmos::work {

using Json = symploke::jsonld::Json;

struct Kind {
  std::string name;
  std::vector<std::string> posted_by;
  std::string taken_by = "commit";  // "commit" or "award"
  long long takers = 1;
  std::string who = "anyone";       // the position an eligible taker holds, or "anyone"
  bool other_provider = false, other_origin = false;  // than the writer of the item reviewed (field "of")
  long long term = -1;              // -1: no term
  std::string within, deliverable_kind, check, checked_by;
  std::string review_by = "none";   // "none", "another origin", "another provider", or a position
  std::vector<std::string> verdicts, accepts;
  std::string accepted_by;
  long long redeliver_within = -1;  // -1: no redelivery declared; a returned item reopens at once
  std::vector<std::string> after_kinds, after_items;
  bool operator==(const Kind&) const = default;
};

struct Kinds {
  bool declared = false;
  std::map<std::string, Kind> kinds;
  std::vector<std::string> unread;
};

namespace detail {
inline std::string text(const Json* j) { return j && j->is_string() ? j->text : std::string(); }
inline std::vector<std::string> strings(const Json* j) {
  std::vector<std::string> v;
  if (!j) return v;
  if (j->is_string()) v.push_back(j->text);
  else if (j->is_array())
    for (const Json& i : j->items)
      if (i.is_string()) v.push_back(i.text);
  return v;
}
inline long long whole(const Json* j, long long otherwise, std::vector<std::string>& unread, const std::string& label) {
  if (!j) return otherwise;
  if (!j->is_number() || j->lexical.empty() || j->lexical.find_first_not_of("0123456789") != std::string::npos || j->lexical.size() > 9) {
    unread.push_back(label + " is not a whole number of nought or more");
    return otherwise;
  }
  return std::stoll(j->lexical);
}
}  // namespace detail

// Reads one kind of work from its written form.
inline Kind read_kind(const std::string& name, const Json& x, std::vector<std::string>& unread) {
  using detail::text;
  Kind k;
  k.name = name;
  const std::string where = "the kind of work " + name;
  if (!x.is_object()) {
    unread.push_back(where + " is not an object");
    return k;
  }
  k.posted_by = detail::strings(x.get("posted by"));
  if (x.has("taken by")) k.taken_by = text(x.get("taken by"));
  k.takers = detail::whole(x.get("takers"), 1, unread, where + "'s takers");
  if (const Json* e = x.get("eligible"); e && e->is_object()) {
    if (e->has("who")) k.who = text(e->get("who"));
    k.other_provider = e->has("provider other than");
    k.other_origin = e->has("origin other than");
  }
  k.term = detail::whole(x.get("term"), -1, unread, where + "'s term");
  if (const Json* d = x.get("deliverable"); d && d->is_object())
    k.within = text(d->get("within")), k.deliverable_kind = text(d->get("kind")), k.check = text(d->get("check")), k.checked_by = text(d->get("checked by"));
  if (const Json* r = x.get("review"); r && r->is_object()) {
    if (r->has("by")) k.review_by = text(r->get("by"));
    k.verdicts = detail::strings(r->get("verdicts")), k.accepts = detail::strings(r->get("accepts"));
  }
  k.accepted_by = text(x.get("accepted by"));
  if (const Json* r = x.get("returned"); r && r->is_object()) k.redeliver_within = detail::whole(r->get("redeliver within"), -1, unread, where + "'s redelivery");
  if (const Json* a = x.get("after"); a && a->is_object()) k.after_kinds = detail::strings(a->get("kinds")), k.after_items = detail::strings(a->get("items"));
  return k;
}

// Reads the part "work".
inline Kinds read(const Json* part) {
  Kinds w;
  if (!part) return w;
  w.declared = true;
  const Json* kinds = part->is_object() ? part->get("kinds") : nullptr;
  if (!kinds || !kinds->is_object()) {
    w.unread.push_back("the part work declares its kinds in an object \"kinds\"");
    return w;
  }
  for (const auto& [name, x] : kinds->members) w.kinds[name] = read_kind(name, x, w.unread);
  return w;
}

// What a kind may name of the community, given by the constitution for its faults of closure.
struct Names {
  std::set<std::string> positions, holdings, decisions;
};

// Every fault of the kinds, each named: who posts, takes, checks, reviews and accepts declared; a term and redelivery of nought or more;
// verdicts that include those the kind accepts; a review by a position, another origin, another provider, or none; what must come
// first a kind declared, and no kind after itself.
inline std::vector<std::string> faults(const Kinds& w, const Names& n) {
  std::vector<std::string> out = w.unread;
  for (const auto& [name, k] : w.kinds) {
    const std::string where = "the kind of work " + name;
    if (k.posted_by.empty()) out.push_back(where + " names no position that posts it");
    for (const auto& p : k.posted_by)
      if (!n.positions.count(p)) out.push_back(where + " is posted by " + p + ", which the constitution lacks");
    if (k.taken_by != "commit" && k.taken_by != "award") out.push_back(where + " is taken by commit or by award, not " + k.taken_by);
    if (k.takers < 1) out.push_back(where + " admits at least one taker");
    if (k.who != "anyone" && !n.positions.count(k.who)) out.push_back(where + " is taken by holders of " + k.who + ", which the constitution lacks");
    if (!k.within.empty() && !n.holdings.count(k.within)) out.push_back(where + " keeps its deliverables within " + k.within + ", which the community does not hold");
    if (!k.check.empty() && !n.positions.count(k.checked_by)) out.push_back(where + " declares the check " + k.check + " and names no office that enters its result");
    const bool by_position = k.review_by != "none" && k.review_by != "another origin" && k.review_by != "another provider";
    if (by_position && !n.positions.count(k.review_by)) out.push_back(where + " is reviewed by " + k.review_by + ": none, another origin, another provider, or a position the constitution declares");
    if (k.review_by != "none") {
      if (k.accepts.empty()) out.push_back(where + " is reviewed and accepts no verdict");
      for (const auto& a : k.accepts)
        if (std::find(k.verdicts.begin(), k.verdicts.end(), a) == k.verdicts.end()) out.push_back(where + " accepts the verdict " + a + ", which is none of its verdicts");
    }
    if (k.accepted_by.empty() || (!n.positions.count(k.accepted_by) && !n.decisions.count(k.accepted_by)))
      out.push_back(where + " is accepted by " + (k.accepted_by.empty() ? std::string("no one") : k.accepted_by) + ": an office or a decision the constitution declares");
    for (const auto& a : k.after_kinds) {
      if (!w.kinds.count(a)) out.push_back(where + " comes after the kind " + a + ", which is not declared");
      if (a == name) out.push_back(where + " comes after itself");
    }
  }
  return out;
}

// ---- The life of an item ---------------------------------------------------------------------------------------------------------

struct Delivery {
  std::string commitment, artefact, by, origin, provider;
  long long turn = 0;
  std::string check = "none";  // none (no check declared), pending, passed, failed
};
struct Review {
  std::string by, origin, provider, verdict;
  long long turn = 0;
  std::size_t delivery = 0;  // the delivery it bears on, by its place
};
struct Taking {
  std::string commitment, taker, bearer, origin, provider;
  long long since = 0;
  bool ended = false;     // released, cancelled or handed over by an act
  bool accepted = false;  // discharged
  long long accepted_at = -1;
};
struct Item {
  std::string id, kind, poster, spec, matter, of;
  Kind life;                       // the kind's life as it stood when the item was posted, under which the item lives
  std::set<std::string> eligible;  // narrowed to these parties, when not empty
  long long posted = 0, due = -1;
  bool withdrawn = false;
  std::vector<Taking> takings;
  std::vector<Delivery> deliveries;
  std::vector<Review> reviews;
};

// The state of one taking at a turn: taken, delivered (awaiting its check, review or acceptance), reviewed (a verdict the kind
// accepts), returned (a verdict outside them, within the redelivery), accepted, lapsed (its term run with nothing delivered), ended
// (by an act, or a return whose redelivery ran out).
inline std::string taking_state(const Kind& k, const Item& i, const Taking& t, long long turn) {
  if (t.accepted) return "accepted";
  if (t.ended) return "ended";
  const Delivery* last = nullptr;
  std::size_t place = 0;
  for (std::size_t d = 0; d < i.deliveries.size(); d++)
    if (i.deliveries[d].commitment == t.commitment && i.deliveries[d].check != "failed") last = &i.deliveries[d], place = d;
  if (!last) return k.term >= 0 && turn >= t.since + k.term ? "lapsed" : "taken";
  const Review* r = nullptr;
  for (const auto& x : i.reviews)
    if (x.delivery == place) r = &x;
  if (last->check == "pending") return "delivered";
  if (k.review_by == "none" || !r) return "delivered";
  if (std::find(k.accepts.begin(), k.accepts.end(), r->verdict) != k.accepts.end()) return "reviewed";
  return k.redeliver_within >= 0 && turn <= r->turn + k.redeliver_within ? "returned" : "ended";
}
// Whether a taking holds the item at a turn: taken, delivered, reviewed or returned.
inline bool active(const std::string& state) { return state == "taken" || state == "delivered" || state == "reviewed" || state == "returned"; }

// The state of an item at a turn: withdrawn; accepted (as many takings accepted as the kind admits takers); waiting (what must come
// first is not accepted); open (fewer takings active or accepted than takers); otherwise the state of its taking (with several takers,
// "taken"). `first_done` says whether what must come first is accepted, which the chronicle reads from the record.
inline std::string state(const Kind& k, const Item& i, long long turn, bool first_done = true) {
  if (i.withdrawn) return "withdrawn";
  long long accepted = 0, holding = 0;
  std::string last;
  for (const auto& t : i.takings) {
    const std::string s = taking_state(k, i, t, turn);
    accepted += s == "accepted";
    if (active(s)) holding++, last = s;
  }
  if (accepted >= k.takers) return "accepted";
  if (holding + accepted < k.takers) return first_done ? "open" : "waiting";
  return k.takers == 1 ? last : "taken";
}
// Whether an open item is overdue at a turn: it has a due turn, and it is past it, not accepted or withdrawn.
inline bool overdue(const Kind& k, const Item& i, long long turn) {
  const std::string s = state(k, i, turn);
  return i.due >= 0 && turn > i.due && s != "accepted" && s != "withdrawn";
}

}  // namespace thesmos::work
