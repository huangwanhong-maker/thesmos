// Compacts (decision 0013): agreements among communities, each party bound by its own ratification, under its own rule, subject to its
// reservations, until its withdrawal takes effect (the design record of 2026-11-27, Sections 5.13 and 8.11; the Vienna Convention on the
// Law of Treaties, 1969, Articles 2(1)(d), 14, 19 to 23, 24, 54 to 56 and 65, read as the ground reads a treaty and no further).
//
// The terms, written as noetarium.compact/1, are carried by each party's act of ratification:
//
//   {"format": "noetarium.compact/1", "id": "clearing compact", "parties": ["x", "y", "z"],
//    "articles": ["article 1", {"id": "article 2", "closed to reservation": true}, "article 3"],
//    "reservations": "admitted",
//    "entry into force": "all",
//    "withdrawal": {"notice": 3},
//    "to": 60}
//
//   parties          the communities it would bind; none named: any community that ratifies it;
//   articles         what it binds its parties to, each closed to reservation or not;
//   reservations     "admitted" (the default) or "none";
//   entry into force "all" (every party named has ratified) or a number of ratifications, at least two;
//   withdrawal       {"notice": n}, the turns between a party's withdrawal and its effect, or "none";
//   to               its last turn, if it has one.
//
// A party's ratification is a record of its own chronicle (an amendment decided by its own rule): the compact, its terms and their digest
// when it ratified them, its reservations (each from a turn, until it withdraws it), the turn of ratification, and the turn of its
// withdrawal, effective after the compact's notice. A compact of several parties keeps each party's ratification and withdrawal apart:
// one party's withdrawal ends its own obligations and no other's, and the compact stays in force among the rest while at least two are
// bound. Read across the parties' records, the compact entered into force at the first turn at which its rule of entry held; it is in
// force at a turn when it has entered, its last turn has not passed, and at least two parties are bound; a party is bound while its
// ratification stands and the compact is in force; an article binds two parties when both are bound and neither has reserved it, the
// reservation excluding the article between the reserving party and every other, both ways (Article 21(1)).
//
// The disclosure agreement of decision 0010 is the smallest case: a compact of two parties, the principal and the community, entering
// into force when both have ratified, withdrawn by either after its notice; the theory note's Proposition 36 holds the two equal.

#pragma once

#include <archeion/fixity/sha256.hpp>

#include <symploke/jsonld/json.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::compacts {

using Json = symploke::jsonld::Json;

struct Article {
  std::string id;
  bool closed = false;  // closed to reservation
  bool operator==(const Article&) const = default;
};

struct Terms {
  std::string format, id;
  std::vector<std::string> parties;  // none named: any community that ratifies it
  std::vector<Article> articles;
  bool reservations = true;          // whether it admits reservations at all
  bool entry_all = true;             // enters into force when every party named has ratified, else at `entry` ratifications
  long long entry = 0;
  bool withdrawal = true;            // whether a party may withdraw
  long long notice = 0;              // the turns between a withdrawal and its effect
  long long to = -1;                 // its last turn; -1: none
  std::vector<std::string> unread;

  const Article* article(const std::string& a) const {
    for (const auto& x : articles)
      if (x.id == a) return &x;
    return nullptr;
  }
  bool operator==(const Terms&) const = default;
};

namespace detail {
inline std::string text(const Json* j) { return j && j->is_string() ? j->text : std::string(); }
inline bool whole(const Json* j, long long& out) {
  if (!j || !j->is_number() || j->number < 0 || j->number != static_cast<double>(static_cast<long long>(j->number))) return false;
  out = static_cast<long long>(j->number);
  return true;
}
}  // namespace detail

inline Terms read(const Json& j) {
  Terms t;
  if (!j.is_object()) {
    t.unread.push_back("the terms are not an object");
    return t;
  }
  t.format = detail::text(j.get("format")), t.id = detail::text(j.get("id"));
  if (const Json* p = j.get("parties"); p && p->is_array())
    for (const Json& x : p->items) {
      if (x.is_string()) t.parties.push_back(x.text);
      else t.unread.push_back("a party is named by a string");
    }
  if (const Json* a = j.get("articles"); a && a->is_array())
    for (const Json& x : a->items) {
      if (x.is_string()) t.articles.push_back({x.text, false});
      else if (x.is_object() && x.get("id") && x.get("id")->is_string()) {
        Article art{x.get("id")->text, false};
        if (const Json* c = x.get("closed to reservation")) {
          if (c->is_bool()) art.closed = c->truth;
          else t.unread.push_back("whether the article " + art.id + " is closed to reservation is true or false");
        }
        t.articles.push_back(art);
      } else
        t.unread.push_back("an article is named, or described with its id");
    }
  if (const Json* r = j.get("reservations")) {
    if (r->is_string() && (r->text == "admitted" || r->text == "none")) t.reservations = r->text == "admitted";
    else t.unread.push_back("reservations are \"admitted\" or \"none\"");
  }
  if (const Json* e = j.get("entry into force")) {
    if (e->is_string() && e->text == "all") t.entry_all = true;
    else if (detail::whole(e, t.entry)) t.entry_all = false;
    else t.unread.push_back("entry into force is \"all\" or a number of ratifications");
  }
  if (const Json* w = j.get("withdrawal")) {
    if (w->is_string() && w->text == "none") t.withdrawal = false;
    else if (w->is_object()) {
      if (const Json* n = w->get("notice"); n && !detail::whole(n, t.notice)) t.unread.push_back("the notice of withdrawal is a whole number of turns");
    } else
      t.unread.push_back("withdrawal is {\"notice\": n} or \"none\"");
  }
  if (const Json* to = j.get("to"); to && !detail::whole(to, t.to)) t.unread.push_back("the last turn is a whole number");
  return t;
}
inline Terms read(std::string_view text) { return read(symploke::jsonld::parse(text)); }

