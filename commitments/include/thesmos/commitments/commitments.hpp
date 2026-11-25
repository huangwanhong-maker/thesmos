// Commitments and their life; obligations of one class cleared by set-off and shortened by novation.
//
// A commitment binds a debtor to a creditor for a matter, after Singh's social commitments (An Ontology for Commitments in
// Multiagent Systems, Artificial Intelligence and Law 7, 1999): an offer is conditional until taken; it is then active; the
// debtor presents performance or hands over what is left; the creditor accepts (discharge), releases, or assigns the claim to
// another; the debtor cancels an active commitment (which stands as a breach) or withdraws an offer.
//
// Proposition 5 (the design record of 2026-11-25): for obligations o(i, j) ≥ 0 of one class in which set-off is agreed, with the
// net obligation of a party what it owes less what it is owed, (1) cancelling a common amount around a cycle leaves every net
// obligation as it was; (2) cancelling cycles to exhaustion leaves an acyclic residual; (3) its volume is at least N, the sum of
// the positive net obligations; (4) replacing "i owes j, j owes k" by "i owes k" as far as it goes leaves volume exactly N, every
// party purely a debtor or purely a creditor. Cancelling cycles in some order can clear less than the greatest circulation,
// which is a problem of minimum-cost flow.

#pragma once

#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace thesmos::commitments {

enum class Status { conditional, active, presented, handed_over, discharged, released, cancelled, withdrawn };

inline std::string_view word(Status s) {
  switch (s) {
    case Status::conditional: return "conditional";
    case Status::active: return "active";
    case Status::presented: return "presented";
    case Status::handed_over: return "handed over";
    case Status::discharged: return "discharged";
    case Status::released: return "released";
    case Status::cancelled: return "cancelled";
    case Status::withdrawn: return "withdrawn";
  }
  return "?";
}

// Whether the commitment is still owed: conditional, active, presented or handed over.
inline bool open(Status s) { return s == Status::conditional || s == Status::active || s == Status::presented || s == Status::handed_over; }

struct Commitment {
  std::string id, debtor, creditor, matter, unit, by, artefact;
  long long amount = 1, due = 0;
  Status status = Status::conditional;
};

// Overdue is a mark, not a status: an active commitment past its due turn.
inline bool overdue(const Commitment& c, long long turn) { return c.status == Status::active && c.due > 0 && turn > c.due; }

// ---- Obligations of one class -------------------------------------------------------------------------------------------------

using Edge = std::pair<std::string, std::string>;  // (debtor, creditor)
using Obligations = std::map<Edge, long long>;

inline std::map<std::string, long long> net(const Obligations& o) {
  std::map<std::string, long long> n;
  for (const auto& [e, amount] : o) {
    n[e.first] += amount;
    n[e.second] -= amount;
  }
  return n;
}

// The net obligations with zeros left out, for comparing two graphs on the same parties.
inline std::map<std::string, long long> net_nonzero(const Obligations& o) {
  std::map<std::string, long long> n;
  for (const auto& [p, v] : net(o))
    if (v) n[p] = v;
  return n;
}

inline long long volume(const Obligations& o) {
  long long v = 0;
  for (const auto& [e, amount] : o) v += amount;
  return v;
}

// N: the sum of the positive net obligations, the least volume that settles the graph.
inline long long floor_volume(const Obligations& o) {
  long long f = 0;
  for (const auto& [p, v] : net(o))
    if (v > 0) f += v;
  return f;
}

namespace detail {
inline Obligations positive(Obligations o) {
  for (auto it = o.begin(); it != o.end();) it = it->second > 0 ? std::next(it) : o.erase(it);
  return o;
}
}  // namespace detail

// A cycle of positive obligations, found by depth-first search in the order of the parties' names, or none.
inline std::optional<std::vector<std::string>> find_cycle(const Obligations& o) {
  std::map<std::string, std::vector<std::string>> adjacent;
  for (const auto& [e, amount] : o)
    if (amount > 0) adjacent[e.first].push_back(e.second);
  for (auto& [u, vs] : adjacent) std::sort(vs.begin(), vs.end());
  std::map<std::string, int> colour;
  std::vector<std::string> path;
  std::optional<std::vector<std::string>> found;
  auto visit = [&](auto&& self, const std::string& u) -> bool {
    colour[u] = 1;
    path.push_back(u);
    for (const std::string& v : adjacent[u]) {
      if (colour[v] == 1) {
        found = std::vector<std::string>(std::find(path.begin(), path.end(), v), path.end());
        return true;
      }
      if (colour[v] == 0 && self(self, v)) return true;
    }
    colour[u] = 2;
    path.pop_back();
    return false;
  };
  std::vector<std::string> starts;
  for (const auto& [u, vs] : adjacent) starts.push_back(u);
  for (const std::string& s : starts)
    if (colour[s] == 0 && visit(visit, s)) return found;
  return std::nullopt;
}

inline bool acyclic(const Obligations& o) { return !find_cycle(o); }

struct SetOff {
  Obligations residual;
  long long cleared = 0;                                            // volume cleared
  std::vector<std::pair<std::vector<std::string>, long long>> cycles;  // each cycle with its common reduction
};

// Cancels cycles until none is left.
inline SetOff set_off(Obligations o) {
  SetOff r;
  o = detail::positive(std::move(o));
  while (const auto cycle = find_cycle(o)) {
    const std::vector<std::string>& c = *cycle;
    long long least = -1;
    for (std::size_t k = 0; k < c.size(); k++) {
      const long long a = o[{c[k], c[(k + 1) % c.size()]}];
      least = least < 0 ? a : std::min(least, a);
    }
    for (std::size_t k = 0; k < c.size(); k++) o[{c[k], c[(k + 1) % c.size()]}] -= least;
    o = detail::positive(std::move(o));
    r.cleared += least * static_cast<long long>(c.size());
    r.cycles.emplace_back(c, least);
  }
  r.residual = std::move(o);
  return r;
}

struct Novation {
  Obligations residual;
  int steps = 0;
};

// Replaces "i owes j, j owes k" by "i owes k" as far as it goes; each step needs the consent of the three.
inline Novation novate(Obligations o) {
  Novation r;
  o = detail::positive(std::move(o));
  while (true) {
    std::optional<std::pair<Edge, Edge>> found;
    for (const auto& [first, a] : o) {
      for (const auto& [second, b] : o)
        if (second.first == first.second) {
          found = std::make_pair(first, second);
          break;
        }
      if (found) break;
    }
    if (!found) break;
    const Edge ij = found->first, jk = found->second;
    const long long least = std::min(o[ij], o[jk]);
    o[ij] -= least;
    o[jk] -= least;
    if (ij.first != jk.second) o[{ij.first, jk.second}] += least;
    o = detail::positive(std::move(o));
    r.steps++;
  }
  r.residual = std::move(o);
  return r;
}

}  // namespace thesmos::commitments
