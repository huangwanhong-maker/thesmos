// The behaviour journal and its join to the chronicle (decision 0008): the record of conduct, evidential, kept beside the chronicle of
// normative acts, constitutive, and joined to it act to episode.
//
// A node records every episode of conduct it sees (behaviour::Episode) in a journal sealed as archeion seals any, each episode followed
// by an entry naming its outcome: the seal of the act it constituted, the reason that act was refused, or that it constituted none. By
// the ground's basic counts-as (Definition 32 of the theory note), an episode that is a request carrying a well-formed envelope counts
// as an act of the envelope's kind when the fold admits it: the act carries in its field "episode" the seal of the episode's entry in
// the journal (a SHA-256 digest that chains every entry before, and so names that entry alone), and the journal's next entry names the
// act's seal. The fold never reads the journal (invariant G9): conduct reaches the institutional state
// only through the acts it constitutes, or acts of evidence that record it.
//
// Proposition 15: in a community whose acts are made through constitute, every admitted act is constituted by exactly one episode,
// which names the act's seal, and the act names that episode's entry; an episode constitutes at most one act; a refused act leaves an
// episode naming the reason and nothing in the chronicle. joined decides it on a chronicle and a journal, in both directions.

#pragma once

#include <thesmos/behaviour/levels.hpp>
#include <thesmos/chronicle/chronicle.hpp>

