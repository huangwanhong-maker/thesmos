// Checks of compacts (decision 0013): terms read and their faults named, their digest independent of how they are written; the faults of
// reservations; a compact of three parties entering into force when all have ratified, one party's withdrawal taking effect after the
// notice and leaving the others bound; entry by a number of ratifications; articles binding pairs of parties, a reservation excluding its
// article both ways until withdrawn; a last turn; on 300 random records the turn of entry equal to the first turn at which the rule of
// entry held, found turn by turn; and each party's ratification and withdrawal kept apart (Proposition 35).

#include <thesmos/compacts/compacts.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <string>
#include <vector>

using namespace thesmos;
using compacts::Ratification;
using compacts::Record;
using compacts::Terms;

namespace {
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
bool has(const std::vector<std::string>& v, const std::string& x) { return std::find(v.begin(), v.end(), x) != v.end(); }
bool any_has(const std::vector<std::string>& v, const std::string& part) {
  for (const auto& x : v)
    if (x.find(part) != std::string::npos) return true;
  return false;
}
const char* kTerms = R"({"format": "noetarium.compact/1", "id": "clearing compact", "parties": ["x", "y", "z"],
  "articles": ["article 1", {"id": "article 2", "closed to reservation": true}, "article 3"],
  "reservations": "admitted", "entry into force": "all", "withdrawal": {"notice": 3}, "to": 60})";
