// Institutional statements: rules written in words, read by a grammar, and measured against a chronicle.
//
// A regulative statement has the parts of the grammar of institutions (Crawford and Ostrom, A Grammar of Institutions, American
// Political Science Review 89, 1995; Frantz and Siddiki, Institutional Grammar 2.0, Public Administration 99, 2021): an attribute
// (the position it binds), a deontic (may, must, must never), an aim (a kind of act of the ground), an object, conditions (here the
// act that calls for it and the term within which it is due) and an or-else (what follows when it is not done). Its written form:
//
//   steward must answer claim after a claim within 1 round or else the claim stands unanswered on the card.
//   notary must never vote.
//   contributor may offer.
//
// The grammar is written in BNF and read by morphe's reader, the aims being the ground's kinds of act, so a statement that is not of
// this form is refused with the place where it stops and what could follow. A duty with a condition and a term becomes a norm over the
// chronicle: each act that calls for it is an episode, which conforms when a holder of the attribute makes the aim on the same matter
// within the term. Its four facets are then measured by thesmos's norms, and a constitution's own choice rules are read as
// statements, so that the rules a community declared are the rules whose regularity is measured.

#pragma once

#include <thesmos/acts/acts.hpp>
#include <thesmos/constitutions/constitutions.hpp>
#include <thesmos/norms/norms.hpp>

#include <morphe/formal/bnf.hpp>

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::statements {

struct Statement {
  std::string attribute, deontic, aim, object, trigger, or_else;  // trigger: the kind of act that calls for the duty
  long long within = -1;                                         // the term in turns, or -1 for none
  std::string text() const {
    std::string t = attribute + " " + deontic + " " + aim;
    if (!object.empty()) t += " " + object;
    if (!trigger.empty()) t += " after " + noun_of(trigger);
    if (within >= 0) t += " within " + std::to_string(within) + (within == 1 ? " round" : " rounds");
    if (!or_else.empty()) t += " or else " + or_else;
    return t + ".";
  }
  // The noun phrase that names the act calling for a duty ("a claim" for an act of claim).
  static std::string noun_of(const std::string& kind) {
    for (const auto& [noun, k] : triggers())
      if (k == kind) return (noun[0] == 'a' || noun[0] == 'o' ? "an " : "a ") + noun;
    return "a " + kind;
  }
  static const std::vector<std::pair<std::string, std::string>>& triggers() {
    static const std::vector<std::pair<std::string, std::string>> t = {{"claim", "claim"},       {"delivery", "deliver"}, {"offer", "offer"},
                                                                     {"proposal", "propose"}, {"request", "request"}, {"commitment", "commit"},
                                                                     {"handover", "hand over"}};
    return t;
  }
};

struct Reading {
  bool read = false;
  Statement statement;
  std::string error;  // where the text stops being a statement, and what could follow
};

namespace detail {

inline std::string grammar_text() {
  std::string aims;
  for (const auto& [kind, force] : acts::ground()) aims += (aims.empty() ? "\"" : " | \"") + kind + "\"";
  std::string nouns;
  for (const auto& [noun, kind] : Statement::triggers()) nouns += (nouns.empty() ? "\"" : " | \"") + noun + "\"";
  return "statement ::= attribute \" \" deontic \" \" aim object? trigger? within? orelse? \".\"? ;\n"
         "attribute ::= [a-z] [a-z\\-]* ;\n"
         "deontic   ::= \"must never\" | \"must\" | \"may\" ;\n"
         "aim       ::= " + aims + " ;\n"
         "object    ::= \" \" noun ;\n"
         "noun      ::= [a-z]+ ;\n"
         "trigger   ::= \" after \" (\"a \" | \"an \") event ;\n"
         "event     ::= " + nouns + " ;\n"
         "within    ::= \" within \" number \" round\" \"s\"? ;\n"
         "number    ::= [0-9]+ ;\n"
         "orelse    ::= \" or else \" rest ;\n"
         "rest      ::= [^.]+ ;\n";
}

inline const morphe::formal::Bnf& grammar() {
  static const morphe::formal::Bnf g = morphe::formal::compile_bnf(grammar_text());
  return g;
}

inline std::string normal(std::string_view s) {
  std::string out;
  bool space = false;
  for (char c : s) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') space = !out.empty();
    else {
      if (space) out += ' ';
      out += c;
      space = false;
    }
  }
  return out;
}

}  // namespace detail