// Every fault of form of terms.
inline std::vector<std::string> faults(const Terms& t) {
  std::vector<std::string> out = t.unread;
  if (t.format != "noetarium.compact/1") out.push_back("the terms' format is not noetarium.compact/1");
  if (t.id.empty()) out.push_back("the compact is named");
  std::set<std::string> seen;
  for (const auto& p : t.parties)
    if (p.empty() || !seen.insert(p).second) out.push_back("the party " + p + " is named twice, or not at all");
  if (t.articles.empty()) out.push_back("the compact binds its parties to at least one article");
  seen.clear();
  for (const auto& a : t.articles)
    if (a.id.empty() || !seen.insert(a.id).second) out.push_back("the article " + a.id + " is named twice, or not at all");
  if (t.entry_all && t.parties.size() < 2) out.push_back("a compact that enters into force when all have ratified names at least two parties");
  if (!t.entry_all && t.entry < 2) out.push_back("a compact enters into force with at least two ratifications");
  if (!t.entry_all && !t.parties.empty() && t.entry > static_cast<long long>(t.parties.size()))
    out.push_back("the compact asks more ratifications than it names parties");
  return out;
}

// The canonical text of terms, and its digest: two ratifications ratify the same compact when their digests agree.
inline std::string canonical(const Terms& t) {
  Json j = Json::object(), parties = Json::array(), articles = Json::array();
  for (const auto& p : t.parties) parties.items.push_back(Json::string(p));
  for (const auto& a : t.articles) {
    Json x = Json::object();
    x.set("id", Json::string(a.id)), x.set("closed to reservation", Json::boolean(a.closed));
    articles.items.push_back(x);
  }
  Json w = Json::object();
  w.set("notice", Json::num(static_cast<double>(t.notice)));
  j.set("format", Json::string(t.format)), j.set("id", Json::string(t.id)), j.set("parties", parties), j.set("articles", articles);
  j.set("reservations", Json::string(t.reservations ? "admitted" : "none"));
  j.set("entry into force", t.entry_all ? Json::string("all") : Json::num(static_cast<double>(t.entry)));
  j.set("withdrawal", t.withdrawal ? w : Json::string("none"));
  if (t.to >= 0) j.set("to", Json::num(static_cast<double>(t.to)));
  return symploke::jsonld::canonical(j);
}
inline std::string digest(const Terms& t) { return archeion::fixity::sha256_hex(canonical(t)); }

// ---- A party's ratification --------------------------------------------------------------------------------------------------------

struct Reservation {
  std::string article;
  long long since = 0, until = -1;  // held from the ratification (or later) until withdrawn; -1: not withdrawn
  bool held(long long turn) const { return turn >= since && (until < 0 || turn < until); }
  bool operator==(const Reservation&) const = default;
};

struct Ratification {
  std::string id;                 // the compact
  std::string party;              // the community whose ratification it is
  bool has_terms = false;         // ratified with its terms (an older record ratified a compact by its name alone)
  Terms terms;
  std::string digest;             // of the terms ratified
  std::vector<Reservation> reservations;
  long long since = 0;            // the turn of ratification (0 for a compact its founding declares)
  long long withdrawn = -1;       // the turn of its withdrawal; -1: none

  long long notice() const { return has_terms ? terms.notice : 0; }
  long long effective() const { return withdrawn < 0 ? -1 : withdrawn + notice(); }  // the turn from which its withdrawal has effect
  bool stands(long long turn) const { return turn >= since && (withdrawn < 0 || turn < effective()); }
  bool reserved(const std::string& article, long long turn) const {
    for (const auto& r : reservations)
      if (r.article == article && r.held(turn)) return true;
    return false;
  }
  std::vector<std::string> reserved_at(long long turn) const {
    std::vector<std::string> out;
    for (const auto& r : reservations)
      if (r.held(turn)) out.push_back(r.article);
    return out;
  }
  bool operator==(const Ratification&) const = default;
};