Ratification ratified(const Terms& t, const std::string& party, long long since, std::vector<std::string> reserved = {}) {
  Ratification r;
  r.id = t.id, r.party = party, r.has_terms = true, r.terms = t, r.digest = compacts::digest(t), r.since = since;
  for (const auto& a : reserved) r.reservations.push_back({a, since, -1});
  return r;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const Terms t = compacts::read(std::string_view(kTerms));

  // ---- 1. Terms ----
  {
    const Terms again = compacts::read(std::string_view(R"({"to": 60, "withdrawal": {"notice": 3}, "entry into force": "all", "reservations": "admitted",
      "articles": ["article 1", {"closed to reservation": true, "id": "article 2"}, "article 3"], "parties": ["x", "y", "z"], "id": "clearing compact",
      "format": "noetarium.compact/1"})"));
    Terms later = t;
    later.notice = 4;
    s.require("the terms are read with no fault, and their digest is the same however they are written, and another when the notice differs",
              compacts::faults(t).empty() && t.parties.size() == 3 && t.article("article 2")->closed && t.notice == 3 && t.to == 60 && compacts::digest(again) == compacts::digest(t) &&
                  compacts::digest(later) != compacts::digest(t),
              compacts::faults(t).empty() ? "" : compacts::faults(t).front());
    const auto f = compacts::faults(compacts::read(std::string_view(
        R"({"format": "noetarium.treaty/1", "id": "", "parties": ["x"], "articles": [], "reservations": "maybe", "entry into force": "all", "withdrawal": 3})")));
    const auto g = compacts::faults(compacts::read(std::string_view(
        R"({"format": "noetarium.compact/1", "id": "k", "parties": ["x", "y"], "articles": ["a", "a"], "entry into force": 5})")));
    s.require("faults are named: another format, no name, no article, a rule of entry by all with one party, reservations neither admitted nor none, a withdrawal written "
              "as a number; an article twice, and more ratifications asked than parties named",
              any_has(f, "format") && any_has(f, "named") && any_has(f, "at least one article") && any_has(f, "at least two parties") && any_has(f, "\"admitted\" or \"none\"") &&
                  any_has(f, "withdrawal is") && any_has(g, "named twice") && any_has(g, "more ratifications"));
  }

  // ---- 2. Reservations ----
  {
    Ratification bare;
    bare.id = "clearing compact", bare.reservations = {{"article 1", 0, -1}};
    Terms closed_all = t;
    closed_all.reservations = false;
    auto none = ratified(closed_all, "x", 0, {"article 1"});
    auto closed = ratified(t, "x", 0, {"article 2"}), unknown = ratified(t, "x", 0, {"article 9"}), twice = ratified(t, "x", 0, {"article 1", "article 1"}),
         fine = ratified(t, "x", 0, {"article 1", "article 3"});
    s.require("reservations: refused to a compact ratified by its name alone, to one whose terms admit none, of an article closed to reservation, of an article it lacks, "
              "and of one article twice; two open articles reserved are admitted",
              any_has(compacts::reservation_faults(bare), "without its terms") && any_has(compacts::reservation_faults(none), "admits no reservation") &&
                  any_has(compacts::reservation_faults(closed), "closed to reservation") && any_has(compacts::reservation_faults(unknown), "names no article") &&
                  any_has(compacts::reservation_faults(twice), "reserved twice") && compacts::reservation_faults(fine).empty());
  }

  // ---- 3. Three parties, entry by all, one withdrawal ----
  {
    Record r{{"x", {ratified(t, "x", 2)}}, {"y", {ratified(t, "y", 5, {"article 3"})}}};
    const auto two = compacts::standing(t, r, 6);
    r["z"] = {ratified(t, "z", 8)};
    const auto all = compacts::standing(t, r, 8);
    r["y"].back().withdrawn = 10;  // effective at 13
    const auto during = compacts::standing(t, r, 12), after = compacts::standing(t, r, 13);
    r["x"].back().withdrawn = 20;
    const auto alone = compacts::standing(t, r, 23);
    s.require("a compact of three entering into force when all have ratified: not in force with two; in force from the third's ratification at turn 8; the second "
              "party's withdrawal at turn 10 leaves it bound through the notice and unbound from turn 13, the other two still bound; when a second withdraws, "
              "one party is left and the compact binds no one",
              !two.in_force && two.entered < 0 && has(two.pending, "z") && all.in_force && all.entered == 8 && all.bound.size() == 3 && during.in_force &&
                  has(during.bound, "y") && has(during.withdrawing, "y") && after.in_force && !has(after.bound, "y") && has(after.withdrawn, "y") && after.bound.size() == 2 &&
                  !alone.in_force && alone.bound.empty() && alone.entered == 8);
  }

  // ---- 4. Entry by a number of ratifications ----
  {
    Terms two = t;
    two.entry_all = false, two.entry = 2;
    Record r{{"x", {ratified(two, "x", 4)}}};
    const auto one = compacts::standing(two, r, 6);
    r["z"] = {ratified(two, "z", 7)};
    const auto entered = compacts::standing(two, r, 7);
    r["y"] = {ratified(two, "y", 9)};
    const auto joined = compacts::standing(two, r, 9);
    s.require("a compact entering into force at two ratifications: not with one; in force from the second at turn 7 between the two; a third party bound from its own "
              "ratification at turn 9",
              !one.in_force && entered.in_force && entered.entered == 7 && entered.bound == std::vector<std::string>{"x", "z"} && joined.bound.size() == 3);
  }

  // ---- 5. Articles between pairs, reservations both ways ----
  {
    Record r{{"x", {ratified(t, "x", 1)}}, {"y", {ratified(t, "y", 1, {"article 3"})}}, {"z", {ratified(t, "z", 1)}}};
    const bool xz = compacts::binds(t, r, 5, "article 3", "x", "z"), xy = compacts::binds(t, r, 5, "article 3", "x", "y"), yx = compacts::binds(t, r, 5, "article 3", "y", "x"),
               y1 = compacts::binds(t, r, 5, "article 1", "y", "z"), none = compacts::binds(t, r, 5, "article 9", "x", "z");
    r["y"].back().reservations.front().until = 7;
    const bool restored = compacts::binds(t, r, 7, "article 3", "x", "y"), before = compacts::binds(t, r, 6, "article 3", "x", "y");
    s.require("an article binds two bound parties to each other unless either reserved it: article 3, reserved by y, binds x and z and neither x and y nor y and x; "
              "article 1 binds y and z; an article the compact lacks binds no one; the reservation withdrawn at turn 7 restores article 3 between x and y from then",
              xz && !xy && !yx && y1 && !none && restored && !before);
  }

  // ---- 6. A last turn ----
  {
    Record r{{"x", {ratified(t, "x", 1)}}, {"y", {ratified(t, "y", 1)}}, {"z", {ratified(t, "z", 1)}}};
    s.require("a compact with a last turn is in force at it and not after", compacts::standing(t, r, 60).in_force && !compacts::standing(t, r, 61).in_force);
  }

  // ---- 7. Entry found turn by turn, and each party kept apart (Proposition 35) ----
  {
    calx::random::Stream rng(20261127, 35, 0, 0);
    bool entry = true, apart = true, kept = true;
    long long entered = 0;
    for (int trial = 0; trial < 300; trial++) {
      Terms k = t;
      const long long n = between(rng, 2, 5);
      k.parties.clear();
      for (long long i = 0; i < n; i++) k.parties.push_back("p" + std::to_string(i));
      k.entry_all = rng.below(2) == 0, k.entry = between(rng, 2, n), k.notice = between(rng, 0, 4), k.to = rng.below(3) == 0 ? between(rng, 10, 40) : -1;
      Record r;
      for (const auto& p : k.parties) {
        if (rng.below(5) == 0) continue;
        long long at = between(rng, 0, 15);
        for (int j = 0, times = static_cast<int>(between(rng, 1, 2)); j < times; j++) {
          Ratification x = ratified(k, p, at);
          if (rng.below(2) == 0) x.withdrawn = at + between(rng, 0, 10);
          r[p].push_back(x);
          if (x.withdrawn < 0) break;
          at = x.effective() + between(rng, 0, 5);
        }
      }
      const long long required = k.entry_all ? n : k.entry;
      long long first = -1;
      for (long long turn = 0; turn <= 60 && first < 0; turn++) {
        long long c = 0;
        for (const auto& p : k.parties)
          if (r.count(p) && compacts::standing(r.at(p), turn)) c++;
        if (c >= required) first = turn;
      }
      const auto end = compacts::standing(k, r, 60);
      entry = entry && end.entered == first;
      entered += first >= 0;
      // Apartness: take a party that ratified, and remove its withdrawals; every other party's standing is unchanged at every turn, and an
      // entered compact keeps binding the others wherever at least two of them stand.
      if (r.empty()) continue;
      const std::string p = r.begin()->first;
      Record unwithdrawn = r;
      for (auto& x : unwithdrawn[p]) x.withdrawn = -1;
      unwithdrawn[p].resize(1);
      for (long long turn = 0; turn <= 60; turn++) {
        const auto a = compacts::standing(k, r, turn), b = compacts::standing(k, unwithdrawn, turn);
        long long others = 0;
        for (const auto& q : k.parties) {
          if (q == p) continue;
          apart = apart && has(a.ratified, q) == has(b.ratified, q);
          others += has(a.ratified, q);
        }
        if (a.entered >= 0 && a.entered <= turn && (k.to < 0 || turn <= k.to) && others >= 2)
          for (const auto& q : k.parties)
            if (q != p && has(a.ratified, q)) kept = kept && has(a.bound, q) && has(b.bound, q);
      }
    }
    s.require("on 300 random records of two to five parties, ratifying, withdrawing and ratifying again, the turn of entry is the first at which the rule of entry held, "
              "found turn by turn (" + std::to_string(entered) + " entered); one party's withdrawal changes no other party's standing at any turn, and wherever at "
              "least two others stand in an entered compact they stay bound whatever it does (Proposition 35)",
              entry && apart && kept && entered > 50);
  }
  return s.done();
}
