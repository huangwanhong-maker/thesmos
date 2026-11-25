// Mandates: authority along chains that only narrow, envelopes charged along the whole chain, and the exposure of a late
// revocation.
//
// A mandate is an act by which a principal empowers a holder: the kinds of act permitted, the matters, an allowance for each
// resource in its own unit, a term in turns, whether it may be delegated, its parent, and a slice (the most that may be charged
// between two checks of its status). A chain stands when its root principal can answer (is no operative), each link was
// issued by the holder of its parent and the parent may be delegated, each link narrows its parent in kinds, matters,
// allowance and term, and no link is revoked (the design record of 2026-11-25, Definition 4; the mandate of the Community
// Diplomacy paper with the slice added).
//
// Proposition 2: when each charge is made against every link from the charged one to the root, the use beneath any link is at
// most its ceiling; with ceilings charged apart, two children of 80 under a root of 100 admit 160. Proposition 3: a verifier
// that learns of a revocation at its next check, admitting at most a slice between two checks, lets at most min(B, σ) be
// charged after the revocation.

#pragma once

#include <thesmos/kernel/kernel.hpp>

#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace thesmos::mandates {

using kernel::Kind;
using kernel::Parties;

struct Mandate {
  std::string id, principal, holder;
  std::set<std::string> kinds;                  // kinds of act
  std::set<std::string> matters;                // subject matters; empty means none
  std::map<std::string, long long> allowance;   // resource -> ceiling, each resource in its own unit
  long long from = 0, to = 0;                   // first and last turn of validity
  bool delegable = false;
  std::string parent;                           // the mandate it is issued under, or empty for a root
  std::map<std::string, long long> slice;       // resource -> the most charged between two checks of status, in that
                                                // resource's unit; a resource absent here has no slice
};

using Registry = std::map<std::string, Mandate>;

// The chain of a mandate, root first. Throws when a mandate is unknown or the chain closes on itself.
inline std::vector<const Mandate*> chain(const Registry& r, const std::string& id) {
  std::vector<const Mandate*> links;
  std::set<std::string> seen;
  std::string cursor = id;
  while (!cursor.empty()) {
    const auto it = r.find(cursor);
    if (it == r.end()) throw std::invalid_argument("thesmos::mandates: the mandate " + cursor + " is unknown");
    if (!seen.insert(cursor).second) throw std::invalid_argument("thesmos::mandates: the chain of " + id + " closes on itself");
    links.push_back(&it->second);
    cursor = it->second.parent;
  }
  std::reverse(links.begin(), links.end());
  return links;
}

inline std::string root_principal(const Registry& r, const std::string& id) { return chain(r, id).front()->principal; }

// Every reason for which a chain carries no authority; an empty list means that it stands.
inline std::vector<std::string> faults(const Parties& ps, const Registry& r, const std::string& id, const std::set<std::string>& revoked = {}) {
  std::vector<std::string> out;
  std::vector<const Mandate*> links;
  try {
    links = chain(r, id);
  } catch (const std::invalid_argument& e) {
    return {e.what()};
  }
  const Mandate& root = *links.front();
  if (!ps.has(root.principal)) out.push_back("the root principal " + root.principal + " is unknown");
  else if (!kernel::answerable(ps.at(root.principal).kind)) out.push_back("the root principal is an operative: nobody answers");
  for (std::size_t k = 1; k < links.size(); k++) {
    const Mandate& parent = *links[k - 1];
    const Mandate& child = *links[k];
    if (child.principal != parent.holder) out.push_back(child.id + " was not issued by the holder of " + parent.id);
    if (!parent.delegable) out.push_back(parent.id + " may not be delegated");
    if (!std::includes(parent.kinds.begin(), parent.kinds.end(), child.kinds.begin(), child.kinds.end())) out.push_back(child.id + " widens the kinds of act");
    if (!std::includes(parent.matters.begin(), parent.matters.end(), child.matters.begin(), child.matters.end())) out.push_back(child.id + " widens the matters");
    for (const auto& [resource, amount] : child.allowance) {
      const auto it = parent.allowance.find(resource);
      if (it == parent.allowance.end() || amount > it->second) out.push_back(child.id + " widens the allowance of " + resource);
    }
    if (!(parent.from <= child.from && child.to <= parent.to)) out.push_back(child.id + " widens the term");
  }
  for (const Mandate* link : links)
    if (revoked.count(link->id)) out.push_back(link->id + " is revoked");
  return out;
}