// Every fault of the reservations a ratification makes (Articles 19 and 20): a compact ratified without its terms admits none, nor one
// whose terms admit none; each names an article of the compact, once, not closed to reservation.
inline std::vector<std::string> reservation_faults(const Ratification& r) {
  std::vector<std::string> out;
  std::set<std::string> seen;
  for (const auto& x : r.reservations) {
    if (!r.has_terms) {
      out.push_back("a compact ratified without its terms admits no reservation");
      break;
    }
    if (!r.terms.reservations) {
      out.push_back("the compact " + r.id + " admits no reservation");
      break;
    }
    const Article* a = r.terms.article(x.article);
    if (!a) out.push_back("the reservation of " + x.article + " names no article of the compact " + r.id);
    else if (a->closed) out.push_back("the article " + x.article + " of the compact " + r.id + " is closed to reservation");
    if (!seen.insert(x.article).second) out.push_back("the article " + x.article + " is reserved twice");
  }
  return out;
}

// ---- The compact across its parties --------------------------------------------------------------------------------------------------

// Each party's ratifications of one compact, in order (a party may ratify again after its withdrawal took effect).
using Record = std::map<std::string, std::vector<Ratification>>;

inline const Ratification* standing(const std::vector<Ratification>& rs, long long turn) {
  for (auto it = rs.rbegin(); it != rs.rend(); ++it)
    if (it->stands(turn)) return &*it;
  return nullptr;
}

struct Standing {
  long long entered = -1;                      // the turn it entered into force; -1: not yet
  bool in_force = false;
  std::vector<std::string> bound;              // the parties it binds at the turn
  std::vector<std::string> ratified;           // whose ratification stands at the turn, in force or not
  std::vector<std::string> withdrawing;        // whose withdrawal is made and not yet effective
  std::vector<std::string> withdrawn;          // whose withdrawal has taken effect, with no ratification standing since
  std::vector<std::string> pending;            // named, or recorded, with no ratification standing and none withdrawn
};

namespace detail {
inline std::vector<std::string> parties_of(const Terms& t, const Record& r) {
  if (!t.parties.empty()) return t.parties;
  std::vector<std::string> out;
  for (const auto& [p, rs] : r) out.push_back(p);
  return out;
}
inline long long standing_count(const std::vector<std::string>& parties, const Record& r, long long turn) {
  long long n = 0;
  for (const auto& p : parties)
    if (const auto it = r.find(p); it != r.end() && standing(it->second, turn)) n++;
  return n;
}
}  // namespace detail

// The compact's standing at a turn, read from every party's ratifications.
inline Standing standing(const Terms& t, const Record& r, long long turn) {
  Standing s;
  const auto parties = detail::parties_of(t, r);
  const long long required = t.entry_all ? static_cast<long long>(parties.size()) : t.entry;
  // The count of standing ratifications changes only at a ratification or at a withdrawal's effect: entry is the first such turn at
  // which the rule of entry held.
  std::set<long long> turns;
  for (const auto& p : parties)
    if (const auto it = r.find(p); it != r.end())
      for (const auto& x : it->second) {
        if (x.since <= turn) turns.insert(x.since);
        if (x.withdrawn >= 0 && x.effective() <= turn) turns.insert(x.effective());
      }
  for (long long c : turns)
    if (required >= 2 && detail::standing_count(parties, r, c) >= required) {
      s.entered = c;
      break;
    }
  const long long now = detail::standing_count(parties, r, turn);
  s.in_force = s.entered >= 0 && (t.to < 0 || turn <= t.to) && now >= 2;
  for (const auto& p : parties) {
    const auto it = r.find(p);
    const Ratification* x = it == r.end() ? nullptr : standing(it->second, turn);
    if (x) {
      s.ratified.push_back(p);
      if (s.in_force) s.bound.push_back(p);
      if (x->withdrawn >= 0) s.withdrawing.push_back(p);
      continue;
    }
    const Ratification* last = nullptr;  // its last ratification made by the turn
    if (it != r.end())
      for (const auto& y : it->second)
        if (y.since <= turn) last = &y;
    (last && last->withdrawn >= 0 && last->effective() <= turn ? s.withdrawn : s.pending).push_back(p);
  }
  return s;
}

inline bool bound(const Terms& t, const Record& r, const std::string& party, long long turn) {
  const Standing s = standing(t, r, turn);
  return std::find(s.bound.begin(), s.bound.end(), party) != s.bound.end();
}

// Whether an article binds two parties to each other at a turn: both bound, the article the compact's, and neither reserving it.
inline bool binds(const Terms& t, const Record& r, long long turn, const std::string& article, const std::string& a, const std::string& b) {
  if (a == b || !t.article(article)) return false;
  const Standing s = standing(t, r, turn);
  auto in = [&](const std::string& p) { return std::find(s.bound.begin(), s.bound.end(), p) != s.bound.end(); };
  if (!in(a) || !in(b)) return false;
  return !standing(r.at(a), turn)->reserved(article, turn) && !standing(r.at(b), turn)->reserved(article, turn);
}

}  // namespace thesmos::compacts
