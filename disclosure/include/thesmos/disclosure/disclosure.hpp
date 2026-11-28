// Disclosure agreements (decision 0010): epistemic behaviour enters a community's record only by disclosure under an agreement
// between an agent's principal and the community, ratified by each side under its own rule and entered in the community's chronicle.
//
// The terms, written as noetarium.disclosure/1 in the field "terms" of each side's act of ratification:
//
//   {"format": "noetarium.disclosure/1", "id": "scribe reasoning", "principal": "ana", "agents": ["ana/scribe"],
//    "scope": {"kinds": ["reasoning", "model call"], "scenes": [], "matters": [], "from": 0, "to": 40},
//    "purpose": {"admits": ["review", "good faith"], "never": ["intent"]},
//    "custody": {"holding": "scribe's reasoning", "custodian": "steward", "retention": 20},
//    "access": {"principal": true, "body": "stewards"},
//    "cost": {"borne by": "principal", "unit": "bytes", "ceiling": 200000},
//    "withdrawal": {"by": "either", "notice": 2, "then": "erased"}}
//
//   scope       which epistemic behaviour (reasoning, a plan, a deliberation, a model call's prompt and answer), of which agents of the
//               principal (none named: every operative it controls), in which scenes and matters (none named: any), for which turns;
//   purpose     the uses the record admits (review of one's own work, evidence of good faith, of intent, research, a dispute), and those
//               it never admits;
//   custody     the holding of kind "epistemic record" that keeps it, its custodian (an office of the community or a party), and the
//               turns it is kept after the agreement ends;
//   access      who may consult it: the principal (with the operatives it controls), and the positions of a body the community declares;
//   cost        who bears the cost of recording and keeping it, and a ceiling on what is recorded in a unit;
//   withdrawal  who may denounce it, the turns of notice before the denunciation takes effect, and what then becomes of what was
//               recorded: kept to the end of its retention, returned to the principal alone, or erased.
//
// The norms of access to the record are holdings' norms (decision 0006), one permission and, where the terms forbid uses, one
// prohibition for each who may consult, each for the purposes it names: the most specific norm decides, and a prohibition prevails at
// equal specificity, so that a use the agreement never admits is refused whatever else permits it.

#pragma once

#include <thesmos/holdings/holdings.hpp>

#include <symploke/jsonld/json.hpp>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