#include <archeion/fixity/journal.hpp>

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace thesmos::chronicle {

struct Conduct {
  archeion::fixity::Journal journal;
};

struct Constituted {
  bool admitted = false;
  std::string reason, episode, seal;
};

namespace detail {
inline std::string outcome(const std::string& episode, const std::string& what) { return "episode:" + episode + "\x1e" + what; }
inline std::string journal_instant(const Conduct& c, const std::string& at) {
  return c.journal.entries.empty() || c.journal.entries.back().when <= at ? at : c.journal.entries.back().when;
}
}  // namespace detail

// Records an episode that constitutes no act (a reading, a run, a model call), with its outcome.
inline std::string observe(Conduct& c, const behaviour::Episode& e, const std::string& at) {
  const std::string when = detail::journal_instant(c, at);
  const std::string d = c.journal.append({when, e.party, "episode", behaviour::canonical(e), {}, {}}).seal;
  c.journal.append({when, e.party, "outcome", detail::outcome(d, "no act"), {}, {}});
  return d;
}

// The basic counts-as: the episode recorded first, the act it carries submitted with the seal of the episode's entry in its field
// "episode", and the outcome recorded next, the act's seal or the reason it was refused.
inline Constituted constitute(State& s, Conduct& c, const behaviour::Episode& e, Act a) {
  const std::string when = detail::journal_instant(c, detail::instant(a));
  const std::string d = c.journal.append({when, e.party, "episode", behaviour::canonical(e), {}, {}}).seal;
  a.fields["episode"] = d;
  const Outcome o = step(s, a);
  const std::string seal = o.admitted ? s.journal.entries.back().seal : std::string();
  c.journal.append({when, e.party, "outcome", detail::outcome(d, o.admitted ? "seal:" + seal : "refused:" + o.reason), {}, {}});
  return {o.admitted, o.reason, d, seal};
}

// Whether a chronicle and a behaviour journal are joined act to episode (Proposition 15), in both directions: the journal seals; every
// episode is followed by its outcome; an outcome naming a seal names exactly one act of the chronicle, which carries the episode's
// digest and seals so; an outcome naming a refusal or no act leaves no act carrying the episode; and every act that carries an episode
// is named by that episode's outcome. Fails at the first place that breaks the join.
inline apodeixis::kernel::Verdict joined(const State& s, const Conduct& c) {
  using apodeixis::kernel::Verdict;
  const std::string claim = "the chronicle and the behaviour journal are joined act to episode";
  if (const auto v = archeion::fixity::verify(c.journal); !v.holds()) return Verdict::no(claim, "the journal does not seal: " + v.reason);
  std::map<std::string, std::vector<std::size_t>> by_episode;  // episode -> the places of the acts that carry it
  for (std::size_t k = 0; k < s.acts.size(); k++)
    if (s.acts[k].has("episode")) by_episode[s.acts[k].field("episode")].push_back(k);
  std::set<std::string> named;
  const auto& e = c.journal.entries;
  for (std::size_t k = 0; k < e.size(); k++) {
    if (e[k].what == "outcome" || e[k].what == "epistemic" || e[k].what == "reasoning") continue;  // epistemic entries join no act (decision 0010)
    if (e[k].what != "episode") return Verdict::no(claim, "entry " + std::to_string(k + 1) + " is neither an episode nor an outcome", k);
    const std::string d = e[k].seal;
    if (k + 1 == e.size() || e[k + 1].what != "outcome" || e[k + 1].about.rfind("episode:" + d + "\x1e", 0) != 0)
      return Verdict::no(claim, "the episode at entry " + std::to_string(k + 1) + " is not followed by its outcome", k);
    const std::string what = e[k + 1].about.substr(("episode:" + d + "\x1e").size());
    const auto acts_of = by_episode.find(d);
    const std::size_t carried = acts_of == by_episode.end() ? 0 : acts_of->second.size();
    if (what.rfind("seal:", 0) == 0) {
      if (carried != 1) return Verdict::no(claim, "the episode at entry " + std::to_string(k + 1) + " is carried by " + std::to_string(carried) + " acts of the chronicle, not one", k);
      const std::size_t place = acts_of->second.front();
      if (s.journal.entries[place].seal != what.substr(5)) return Verdict::no(claim, "the outcome at entry " + std::to_string(k + 2) + " names another seal than the act's", k + 1);
      named.insert(d);
    } else if (carried != 0)
      return Verdict::no(claim, "the episode at entry " + std::to_string(k + 1) + " constituted no act, and the chronicle holds one carrying it", k);
  }
  for (const auto& [d, places] : by_episode)
    if (!named.count(d)) return Verdict::no(claim, "the act at place " + std::to_string(places.front() + 1) + " carries an episode the journal does not name as its", places.front());
  return Verdict::yes(claim, "every act carrying an episode is the outcome of exactly that episode, and every episode's outcome stands in the chronicle", e.size());
}

// ---- Epistemic behaviour by agreement (decision 0010) -----------------------------------------------------------------------------
//
// An agent's epistemic episode (its reasoning, a plan, a deliberation, a model call's prompt and answer) enters the journal as level E
// only under a disclosure agreement in force that covers it: the entry "epistemic" names the agreement and the digest and size of what
// was disclosed, never the content itself, which the agreement's record keeps under its norms of access, so that an erasure leaves the
// journal sealed. Outside an agreement the journal keeps only that an episode of reasoning took place, its party and its turn (the entry
// "reasoning"). An epistemic entry is joined to the episode of conduct it preceded: the next episode of the same party in the journal.

struct Epistemic {
  std::string party, kind, scene, matter;
  long long turn = 0;
  std::string content;  // what is disclosed; only its digest enters the journal
  long long size = 0;   // in the agreement's unit of cost
};

struct Disclosed {
  bool recorded = false;  // level E, under an agreement
  std::string agreement, seal, digest, reason;
};

namespace detail {
inline std::vector<std::string> split(const std::string& s, char sep) {
  std::vector<std::string> out(1);
  for (char c : s) {
    if (c == sep) out.emplace_back();
    else out.back() += c;
  }
  return out;
}
// An epistemic entry's text: its fields in a fixed order, apart by the record separator, which no field may hold (the journal refuses the
// unit separator in any field).
inline std::string epistemic_text(const Epistemic& e, const std::string& bearer, const std::string& agreement, const std::string& digest) {
  return e.party + '\x1e' + bearer + '\x1e' + e.kind + '\x1e' + e.scene + '\x1e' + e.matter + '\x1e' + std::to_string(e.turn) + '\x1e' + agreement + '\x1e' + digest + '\x1e' +
         std::to_string(e.size);
}
// What has been recorded under an agreement, in its unit, by the journal's epistemic entries.
inline long long recorded_under(const Conduct& c, const std::string& agreement) {
  long long n = 0;
  for (const auto& x : c.journal.entries)
    if (x.what == "epistemic")
      if (const auto f = split(x.about, '\x1e'); f.size() == 9 && f[6] == agreement) n += std::stoll(f[8]);
  return n;
}
inline bool clean(const std::string& v) { return v.find('\x1f') == std::string::npos && v.find('\x1e') == std::string::npos; }
}  // namespace detail

// The agreement that covers an epistemic episode, with room under its ceiling for what it discloses, or the reason none does.
inline std::pair<std::string, std::string> covering(const State& s, const Conduct& c, const Epistemic& e) {
  std::string why = "no disclosure agreement in force covers it";
  for (const auto& [id, g] : s.agreements) {
    if (!in_force(g, e.turn) || !disclosure::covers(g.terms, e.party, origin_of(s, e.party), e.kind, e.scene, e.matter, e.turn)) continue;
    if (g.terms.ceiling >= 0 && detail::recorded_under(c, id) + e.size > g.terms.ceiling) {
      why = "the agreement " + id + " covers it, and its ceiling of " + std::to_string(g.terms.ceiling) + " " + g.terms.unit + " would be passed";
      continue;
    }
    return {id, {}};
  }
  return {{}, why};
}

// Records an epistemic episode: its disclosure under the agreement that covers it, or, where none does, only that reasoning took place.
inline Disclosed disclose(const State& s, Conduct& c, const Epistemic& e, const std::string& at) {
  Disclosed out;
  for (const std::string* v : {&e.party, &e.kind, &e.scene, &e.matter})
    if (!detail::clean(*v)) {
      out.reason = "an epistemic episode's party, kind, scene and matter hold no separator";
      return out;
    }
  if (!s.parties.has(e.party) || !disclosure::kinds().count(e.kind) || e.size < 0) {
    out.reason = "an epistemic episode is a known party's reasoning, plan, deliberation or model call, of a size no less than nought";
    return out;
  }
  const std::string when = detail::journal_instant(c, at);
  const auto [agreement, why] = covering(s, c, e);
  if (agreement.empty()) {
    out.reason = why;
    out.seal = c.journal.append({when, e.party, "reasoning", e.party + '\x1e' + std::to_string(e.turn), {}, {}}).seal;
    return out;
  }
  out.recorded = true, out.agreement = agreement, out.digest = archeion::fixity::sha256_hex(e.content);
  const std::string bearer = origin_of(s, e.party);
  out.seal = c.journal.append({when, e.party, "epistemic", detail::epistemic_text(e, bearer, agreement, out.digest), {}, {}}).seal;
  return out;
}

// The episode of conduct an epistemic entry preceded: the next episode of the same party in the journal, by its place, or none.
inline std::optional<std::size_t> preceded(const Conduct& c, std::size_t entry) {
  const auto& e = c.journal.entries;
  for (std::size_t k = entry + 1; k < e.size(); k++) {
    if (e[k].who != e[entry].who) continue;
    if (e[k].what == "episode") return k;
    if (e[k].what == "epistemic" || e[k].what == "reasoning") return std::nullopt;  // a later episode of reasoning comes first
  }
  return std::nullopt;
}

// Whether the journal keeps epistemic behaviour only by agreement (Proposition 24): it seals; every epistemic entry names an agreement
// of the chronicle in force at its turn that covers it, a digest, and a size within the agreement's ceiling as the entries before it
// stand; every entry of reasoning outside agreements holds its party and its turn and nothing else. Fails at the first entry that does not.
inline apodeixis::kernel::Verdict disclosed_by_agreement(const State& s, const Conduct& c) {
  using apodeixis::kernel::Verdict;
  const std::string claim = "the behaviour journal keeps epistemic behaviour only under the agreements that cover it";
  if (const auto v = archeion::fixity::verify(c.journal); !v.holds()) return Verdict::no(claim, "the journal does not seal: " + v.reason);
  std::map<std::string, long long> used;
  std::size_t epistemic = 0, reasoning = 0;
  const auto& e = c.journal.entries;
  for (std::size_t k = 0; k < e.size(); k++) {
    const std::string at = "entry " + std::to_string(k + 1);
    if (e[k].what == "reasoning") {
      const auto f = detail::split(e[k].about, '\x1e');
      if (f.size() != 2 || f[0] != e[k].who) return Verdict::no(claim, at + " records reasoning outside an agreement with more than its party and its turn", k);
      reasoning++;
      continue;
    }
    if (e[k].what != "epistemic") continue;
    const auto f = detail::split(e[k].about, '\x1e');
    if (f.size() != 9 || f[0] != e[k].who) return Verdict::no(claim, at + " is not an epistemic entry as the journal writes one", k);
    long long turn = 0, size = 0;
    try {
      turn = std::stoll(f[5]), size = std::stoll(f[8]);
    } catch (const std::exception&) {
      return Verdict::no(claim, at + " has no turn or size", k);
    }
    const auto g = s.agreements.find(f[6]);
    if (g == s.agreements.end()) return Verdict::no(claim, at + " names " + f[6] + ", which is no agreement of the chronicle", k);
    if (!in_force(g->second, turn)) return Verdict::no(claim, at + " was recorded at turn " + std::to_string(turn) + ", when the agreement " + f[6] + " was not in force", k);
    if (!s.parties.has(f[0]) || !disclosure::covers(g->second.terms, f[0], origin_of(s, f[0]), f[2], f[3], f[4], turn))
      return Verdict::no(claim, at + " is not covered by the agreement it names", k);
    if (!holdings::is_digest(f[7])) return Verdict::no(claim, at + " names no digest of what was disclosed", k);
    used[f[6]] += size;
    if (g->second.terms.ceiling >= 0 && used[f[6]] > g->second.terms.ceiling) return Verdict::no(claim, at + " passes the ceiling of the agreement " + f[6], k);
    epistemic++;
  }
  return Verdict::yes(claim, std::to_string(epistemic) + " disclosed under agreements, " + std::to_string(reasoning) + " recorded only as reasoning", e.size());
}

// ---- Descriptions at levels (decision 0010) -----------------------------------------------------------------------------------------

// The chronicle and the behaviour journal described at levels: each act at N, a constituted act of its kind in this community's order;
// each episode of conduct at O with its trace at M (manifestation), joined to the act it constituted (codified counts-as); each
// disclosure at E with its record at M (disclosure), joined to the episode it preceded (enactment) and described as the formation of
// intent; each episode of reasoning outside agreements at M alone, its content undisclosed. An occurrence is named by the episode of
// conduct, so that one occurrence is described at E, O, M and N at once.
inline behaviour::Descriptions describe(const State& s, const Conduct& c) {
  behaviour::Descriptions out;
  const behaviour::Part& t = s.rules.behaviour;
  const auto& e = c.journal.entries;
  std::map<std::string, std::string> act_of_seal;  // an act's seal -> its description
  std::map<std::string, std::string> occurrence_of_act;
  for (std::size_t k = 0; k < e.size(); k++)
    if (e[k].what == "outcome") {
      const auto f = detail::split(e[k].about, '\x1e');
      if (f.size() == 2 && f[0].rfind("episode:", 0) == 0 && f[1].rfind("seal:", 0) == 0) occurrence_of_act[f[1].substr(5)] = "occurrence " + f[0].substr(8, 12);
    }
  for (std::size_t k = 0; k < s.acts.size(); k++) {
    const std::string seal = s.journal.entries[k].seal, id = "act " + std::to_string(k + 1);
    const auto occ = occurrence_of_act.find(seal);
    out.items.push_back({id, occ == occurrence_of_act.end() ? id : occ->second, "N", s.acts[k].kind, s.community, "", "", seal, {}});
    act_of_seal[seal] = id;
  }
  const std::string formation = t.facet("stage") && !t.facet("stage")->values.empty() ? t.facet("stage")->values.front() : std::string();
  for (std::size_t k = 0; k < e.size(); k++) {
    const std::string n = std::to_string(k + 1);
    if (e[k].what == "episode") {
      const std::string occ = "occurrence " + e[k].seal.substr(0, 12);
      out.items.push_back({"conduct " + n, occ, "O", "", "", "", "", e[k].seal, {}});
      out.items.push_back({"trace " + n, occ, "M", "", "", "A", "", e[k].seal, {}});
      out.links.push_back({"manifestation", "conduct " + n, "trace " + n});
      if (k + 1 < e.size() && e[k + 1].what == "outcome") {
        const auto f = detail::split(e[k + 1].about, '\x1e');
        if (f.size() == 2 && f[1].rfind("seal:", 0) == 0)
          if (const auto a = act_of_seal.find(f[1].substr(5)); a != act_of_seal.end()) out.links.push_back({"codified counts-as", "conduct " + n, a->second});
      }
    } else if (e[k].what == "epistemic" || e[k].what == "reasoning") {
      const auto next = preceded(c, k);
      const std::string occ = next ? "occurrence " + e[*next].seal.substr(0, 12) : "occurrence " + e[k].seal.substr(0, 12);
      out.items.push_back({"record " + n, occ, "M", "", "", "C", "", e[k].seal, {}});  // a self-report, or the bare fact of reasoning
      if (e[k].what == "epistemic") {
        std::map<std::string, std::string> facets;
        if (!formation.empty()) facets["stage"] = formation;
        out.items.push_back({"intent " + n, occ, "E", "", "", "", "", e[k].seal, facets});
        out.links.push_back({"disclosure", "intent " + n, "record " + n});
        if (next) out.links.push_back({"enactment", "intent " + n, "conduct " + std::to_string(*next + 1)});
      }
    }
  }
  return out;
}

}  // namespace thesmos::chronicle
