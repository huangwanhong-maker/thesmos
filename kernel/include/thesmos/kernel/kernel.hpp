// The kernel of thesmos: parties of declared kinds, the relations of control and provision, and origins under a criterion.
//
// A party is an identified participant of one declared kind: a person, an organisation, an operative (an identity under which
// a program acts, today a session of a language model with its tools) or a community. Two relations among parties are
// declared facts: control (the party that holds an operative's credentials and configuration) and provision (the provider of
// the model it runs). The origin of a party under control is the first party reached along control that is not an operative;
// under provision, its provider. Sameness of origin is sameness under a named criterion, and parties whose traces coincide
// stay distinct until a declared relation joins them (the design record of 2026-11-25, Definitions 1 to 3).

#pragma once

#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>

namespace thesmos::kernel {

enum class Kind { person, organisation, operative, community };

inline std::string_view word(Kind k) {
  switch (k) {
    case Kind::person: return "person";
    case Kind::organisation: return "organisation";
    case Kind::operative: return "operative";
    case Kind::community: return "community";
  }
  return "?";
}

inline std::optional<Kind> kind_named(std::string_view w) {
  if (w == "person") return Kind::person;
  if (w == "organisation") return Kind::organisation;
  if (w == "operative") return Kind::operative;
  if (w == "community") return Kind::community;
  return std::nullopt;
}

// Whether a party of this kind can answer for an act: every kind but the operative.
inline bool answerable(Kind k) { return k != Kind::operative; }

struct Party {
  std::string id;
  Kind kind = Kind::person;
  std::string controller;  // of an operative: the party that holds its credentials and configuration (declared)
  std::string provider;    // of an operative: the provider and model it runs (declared)
};

class Parties {
 public:
  void add(Party p) {
    if (p.id.empty()) throw std::invalid_argument("thesmos::kernel: a party needs an identifier");
    const std::string id = p.id;
    by_id_[id] = std::move(p);
  }
  bool has(const std::string& id) const { return by_id_.count(id) > 0; }
  const Party& at(const std::string& id) const {
    const auto it = by_id_.find(id);
    if (it == by_id_.end()) throw std::out_of_range("thesmos::kernel: the party " + id + " is unknown");
    return it->second;
  }
  Party& at(const std::string& id) {
    const auto it = by_id_.find(id);
    if (it == by_id_.end()) throw std::out_of_range("thesmos::kernel: the party " + id + " is unknown");
    return it->second;
  }
  const std::map<std::string, Party>& all() const { return by_id_; }

 private:
  std::map<std::string, Party> by_id_;
};

enum class Criterion { control, provision };

// The origin of a party under a criterion. Under control, the first party reached along declared controllers that is not an
// operative; an operative whose controller is undeclared, unknown, or reached again is its own origin. Under provision, an
// operative's declared provider, and any other party itself.
inline std::string origin(const Parties& ps, std::string id, Criterion c = Criterion::control) {
  if (c == Criterion::provision) {
    const Party& p = ps.at(id);
    return p.kind == Kind::operative && !p.provider.empty() ? p.provider : id;
  }
  std::set<std::string> seen;
  while (true) {
    const Party& p = ps.at(id);
    if (p.kind != Kind::operative || p.controller.empty() || !ps.has(p.controller) || !seen.insert(id).second) return id;
    id = p.controller;
  }
}

}  // namespace thesmos::kernel
