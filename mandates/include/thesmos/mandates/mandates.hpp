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
//
// Passports (decision 0007) add four parts, each written into an envelope only when it is set, so that every mandate issued before
// them keeps its text and its seal: the communities in which a mandate may be used beyond its home; the SHA-256 digest of its
// holder's bearer credential; a term in instants beside the term in turns; and whether it reserves, so that its ceilings are charged
// against the links above it when it is issued (one mandate a host, after the design's Definition 39). A reserving link's ancestors
// hold its reservation, so what is later charged beneath it is charged from it down, and never twice above it (Proposition 12 of the
// theory note).

#pragma once

#include <thesmos/kernel/kernel.hpp>

#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
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
  // Passports (decision 0007).
  std::set<std::string> communities;            // where it may be used beyond its home; empty: none named, which narrowing reads
                                                // as unrestricted and presentation as nowhere outside the home
  std::string credential;                       // the SHA-256 digest of the holder's bearer credential, 64 lower-case hexadecimal
                                                // digits; empty: none
  bool reserves = false;                        // its ceilings are charged against the links above it when it is issued
  std::string since, until;                     // a term in instants, ISO 8601 in UTC (YYYY-MM-DDTHH:MM:SSZ); empty: unbounded
};

using Registry = std::map<std::string, Mandate>;

// Whether a text is an instant as mandates write their terms: ISO 8601 in UTC, YYYY-MM-DDTHH:MM:SSZ, which sorts as text.
inline bool is_instant(std::string_view s) {
  if (s.size() != 20 || s[4] != '-' || s[7] != '-' || s[10] != 'T' || s[13] != ':' || s[16] != ':' || s[19] != 'Z') return false;
  for (std::size_t k : {0, 1, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18})
    if (s[k] < '0' || s[k] > '9') return false;
  const int month = (s[5] - '0') * 10 + (s[6] - '0'), day = (s[8] - '0') * 10 + (s[9] - '0');
  const int hour = (s[11] - '0') * 10 + (s[12] - '0'), minute = (s[14] - '0') * 10 + (s[15] - '0'), second = (s[17] - '0') * 10 + (s[18] - '0');
  return month >= 1 && month <= 12 && day >= 1 && day <= 31 && hour <= 23 && minute <= 59 && second <= 60;
}

// Whether an instant lies within a mandate's term in instants; a mandate that declares no bound is unbounded on that side.
inline bool within_instants(const Mandate& m, const std::string& at) { return (m.since.empty() || m.since <= at) && (m.until.empty() || at <= m.until); }

// Whether a mandate names a community among those in which it may be used beyond its home.
inline bool names(const Mandate& m, const std::string& community) { return m.communities.count(community) > 0; }

// Whether a child is a split of its parent: issued by the parent's principal for the parent's own holder, reserving its ceilings.
inline bool split(const Mandate& parent, const Mandate& child) { return child.reserves && child.holder == parent.holder && child.principal == parent.principal; }

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
  if (root.reserves) out.push_back(root.id + " reserves its ceilings and has no parent to reserve them against");
  for (const Mandate* link : links) {
    const bool formed = (link->since.empty() || is_instant(link->since)) && (link->until.empty() || is_instant(link->until));
    if (!formed) out.push_back(link->id + " states its term in instants other than as ISO 8601 in UTC (YYYY-MM-DDTHH:MM:SSZ)");
    else if (!link->since.empty() && !link->until.empty() && link->until < link->since) out.push_back(link->id + " ends its term in instants before it begins");
  }
  for (std::size_t k = 1; k < links.size(); k++) {
    const Mandate& parent = *links[k - 1];
    const Mandate& child = *links[k];
    // A link is a delegation by its parent's holder, which the parent must allow; or a split, by which the parent's principal divides
    // the parent among parts for the same holder (one for each host), each reserving its ceilings so that the parts never pass the
    // whole (decision 0007, Definition 20 of the theory note).
    if (!split(parent, child)) {
      if (child.principal != parent.holder) out.push_back(child.id + " was not issued by the holder of " + parent.id);
      if (!parent.delegable) out.push_back(parent.id + " may not be delegated");
    }
    if (!std::includes(parent.kinds.begin(), parent.kinds.end(), child.kinds.begin(), child.kinds.end())) out.push_back(child.id + " widens the kinds of act");
    if (!std::includes(parent.matters.begin(), parent.matters.end(), child.matters.begin(), child.matters.end())) out.push_back(child.id + " widens the matters");
    for (const auto& [resource, amount] : child.allowance) {
      const auto it = parent.allowance.find(resource);
      if (it == parent.allowance.end() || amount > it->second) out.push_back(child.id + " widens the allowance of " + resource);
    }
    if (!(parent.from <= child.from && child.to <= parent.to)) out.push_back(child.id + " widens the term");
    // Passports: a link that names communities narrows to some of them; a bound in instants is kept or tightened.
    if (!parent.communities.empty() &&
        (child.communities.empty() || !std::includes(parent.communities.begin(), parent.communities.end(), child.communities.begin(), child.communities.end())))
      out.push_back(child.id + " widens the communities in which it may be used");
    if ((!parent.since.empty() && (child.since.empty() || child.since < parent.since)) || (!parent.until.empty() && (child.until.empty() || child.until > parent.until)))
      out.push_back(child.id + " widens the term in instants");
  }
  for (const Mandate* link : links)
    if (revoked.count(link->id)) out.push_back(link->id + " is revoked");
  return out;
}

// The links a charge made under `id` is charged against, root first: from the deepest reserving link of its chain down to `id`,
// whose ancestors hold that link's reservation already, or the whole chain when no link reserves (so for every mandate issued
// before decision 0007, the chain itself).
inline std::vector<const Mandate*> charged_links(const Registry& r, const std::string& id) {
  std::vector<const Mandate*> links = chain(r, id);
  std::size_t from = 0;
  for (std::size_t k = 0; k < links.size(); k++)
    if (links[k]->reserves) from = k;
  return {links.begin() + static_cast<std::ptrdiff_t>(from), links.end()};
}

// The links a reserving mandate's ceilings are charged against when it is issued: those a charge under its parent reaches. Empty for
// a mandate that does not reserve.
inline std::vector<const Mandate*> reserved_against(const Registry& r, const Mandate& m) {
  if (!m.reserves || m.parent.empty()) return {};
  return charged_links(r, m.parent);
}

// Use charged along the whole chain, so that each ceiling bounds everything beneath it. With `shared` false each mandate is
// charged alone, which reproduces the fault of independent ceilings.
class Envelopes {
 public:
  explicit Envelopes(const Registry& r, bool shared = true) : registry_(&r), shared_(shared) {}

  // Charges a reserving mandate's ceilings against the links above it when it is issued (Definition 20 of the theory note), when every
  // one of them has room for every ceiling; changes nothing and returns false otherwise, or when the mandate does not reserve.
  bool reserve(const Mandate& m) {
    if (!m.reserves || m.parent.empty()) return false;
    const auto above = reserved_against(*registry_, m);
    for (const auto& [resource, ceiling] : m.allowance)
      for (const Mandate* link : above) {
        const auto c = link->allowance.find(resource);
        if (c == link->allowance.end() || used(link->id, resource) + ceiling > c->second) return false;
      }
    for (const auto& [resource, ceiling] : m.allowance)
      for (const Mandate* link : above) used_[link->id][resource] += ceiling;
    return true;
  }

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
    if (shared_) return charged_links(*registry_, id);
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