namespace thesmos::disclosure {

using Json = symploke::jsonld::Json;

// The vocabularies the ground knows.
inline const std::set<std::string>& kinds() {
  static const std::set<std::string> s = {"reasoning", "plan", "deliberation", "model call"};
  return s;
}
inline const std::set<std::string>& purposes() {
  static const std::set<std::string> s = {"review", "good faith", "intent", "research", "dispute"};
  return s;
}

struct Terms {
  std::string format, id, principal;
  std::set<std::string> agents;           // the operatives it covers; none named: every operative the principal controls
  std::set<std::string> kinds;            // of epistemic behaviour
  std::set<std::string> scenes, matters;  // none named: any
  long long from = 0, to = -1;            // the turns it covers; to = -1: no end
  std::set<std::string> admits, never;    // purposes
  std::string holding, custodian;         // the record's holding, and who keeps it
  long long retention = 0;                // turns the record is kept after the agreement ends
  bool principal_reads = true;            // the principal (with the operatives it controls) may consult it
  std::string body;                       // the body whose positions may consult it, if any
  std::string borne_by = "principal";     // principal, community or shared
  std::string unit = "bytes";
  long long ceiling = -1;                 // the most recorded under it, in its unit; -1: no ceiling
  std::string by = "either";              // who may denounce: either, principal or community
  long long notice = 0;                   // turns from a denunciation to its effect
  std::string then = "kept";              // what becomes of the record when the agreement ends: kept, returned or erased
  std::vector<std::string> unread;        // parts of unexpected shape
};

namespace detail {
inline std::string text(const Json* j) { return j && j->is_string() ? j->text : std::string(); }
inline std::set<std::string> strings(const Json* j, std::vector<std::string>& unread, const std::string& label) {
  std::set<std::string> v;
  if (!j) return v;
  if (!j->is_array()) {
    unread.push_back(label + " is not a list");
    return v;
  }
  for (const Json& i : j->items) {
    if (i.is_string()) v.insert(i.text);
    else unread.push_back(label + " holds something that is not a name");
  }
  return v;
}
inline long long whole(const Json* j, long long otherwise, std::vector<std::string>& unread, const std::string& label) {
  if (!j) return otherwise;
  if (!j->is_number() || j->lexical.empty() || j->lexical.find_first_not_of("0123456789-") != std::string::npos || j->lexical.size() > 18) {
    unread.push_back(label + " is not a whole number");
    return otherwise;
  }
  return std::stoll(j->lexical);
}
}  // namespace detail

// Reads the terms from their written form; what does not read is noted in `unread`.
inline Terms read(const Json& j) {
  using detail::text;
  Terms t;
  if (!j.is_object()) {
    t.unread.push_back("the terms are not an object");
    return t;
  }
  t.format = text(j.get("format")), t.id = text(j.get("id")), t.principal = text(j.get("principal"));
  t.agents = detail::strings(j.get("agents"), t.unread, "the agents");
  if (const Json* s = j.get("scope"); s && s->is_object()) {
    t.kinds = detail::strings(s->get("kinds"), t.unread, "the kinds in scope");
    t.scenes = detail::strings(s->get("scenes"), t.unread, "the scenes in scope");
    t.matters = detail::strings(s->get("matters"), t.unread, "the matters in scope");
    t.from = detail::whole(s->get("from"), 0, t.unread, "the first turn in scope");
    t.to = detail::whole(s->get("to"), -1, t.unread, "the last turn in scope");
  } else
    t.unread.push_back("the terms declare no scope");
  if (const Json* p = j.get("purpose"); p && p->is_object()) {
    t.admits = detail::strings(p->get("admits"), t.unread, "the purposes admitted");
    t.never = detail::strings(p->get("never"), t.unread, "the purposes never admitted");
  } else
    t.unread.push_back("the terms declare no purpose");
  if (const Json* c = j.get("custody"); c && c->is_object()) {
    t.holding = text(c->get("holding")), t.custodian = text(c->get("custodian"));
    t.retention = detail::whole(c->get("retention"), 0, t.unread, "the retention");
  } else
    t.unread.push_back("the terms declare no custody");
  if (const Json* a = j.get("access"); a && a->is_object()) {
    if (const Json* p = a->get("principal"); p && p->is_bool()) t.principal_reads = p->truth;
    t.body = text(a->get("body"));
  }
  if (const Json* c = j.get("cost"); c && c->is_object()) {
    if (c->has("borne by")) t.borne_by = text(c->get("borne by"));
    if (c->has("unit")) t.unit = text(c->get("unit"));
    t.ceiling = detail::whole(c->get("ceiling"), -1, t.unread, "the ceiling");
  }
  if (const Json* w = j.get("withdrawal"); w && w->is_object()) {
    if (w->has("by")) t.by = text(w->get("by"));
    t.notice = detail::whole(w->get("notice"), 0, t.unread, "the notice");
    if (w->has("then")) t.then = text(w->get("then"));
  }
  return t;
}
inline Terms read(std::string_view written) {
  try {
    return read(symploke::jsonld::parse(written));
  } catch (const std::exception& e) {
    Terms t;
    t.unread.push_back(std::string("the terms are not JSON: ") + e.what());
    return t;
  }
}

// The digest that identifies the terms: the SHA-256 of their canonical JSON (RFC 8785), so that two writings of the same terms share it.
inline std::string digest(std::string_view written) {
  try {
    return archeion::fixity::sha256_hex(symploke::jsonld::canonical(symploke::jsonld::parse(written)));
  } catch (const std::exception&) {
    return {};
  }
}

// Every fault of form of the terms, each named; what they say of the community (its positions, bodies, parties) is the fold's to judge.
inline std::vector<std::string> faults(const Terms& t) {
  std::vector<std::string> out = t.unread;
  if (t.format != "noetarium.disclosure/1") out.push_back("the terms' format is not noetarium.disclosure/1");
  if (t.id.empty()) out.push_back("the agreement has no identifier");
  if (t.principal.empty()) out.push_back("the agreement names no principal");
  if (t.kinds.empty()) out.push_back("the agreement covers no kind of epistemic behaviour");
  for (const auto& k : t.kinds)
    if (!kinds().count(k)) out.push_back("the agreement covers " + k + ", which is no kind of epistemic behaviour: reasoning, plan, deliberation or model call");
  if (t.from < 0 || (t.to >= 0 && t.to < t.from)) out.push_back("the agreement's turns run from its first to its last");
  if (t.admits.empty()) out.push_back("the agreement admits no use of what it records");
  for (const auto& p : t.admits)
    if (!purposes().count(p)) out.push_back("the agreement admits the use " + p + ", which is none the ground knows: review, good faith, intent, research or dispute");
  for (const auto& p : t.never) {
    if (!purposes().count(p)) out.push_back("the agreement never admits " + p + ", which is no use the ground knows");
    if (t.admits.count(p)) out.push_back("the agreement both admits and never admits the use " + p);
  }
  if (t.holding.empty()) out.push_back("the agreement names no holding that keeps its record");
  if (t.custodian.empty()) out.push_back("the agreement names no custodian of its record");
  if (t.retention < 0) out.push_back("the record is kept no fewer than nought turns after the agreement ends");
  if (!t.principal_reads && t.body.empty()) out.push_back("the agreement lets no one consult its record");
  if (t.borne_by != "principal" && t.borne_by != "community" && t.borne_by != "shared") out.push_back("the cost is borne by the principal, the community, or both shared");
  if (t.unit.empty()) out.push_back("the cost names its unit");
  if (t.by != "either" && t.by != "principal" && t.by != "community") out.push_back("the agreement is denounced by either side, the principal or the community");
  if (t.notice < 0) out.push_back("a denunciation's notice is no fewer than nought turns");
  if (t.then != "kept" && t.then != "returned" && t.then != "erased") out.push_back("what was recorded is kept, returned to the principal, or erased when the agreement ends");
  return out;
}

// Whether the terms cover an epistemic episode: its party one of the agents named (or, when none is named, an operative whose origin is
// the principal), its kind in scope, its scene and matter in scope where the terms name any, and its turn within the agreement's turns.
inline bool covers(const Terms& t, const std::string& party, const std::string& origin, const std::string& kind, const std::string& scene, const std::string& matter, long long turn) {
  const bool agent = t.agents.empty() ? origin == t.principal && party != t.principal : t.agents.count(party) > 0;
  if (!agent || !t.kinds.count(kind)) return false;
  if (!t.scenes.empty() && !t.scenes.count(scene)) return false;
  if (!t.matters.empty() && !t.matters.count(matter)) return false;
  return turn >= t.from && (t.to < 0 || turn <= t.to);
}

// The norms of access to the record (decision 0006): for the principal (the attribute party, which binds the operatives it controls and
// whoever acts under its mandates too) and for each position of the body, a permission to consult for the purposes admitted and, where the terms forbid uses, a prohibition
// for them.
inline std::vector<holdings::Norm> norms(const Terms& t, const std::vector<std::string>& body_positions) {
  std::vector<holdings::Norm> out;
  std::vector<holdings::Attribute> who;
  if (t.principal_reads) who.push_back({holdings::Who::party, t.principal});
  for (const auto& p : body_positions) who.push_back({holdings::Who::position, p});
  std::size_t k = 0;
  for (const auto& a : who) {
    holdings::Norm n;
    n.id = "agreement " + t.id + " " + std::to_string(++k);
    n.attribute = a, n.deontic = holdings::Deontic::may, n.aim = holdings::Aim::consult;
    n.object = {holdings::Over::holding, t.holding};
    n.conditions.purposes = t.admits;
    out.push_back(n);
    if (!t.never.empty()) {
      holdings::Norm m = n;
      m.id = "agreement " + t.id + " " + std::to_string(++k);
      m.deontic = holdings::Deontic::must_not, m.conditions.purposes = t.never;
      out.push_back(m);
    }
  }
  return out;
}

}  // namespace thesmos::disclosure