// Reads a statement from its written form.
inline Reading read(std::string_view written) {
  Reading r;
  const std::string text = detail::normal(written);
  const auto p = detail::grammar().parse(text);
  if (!p.accepted) {
    r.error = p.error;
    return r;
  }
  auto part = [&](const char* name) {
    const auto* n = p.tree.find(name);
    return n ? text.substr(n->begin, n->end - n->begin) : std::string();
  };
  Statement& s = r.statement;
  s.attribute = part("attribute");
  s.deontic = part("deontic");
  s.aim = part("aim");
  s.object = part("noun");
  const std::string event = part("event");
  for (const auto& [noun, kind] : Statement::triggers())
    if (noun == event) s.trigger = kind;
  if (const std::string n = part("number"); !n.empty()) s.within = std::stoll(n);
  if (p.tree.find("orelse")) s.or_else = part("rest");
  r.read = true;
  return r;
}

// The statements a constitution's choice rules make: one for each kind of act a position may, must or must never make. A duty's
// condition is read from its "when" (the first act the grammar names in it) and its term from its "within".
inline std::vector<Statement> of(const constitutions::Constitution& c) {
  std::vector<Statement> out;
  auto term = [](const std::string& within) -> long long {
    static const std::map<std::string, long long> words = {{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}};
    std::istringstream in(within);
    std::string first;
    in >> first;
    if (words.count(first)) return words.at(first);
    try {
      return first.empty() ? -1 : std::stoll(first);
    } catch (...) {
      return -1;
    }
  };
  for (const auto& rule : c.choice) {
    for (const auto& k : rule.may) out.push_back({rule.who, "may", k, {}, {}, {}, -1});
    for (const auto& k : rule.must_never) out.push_back({rule.who, "must never", k, {}, {}, {}, -1});
    for (const auto& k : rule.must) {
      Statement s{rule.who, "must", k, {}, {}, rule.or_else, term(rule.within)};
      for (const auto& [noun, kind] : Statement::triggers())
        if (s.trigger.empty() && rule.when.find(noun) != std::string::npos) s.trigger = kind, s.object = noun;
      out.push_back(s);
    }
  }
  return out;
}

// The norm a statement states over a chronicle. A duty with a condition and a term: each act of the condition is an episode, which
// conforms when a holder of the attribute makes the aim on the same matter (field "id") within the term. A prohibition: each act of
// the aim by a holder is an episode, which never conforms. `holds(party, position)` says who holds the attribute.
inline norms::Norm norm(const Statement& s, std::function<bool(const std::string&, const std::string&)> holds, const std::vector<acts::Act>& chronicle) {
  norms::Norm n;
  n.id = s.text();
  if (s.deontic == "must" && !s.trigger.empty()) {
    const std::string trigger = s.trigger, aim = s.aim, attribute = s.attribute;
    const long long within = s.within;
    n.applies = [trigger](const acts::Act& a) { return a.kind == trigger; };
    n.complies = [&chronicle, aim, attribute, within, holds](const acts::Act& episode) {
      for (const auto& a : chronicle)
        if (a.kind == aim && a.field("id") == episode.field("id") && a.turn >= episode.turn && (within < 0 || a.turn <= episode.turn + within) &&
            (attribute == "anyone" || holds(a.author, attribute)))
          return true;
      return false;
    };
  } else if (s.deontic == "must never") {
    const std::string aim = s.aim, attribute = s.attribute;
    n.applies = [aim, attribute, holds](const acts::Act& a) { return a.kind == aim && (attribute == "anyone" || holds(a.author, attribute)); };
    n.complies = [](const acts::Act&) { return false; };
  } else {
    n.applies = [](const acts::Act&) { return false; };
    n.complies = [](const acts::Act&) { return true; };
  }
  return n;
}

}  // namespace thesmos::statements
