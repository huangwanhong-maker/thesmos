// The register of norms: four facets read from a chronicle, which vary independently.
//
// Regularity is the share of applicable episodes in which conduct conforms; operation is the number of decisions that invoked
// the norm (acts of answer, acceptance, admission or conversion citing it); adoption is an act of amendment naming it;
// justification is the reasons recorded for it and the challenges to it. Two combinations are marked: a norm invoked with no
// act of adoption is "operative, unadopted" (a provider's default, a template, a steward's habit deciding cases ahead of any
// adoption), and one adopted and never invoked is "adopted, inoperative" (the Evaluative Authority paper; the design record of
// 2026-11-25, Section 5.1).

#pragma once

#include <thesmos/acts/acts.hpp>

#include <morphe/arithmetic/rational.hpp>

#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace thesmos::norms {

using acts::Act;
using morphe::arithmetic::Rational;

struct Norm {
  std::string id;
  std::function<bool(const Act&)> applies;   // the episodes it bears on
  std::function<bool(const Act&)> complies;  // conduct that conforms, in an applicable episode
};

struct Facets {
  std::optional<Rational> regularity;  // absent when no episode applies
  long long episodes = 0, invoked = 0, reasons = 0, challenges = 0;
  bool adopted = false;
  std::vector<std::string> marks;
};

namespace detail {
// Whether a comma-separated list of names holds `id`.
inline bool cites(const std::string& list, const std::string& id) {
  std::stringstream in(list);
  for (std::string item; std::getline(in, item, ',');) {
    while (!item.empty() && item.front() == ' ') item.erase(item.begin());
    if (item == id) return true;
  }
  return false;
}
}  // namespace detail

inline Facets facets(const Norm& n, const std::vector<Act>& chronicle) {
  Facets f;
  long long complying = 0;
  for (const Act& a : chronicle) {
    if (n.applies && n.applies(a)) {
      f.episodes++;
      complying += n.complies && n.complies(a) ? 1 : 0;
    }
    if (a.kind == "amend" && a.field("norm") == n.id) f.adopted = true;
    if ((a.kind == "answer" || a.kind == "accept" || a.kind == "admit" || a.kind == "convert") && detail::cites(a.field("cites"), n.id)) f.invoked++;
    if ((a.kind == "propose" || a.kind == "say") && a.field("about") == n.id && !a.field("reason").empty()) f.reasons++;
    if (a.kind == "claim" && a.field("about") == n.id) f.challenges++;
  }
  if (f.episodes) f.regularity = Rational(complying) / Rational(f.episodes);
  if (f.invoked && !f.adopted) f.marks.push_back("operative, unadopted");
  if (f.adopted && !f.invoked) f.marks.push_back("adopted, inoperative");
  return f;
}

}  // namespace thesmos::norms