// Use charged along the whole chain, so that each ceiling bounds everything beneath it. With `shared` false each mandate is
// charged alone, which reproduces the fault of independent ceilings.
class Envelopes {
 public:
  explicit Envelopes(const Registry& r, bool shared = true) : registry_(&r), shared_(shared) {}

  bool can_debit(const std::string& id, const std::string& resource, long long amount) const {
    for (const Mandate* link : links(id)) {
      const auto ceiling = link->allowance.find(resource);
      if (ceiling == link->allowance.end() || used(link->id, resource) + amount > ceiling->second) return false;
    }
    return true;
  }
  // Charges `amount` when every link has room for it, and changes nothing otherwise.
  bool debit(const std::string& id, const std::string& resource, long long amount) {
    if (!can_debit(id, resource, amount)) return false;
    for (const Mandate* link : links(id)) used_[link->id][resource] += amount;
    return true;
  }
  long long used(const std::string& id, const std::string& resource) const {
    const auto m = used_.find(id);
    if (m == used_.end()) return 0;
    const auto v = m->second.find(resource);
    return v == m->second.end() ? 0 : v->second;
  }
  const std::map<std::string, std::map<std::string, long long>>& all() const { return used_; }

 private:
  std::vector<const Mandate*> links(const std::string& id) const {
    if (shared_) return chain(*registry_, id);
    return {&registry_->at(id)};
  }
  const Registry* registry_;
  bool shared_;
  std::map<std::string, std::map<std::string, long long>> used_;
};

// The faults of a declared relation of control: a controller that is unknown, and operatives that control one another in a
// cycle. Control is a function: joint control is declared as control by one party (an organisation of the controllers).
inline std::vector<std::string> control_faults(const Parties& ps) {
  std::vector<std::string> out;
  for (const auto& [id, p] : ps.all()) {
    if (p.controller.empty()) continue;
    if (!ps.has(p.controller)) {
      out.push_back(id + " names an unknown controller, " + p.controller);
      continue;
    }
    std::set<std::string> seen{id};
    std::string cursor = p.controller;
    while (ps.has(cursor) && ps.at(cursor).kind == Kind::operative && !ps.at(cursor).controller.empty()) {
      if (!seen.insert(cursor).second) {
        out.push_back(id + " is controlled along a cycle of operatives");
        break;
      }
      cursor = ps.at(cursor).controller;
    }
  }
  return out;
}

// Proposition 3 as a process, for one verifier remote from the place where status is kept: a charge of `cost` is attempted
// every turn; the verifier checks status at every multiple of `window` and learns there of a revocation entered before the
// check; between two checks it admits at most `slice` (0: no slice) in the resource's unit; nothing beyond `budget`. Returns what
// was charged at or after the revocation. The place that keeps the status (the community's own fold) knows a revocation at once.
inline long long exposure(long long budget, long long slice, long long cost, long long revoked_at, long long window, long long turns = 400) {
  const long long bound = slice > 0 ? slice : std::numeric_limits<long long>::max() / 4;
  long long remaining = budget, in_window = 0, lost = 0;
  bool known = false;
  for (long long turn = 0; turn < turns; turn++) {
    if (turn % window == 0) {
      in_window = 0;
      known = turn >= revoked_at;
    }
    if (known || remaining < cost || in_window + cost > bound) continue;
    remaining -= cost;
    in_window += cost;
    if (turn >= revoked_at) lost += cost;
  }
  return lost;
}

}  // namespace thesmos::mandates
