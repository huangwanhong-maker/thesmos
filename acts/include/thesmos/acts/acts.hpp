// Acts: a declared force, a typed envelope that the system reads, and a free body that persons and models read.
//
// The five forces are Searle's classes of illocution (A Taxonomy of Illocutionary Acts, 1975): an expression, an assertion, a
// directive, a commitment, and a declaration made by virtue of a power. The ground fixes the kinds of act and the force of each.
// An act's consequences follow from its envelope alone: its author, kind, turn, mandate, typed fields, the mandate it issues,
// the charges it makes and the first holders it names. Its body enters the seal and stays out of the fold, which is also the
// defence against instructions carried inside messages (the design record of 2026-11-25, Definitions 5 and 6, idea 2).
//
// The mandate an act issues is written into its envelope part by part; the parts passports added (decision 0007: the communities in
// which it may be used, its holder's credential by digest, a reservation, a term in instants) are written only when set, so that every
// envelope written before them is written again exactly, and every seal stands.

#pragma once

#include <thesmos/mandates/mandates.hpp>

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace thesmos::acts {

enum class Force { expressive, assertive, directive, commissive, declarative };

inline std::string_view word(Force f) {
  switch (f) {
    case Force::expressive: return "expressive";
    case Force::assertive: return "assertive";
    case Force::directive: return "directive";
    case Force::commissive: return "commissive";
    case Force::declarative: return "declarative";
  }
  return "?";
}

// The kinds of act of the ground and their forces.
inline const std::map<std::string, Force>& ground() {
  static const std::map<std::string, Force> table = {
      {"say", Force::expressive},        {"acknowledge", Force::expressive},
      {"report", Force::assertive},      {"deliver", Force::assertive},     {"assess", Force::assertive},
      {"attest", Force::assertive},      {"hand over", Force::assertive},
      {"request", Force::directive},     {"claim", Force::directive},       {"propose", Force::directive},
      {"offer", Force::commissive},      {"take", Force::commissive},       {"commit", Force::commissive},
      {"grant", Force::commissive},
      {"found", Force::declarative},     {"join", Force::declarative},      {"admit", Force::declarative},
      {"leave", Force::declarative},     {"mandate", Force::declarative},   {"revoke", Force::declarative},
      {"accept", Force::declarative},    {"release", Force::declarative},   {"cancel", Force::declarative},
      {"assign", Force::declarative},    {"use", Force::declarative},       {"answer", Force::declarative},
      {"convert", Force::declarative},   {"vote", Force::declarative},      {"amend", Force::declarative},
      // access to a community's holdings, each made by virtue of a norm of access (decision 0006); "use" serves grants and holdings
      {"consult", Force::declarative},   {"deposit", Force::declarative},   {"modify", Force::declarative},
      {"lend", Force::declarative},      {"withdraw", Force::declarative},
      // a passport presented at a host, and a community's recognition of another in levels (decision 0009)
      {"present", Force::assertive},     {"recognise", Force::declarative},
      // a side's ratification of a disclosure agreement, and its denunciation (decision 0010)
      {"agree", Force::commissive},      {"denounce", Force::declarative},
  };
  return table;
}

inline std::optional<Force> force_of(const std::string& kind) {
  const auto it = ground().find(kind);
  return it == ground().end() ? std::nullopt : std::optional<Force>(it->second);
}

struct Charge {
  std::string mandate, resource;
  long long amount = 0;
};

struct Act {
  std::string author, kind;
  long long turn = 0;
  std::string when;     // an instant, ISO 8601 in UTC; when empty, the act is placed by its turn
  std::string mandate;  // the mandate the author acts under, if any
  std::map<std::string, std::string> fields;                   // the typed envelope beyond author, kind, turn and mandate
  std::vector<std::pair<std::string, std::string>> holders;    // a founding's first holders: party, position
  std::optional<mandates::Mandate> grant;                      // the mandate an act of kind "mandate" issues
  std::vector<Charge> charges;                                 // use of the act's mandate's allowance
  std::string body;                                            // free text, read by persons and models, never by the fold

  const std::string& field(const std::string& k) const {
    static const std::string none;
    const auto it = fields.find(k);
    return it == fields.end() ? none : it->second;
  }
  bool has(const std::string& k) const { return fields.count(k) > 0; }
  long long number(const std::string& k, long long otherwise) const {
    const auto it = fields.find(k);
    return it == fields.end() || it->second.empty() ? otherwise : std::stoll(it->second);
  }
};

namespace detail {
inline void put(std::string& s, std::string_view key, std::string_view value) {
  s += key;
  s += ':';
  s += std::to_string(value.size());
  s += ':';
  s += value;
  s += ';';
}
inline std::string joined(const std::set<std::string>& items) {
  std::string s;
  for (const auto& i : items) put(s, "", i);
  return s;
}
}  // namespace detail

// The canonical text of an act's envelope: every typed part, each value prefixed by its length, in a fixed order, so that two
// envelopes have one text exactly when they are equal. The body is left out; the seal takes it apart.
inline std::string envelope(const Act& a) {
  using detail::put;
  std::string s;
  put(s, "author", a.author);
  put(s, "kind", a.kind);
  put(s, "turn", std::to_string(a.turn));
  put(s, "when", a.when);
  put(s, "mandate", a.mandate);
  for (const auto& [k, v] : a.fields) put(s, "field." + k, v);
  for (const auto& [party, position] : a.holders) put(s, "holder", party + "\x1e" + position);
  if (a.grant) {
    const mandates::Mandate& m = *a.grant;
    std::string g;
    put(g, "id", m.id);
    put(g, "principal", m.principal);
    put(g, "holder", m.holder);
    put(g, "kinds", detail::joined(m.kinds));
    put(g, "matters", detail::joined(m.matters));
    for (const auto& [r, n] : m.allowance) put(g, "allowance." + r, std::to_string(n));
    put(g, "term", std::to_string(m.from) + "-" + std::to_string(m.to));
    put(g, "delegable", m.delegable ? "yes" : "no");
    put(g, "parent", m.parent);
    for (const auto& [r, n] : m.slice) put(g, "slice." + r, std::to_string(n));
    // The parts passports add (decision 0007), written only when set, so that a mandate without them keeps its text and its seal.
    if (!m.communities.empty()) put(g, "communities", detail::joined(m.communities));
    if (!m.credential.empty()) put(g, "credential", m.credential);
    if (m.reserves) put(g, "reserves", "yes");
    if (!m.since.empty() || !m.until.empty()) put(g, "instants", m.since + "/" + m.until);
    put(s, "grant", g);
  }
  for (const Charge& c : a.charges) put(s, "charge", c.mandate + "\x1e" + c.resource + "\x1e" + std::to_string(c.amount));
  return s;
}

}  // namespace thesmos::acts
