// Holdings: what a community holds, and the norms by which its parties may reach it.
//
// A community holds resources of many kinds: data, documents, tools, budgets, capacity, knowledge, briefings, the time of agents,
// references to accounts kept elsewhere (an open vocabulary). A holding has an identifier, a kind, a custodian (an office of the
// constitution, or a party), a provenance (a digest by archeion's rule, a source, a licence) and, when it is metered, a unit and a
// quantity; it may be kept within another holding (a shelf, a bench, a pool), and what is deposited within a holding that declares
// review stands checked only when a party of another origin attests it. Its visibility (public, members, or named offices) says who
// may know of it and read its description without an act; reading its content is an act under the norms.
//
// Access is governed by norms in the grammar of institutions (Crawford and Ostrom, A Grammar of Institutions, American Political
// Science Review 89, 1995; Frantz and Siddiki, Institutional Grammar 2.0, Public Administration 99, 2021): an attribute (anyone; a
// kind of party; anyone who holds a position; a party whose origin holds a position; a position; a role towards the holding, its
// custodian or its depositor), a deontic (may, must, must not), an aim (consult, use, deposit, modify, lend, withdraw), an object
// (any holding, a kind, a holding with what is kept within it), conditions (under a mandate of given kinds and matters, within a
// quota a round, by need under a declared standard, for a declared purpose, sent or not to a service outside the community) and an
// or-else. A permission silent on transmission permits none; a prohibition silent on it forbids whatever is sent.
//
// The resolution is declared and decided (the theory note, Definitions 10 to 13 and Proposition 8). Among the norms that match a
// request (its attribute holds of the party, its aim is the request's, its object covers the holding), the most specific decide, by
// a declared order: first the object (a holding before a kind before any holding, and among holdings the nearer before the farther,
// the holding itself before what it is kept within, as archeion's access rules order units), then the attribute (a role; a
// position the author holds itself; a position held by the party that answers for the author's act, its mandate's root
// principal; an origin; anyone who holds a position; a kind of party; anyone). At that level a prohibition whose conditions hold
// prevails over any permission; otherwise a permission (may or must) whose conditions hold permits; otherwise the request is
// refused, the conditions it failed named. Where no norm speaks the request is refused, unless the constitution declares its kind
// open to that aim. The verdict depends on the set of norms alone; their declared order only chooses which norm is cited.

#pragma once

#include <thesmos/kernel/kernel.hpp>

#include <archeion/fixity/sha256.hpp>
#include <symploke/jsonld/json.hpp>

#include <algorithm>
#include <compare>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::holdings {

using Json = symploke::jsonld::Json;

// ---- Aims, holdings ---------------------------------------------------------------------------------------------------------------

enum class Aim { consult, use, deposit, modify, lend, withdraw };

inline const std::vector<Aim>& aims() {
  static const std::vector<Aim> v = {Aim::consult, Aim::use, Aim::deposit, Aim::modify, Aim::lend, Aim::withdraw};
  return v;
}
inline std::string_view word(Aim a) {
  switch (a) {
    case Aim::consult: return "consult";
    case Aim::use: return "use";
    case Aim::deposit: return "deposit";
    case Aim::modify: return "modify";
    case Aim::lend: return "lend";
    case Aim::withdraw: return "withdraw";
  }
  return "?";
}
inline std::optional<Aim> aim_named(std::string_view w) {
  for (Aim a : aims())
    if (word(a) == w) return a;
  return std::nullopt;
}
// Whether an aim takes from a metered holding what is available: use and withdrawal consume it, a loan holds it until its term.
inline bool draws(Aim a) { return a == Aim::use || a == Aim::withdraw || a == Aim::lend; }
// The aims by which a party reads what is held (decision 0013): an operative's reading of these reaches its provider where the
// constitution declares that operatives transmit to their provider.
inline bool reads(Aim a) { return a == Aim::consult || a == Aim::use || a == Aim::lend || a == Aim::withdraw; }

// ---- Visibility ---------------------------------------------------------------------------------------------------------------------

// Who may know of a holding, or of a part of a constitution, and read its description without an act: anyone (public), anyone who
// holds a position of the community (members), or the holders of named offices. Reading a holding's content is an act, decided by
// the norms of access; visibility decides only what is shown (in a profile, in a briefing).
struct Visibility {
  enum class Level { everyone, members, offices } level = Level::members;
  std::set<std::string> offices;

  static Visibility everyone() { return {Level::everyone, {}}; }
  static Visibility members() { return {Level::members, {}}; }
};
inline std::string text(const Visibility& v) {
  if (v.level == Visibility::Level::everyone) return "public";
  if (v.level == Visibility::Level::members) return "members";
  std::string t = "offices:";
  for (const auto& o : v.offices) t += " " + o;
  return t;
}
// Whether a party holding these positions (its own and those of the party that answers for it) may see what has this visibility.
inline bool sees(const Visibility& v, const std::set<std::string>& positions) {
  if (v.level == Visibility::Level::everyone) return true;
  if (v.level == Visibility::Level::members) return !positions.empty();
  for (const auto& o : v.offices)
    if (positions.count(o)) return true;
  return false;
}

struct Provenance {
  std::string digest;   // SHA-256 of what is held, 64 lower-case hexadecimal digits (archeion's rule), or empty for a container or a pool
  std::string source, licence;
};

struct Holding {
  std::string id, kind;
  std::string unit;                     // metered when not empty
  long long quantity = 0;               // in its unit, when metered
  std::string custodian;                // an office (a position of the constitution) or, with office false, a party
  bool office = true;
  std::string within;                   // the holding it is kept within, or empty
  std::string review;                   // "another origin" or "another provider" (decision 0012): what is deposited within it stands checked only
                                        // when a party of another origin, or another provider, than its depositor's attests it
  std::vector<std::string> verdicts, checks_on;  // the verdicts an attestation may give, and those that check (decision 0012)
  Provenance provenance;
  std::string depositor, deposited_by;  // the party that answers for its deposit and the deposit's author; empty when declared at the founding
  long long since = 0;                  // the turn at which it was declared or deposited
  bool pending = false;                 // deposited within a holding that declares review, and not yet checked
  std::string checked_by;
  bool withdrawn = false;               // an unmetered holding withdrawn: a marker stays in the record
  std::vector<std::string> versions;    // the digests it had before each modification, oldest first
  Visibility visibility;                // who may know of it; members' unless declared (a deposit takes its container's)

  bool metered() const { return !unit.empty(); }
};

inline const std::set<std::string>& reviews() {
  static const std::set<std::string> s = {"another origin", "another provider"};
  return s;
}

// The digest of bytes by archeion's rule, and whether a text is one.
inline std::string digest(std::string_view bytes) { return archeion::fixity::sha256_hex(bytes); }
inline bool is_digest(std::string_view s) {
  return s.size() == 64 && std::all_of(s.begin(), s.end(), [](char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'); });
}

// ---- Norms ------------------------------------------------------------------------------------------------------------------------

enum class Deontic { may, must, must_not };
inline std::string_view word(Deontic d) { return d == Deontic::may ? "may" : d == Deontic::must ? "must" : "must not"; }

// The attribute: whom the norm binds. `member` is anyone who holds some position of the community.
// `party` binds one party named, and those that act for it: the operatives whose origin it is, and any act it answers for (decision
// 0010, for the norms an agreement makes).
enum class Who { anyone, kind, member, origin, position, role, party };
struct Attribute {
  Who who = Who::anyone;
  std::string value;  // the kind of party, the position (of the party, or of its origin), or the role (custodian, depositor)
};
// The declared order of specificity among attributes, least specific first: a position counts for more when the author holds it
// itself than when it reaches the author only through the party that answers for the act.
inline int rank(Who w, bool own = true) {
  switch (w) {
    case Who::anyone: return 0;
    case Who::kind: return 1;
    case Who::member: return 2;
    case Who::origin: return 3;
    case Who::position: return own ? 5 : 4;
    case Who::role: return 6;
    case Who::party: return 7;
  }
  return 0;
}
inline const std::set<std::string>& roles() {
  static const std::set<std::string> s = {"custodian", "depositor"};
  return s;
}

enum class Over { any, kind, holding };
struct Object {
  Over over = Over::any;
  std::string value;  // the kind, or the holding (with what is kept within it)
};

struct Conditions {
  bool under_mandate = false;                    // made under a mandate covering these kinds of act and matters
  std::set<std::string> mandate_kinds, mandate_matters;
  long long quota = -1;                          // the most the party that answers may draw under the norm in one round; -1: none
  std::string need;                              // a standard under which the record attests the party's need; empty: none
  std::set<std::string> purposes;                // the purposes declared; empty: any
  // Transmission: whether what is reached may be sent to a service outside the community. A permission that declares nothing permits
  // no transmission; one that declares services permits sending to them ("any": to any). A prohibition that declares nothing forbids
  // whatever is sent; one that declares services forbids only sending to them ("any": sending anywhere).
  bool transmit_declared = false, transmit_any = false;
  std::set<std::string> transmit_to;

  bool none() const { return !under_mandate && quota < 0 && need.empty() && purposes.empty() && !transmit_declared; }
};

struct Norm {
  std::string id;
  Attribute attribute;
  Deontic deontic = Deontic::may;
  Aim aim = Aim::consult;
  Object object;
  Conditions conditions;
  std::string or_else;

  bool permits() const { return deontic != Deontic::must_not; }
};

// What a constitution declares of its holdings: those it holds at its founding, the norms of access in declared order, the kinds
// declared open to some aims where no norm speaks, and the kinds it expects to be deposited.
struct Register {
  std::map<std::string, Holding> holdings;
  std::vector<Norm> norms;
  std::map<std::string, std::set<Aim>> open;
  std::set<std::string> kinds;
  std::vector<std::string> unread;  // parts of the written form that could not be read, reported by admission
  // Whether an operative's reading counts as a transmission to its provider (decision 0013): the base taxonomy's counts-as rule "a
  // reading by an operative reaches its provider", applied where the part access declares "operatives transmit to their provider".
  bool operatives_transmit = false;

  bool empty() const { return holdings.empty() && norms.empty() && open.empty() && kinds.empty() && unread.empty() && !operatives_transmit; }
};

// ---- Requests --------------------------------------------------------------------------------------------------------------------

// The holding a request bears on, as the norms see it. A fresh target is the holding a deposit would make.
struct Target {
  std::string id, kind, custodian, depositor;
  bool office = true;
  std::vector<std::string> within;  // the holdings it is kept within, nearest first
  bool fresh = false;
  bool metered = false;
  std::string unit;
  std::vector<Aim> aims;            // the aims a briefing considers on it (decide ignores them)
  Visibility visibility;            // who may know of it (decide ignores it)
};

// What the record says of the party that makes a request.
struct Situation {
  std::string party;                           // the author
  std::string bearer;                          // the party that answers for the act: its mandate's root principal, or its author
  kernel::Kind kind = kernel::Kind::person;
  std::string origin;                          // the author's origin under control
  std::set<std::string> positions;             // held by the author itself
  std::set<std::string> bearer_positions;      // held by the party that answers for the act, when it is another
  std::set<std::string> origin_positions;      // held by the author's origin

  bool holds(const std::string& position) const { return positions.count(position) > 0 || bearer_positions.count(position) > 0; }
  bool mandated = false;                       // whether it acts under a mandate, and that mandate's kinds and matters
  std::set<std::string> mandate_kinds, mandate_matters;
  std::set<std::string> needs;                 // the standards under which the record attests its need
  std::map<std::string, long long> used;       // norm -> what the party that answers drew under it in this round
};

struct Request {
  Aim aim = Aim::consult;
  long long amount = 0;  // in the holding's unit, when metered
  std::string purpose;
  std::string transmit;  // the service outside the community to which what is reached is sent, if the act declares one
  std::string reaches;   // the provider an operative's reading reaches, as the counts-as rule of decision 0013 makes it, if any
};

// What a request counts against a quota: its amount when it moves a metered quantity, else one act.
inline long long counted(const Target& t, const Request& q) {
  return t.metered && (q.aim == Aim::deposit || draws(q.aim)) ? q.amount : 1;
}
// The amount a briefing asks with: the least that can be moved.
inline long long least(const Target& t, Aim a) { return t.metered && (a == Aim::deposit || draws(a)) ? 1 : 0; }

// ---- Matching and specificity ----------------------------------------------------------------------------------------------------

struct Specificity {
  int over = 0;             // a holding 2, a kind 1, any holding 0
  long long nearness = 0;   // among holdings: minus the steps from the target to the holding named (0: the target itself)
  int who = 0;              // rank of the attribute
  auto operator<=>(const Specificity&) const = default;
};

inline bool holds(const Attribute& a, const Target& t, const Situation& s) {
  switch (a.who) {
    case Who::anyone: return true;
    case Who::kind: return kernel::word(s.kind) == a.value;
    case Who::member: return !s.positions.empty() || !s.bearer_positions.empty();
    case Who::origin: return s.origin_positions.count(a.value) > 0;
    case Who::position: return s.holds(a.value);
    case Who::role:
      if (a.value == "custodian")
        return !t.custodian.empty() && (t.office ? s.holds(t.custodian) : (t.custodian == s.party || t.custodian == s.bearer));
      if (a.value == "depositor") return !t.depositor.empty() && (t.depositor == s.party || t.depositor == s.bearer);
      return false;
    case Who::party: return !a.value.empty() && (a.value == s.party || a.value == s.bearer || a.value == s.origin);
  }
  return false;
}

// The specificity with which a norm speaks to a request on a target, or nothing when it does not match.
inline std::optional<Specificity> matches(const Norm& n, const Target& t, Aim aim, const Situation& s) {
  if (n.aim != aim || !holds(n.attribute, t, s)) return std::nullopt;
  Specificity sp;
  sp.who = rank(n.attribute.who, n.attribute.who != Who::position || s.positions.count(n.attribute.value) > 0);
  if (n.object.over == Over::kind) {
    if (n.object.value != t.kind) return std::nullopt;
    sp.over = 1;
  } else if (n.object.over == Over::holding) {
    long long d = -1;
    if (!t.fresh && n.object.value == t.id) d = 0;
    for (std::size_t i = 0; d < 0 && i < t.within.size(); i++)
      if (t.within[i] == n.object.value) d = static_cast<long long>(i) + 1;
    if (d < 0) return std::nullopt;
    sp.over = 2;
    sp.nearness = -d;
  }
  return sp;
}

// The conditions of a norm that a request fails, each said in words.
inline std::vector<std::string> unmet(const Norm& n, const Target& t, const Request& q, const Situation& s) {
  std::vector<std::string> out;
  const Conditions& c = n.conditions;
  if (c.under_mandate) {
    if (!s.mandated) out.push_back("it is made under no mandate");
    else {
      for (const auto& k : c.mandate_kinds)
        if (!s.mandate_kinds.count(k)) out.push_back("its mandate does not cover " + k);
      for (const auto& m : c.mandate_matters)
        if (!s.mandate_matters.count(m)) out.push_back("its mandate does not cover the matter " + m);
    }
  }
  if (c.quota >= 0) {
    const auto u = s.used.find(n.id);
    const long long used = u == s.used.end() ? 0 : u->second;
    if (used + counted(t, q) > c.quota)
      out.push_back("its quota is " + std::to_string(c.quota) + (t.metered ? " " + t.unit : "") + " a round, of which " + std::to_string(used) + " is drawn");
  }
  if (!c.need.empty() && !s.needs.count(c.need)) out.push_back("the record attests no need under \"" + c.need + "\"");
  if (!c.purposes.empty() && !c.purposes.count(q.purpose))
    out.push_back(q.purpose.empty() ? "it declares no purpose" : "its purpose, " + q.purpose + ", is none of those declared");
  // The request's transmissions: the service the act declares, and the provider an operative's reading reaches (decision 0013). A
  // permission must cover each; a prohibition that names transmissions speaks only to a request sent to one of those it names.
  auto covered = [&](const std::string& x) { return c.transmit_any || c.transmit_to.count(x) > 0; };
  if (n.permits()) {
    if (!q.transmit.empty() && !covered(q.transmit))
      out.push_back(c.transmit_declared ? "it would be sent to " + q.transmit + ", outside those the norm permits" : "it would be sent outside the community, to " + q.transmit + ", which the norm does not permit");
    if (!q.reaches.empty() && q.reaches != q.transmit && !covered(q.reaches))
      out.push_back(c.transmit_declared ? "read by an operative, it would reach its provider, " + q.reaches + ", outside those the norm permits"
                                        : "read by an operative, it would reach its provider, " + q.reaches + ", outside the community, which the norm does not permit");
  } else if (c.transmit_declared && !((!q.transmit.empty() && covered(q.transmit)) || (!q.reaches.empty() && covered(q.reaches)))) {
    if (q.transmit.empty() && q.reaches.empty()) out.push_back("it is sent nowhere outside");
    else out.push_back("it is sent to " + (q.transmit.empty() ? q.reaches : q.transmit) + ", which the prohibition does not name");
  }
  return out;
}

// ---- Words ------------------------------------------------------------------------------------------------------------------------

inline std::string text(const Attribute& a) {
  switch (a.who) {
    case Who::anyone: return "anyone";
    case Who::kind: return "any " + a.value;
    case Who::member: return "anyone who holds a position";
    case Who::origin: return "a party whose origin holds " + a.value;
    case Who::position: return a.value;
    case Who::role: return "the " + a.value;
    case Who::party: return a.value;
  }
  return "?";
}
inline std::string text(const Object& o, Aim aim) {
  const std::string prep = aim == Aim::deposit ? "to " : aim == Aim::withdraw ? "from " : "";
  if (o.over == Over::holding) return prep + "the " + o.value;
  if (o.over == Over::kind) return prep + "a holding of kind " + o.value;
  return prep + "any holding";
}
inline std::string joined(const std::set<std::string>& items, const std::string& last) {
  std::string out;
  std::size_t k = 0;
  for (const auto& i : items) {
    out += (k == 0 ? "" : k + 1 == items.size() ? " " + last + " " : ", ") + i;
    k++;
  }
  return out;
}
inline std::string text(const Conditions& c) {
  std::string t;
  if (c.under_mandate) {
    t += " under a mandate";
    if (!c.mandate_kinds.empty()) t += " of " + joined(c.mandate_kinds, "and");
    if (!c.mandate_matters.empty()) t += " on " + joined(c.mandate_matters, "and");
  }
  if (c.quota >= 0) t += " within " + std::to_string(c.quota) + " a round";
  if (!c.need.empty()) t += " by need under " + c.need;
  if (!c.purposes.empty()) t += " for " + joined(c.purposes, "or");
  if (c.transmit_any) t += ", sent anywhere outside";
  else if (!c.transmit_to.empty()) t += ", sent to " + joined(c.transmit_to, "or");
  return t;
}
// A norm in words: "contributor may deposit to the shelf.", "member may use the bench for analysis or replication."
inline std::string text(const Norm& n) {
  std::string t = text(n.attribute) + " " + std::string(word(n.deontic)) + " " + std::string(word(n.aim)) + " " + text(n.object, n.aim) + text(n.conditions);
  if (!n.or_else.empty()) t += " or else " + n.or_else;
  return t + ".";
}

// ---- Decision ---------------------------------------------------------------------------------------------------------------------

struct Decision {
  bool permitted = false;
  bool spoken = false;              // some norm matched the request
  bool open = false;                // no norm spoke, and the kind is open to the aim
  std::string norm;                 // the norm cited, if any
  std::size_t index = std::numeric_limits<std::size_t>::max();
  std::vector<std::string> unmet;   // the conditions failed at the deciding level, when refused there
  std::optional<Specificity> level;
  std::string reason;
};

namespace detail {
inline std::string party_of(const Situation& s) { return s.party.empty() ? std::string("the party") : s.party; }
inline std::string what(const Target& t, const Request& q) {
  return std::string(word(q.aim)) + (t.fresh ? " a new " + t.kind + (t.within.empty() ? std::string() : " within the " + t.within.front()) : " the " + t.id);
}
}  // namespace detail

// The resolution on the norms that decide (those of greatest specificity among the matching, in declared order).
inline Decision resolve(const Register& r, const std::vector<std::size_t>& top, Specificity level, const Target& t, const Request& q, const Situation& s) {
  Decision d;
  d.spoken = true;
  d.level = level;
  for (std::size_t i : top) {
    const Norm& n = r.norms[i];
    if (!n.permits() && unmet(n, t, q, s).empty()) {
      d.norm = n.id, d.index = i;
      d.reason = "the norm \"" + n.id + "\" forbids it: " + text(n);
      return d;
    }
  }
  for (std::size_t i : top) {
    const Norm& n = r.norms[i];
    if (n.permits() && unmet(n, t, q, s).empty()) {
      d.permitted = true;
      d.norm = n.id, d.index = i;
      d.reason = "permitted by the norm \"" + n.id + "\": " + text(n);
      return d;
    }
  }
  const std::size_t cited = [&] {
    for (std::size_t i : top)
      if (r.norms[i].permits()) return i;
    return top.front();
  }();
  const Norm& n = r.norms[cited];
  d.norm = n.id, d.index = cited;
  if (n.permits()) {
    d.unmet = unmet(n, t, q, s);
    std::string why;
    for (const auto& u : d.unmet) why += (why.empty() ? "" : "; ") + u;
    d.reason = "the norm \"" + n.id + "\" permits it only" + text(n.conditions) + ": " + why;
  } else {
    d.reason = "the most specific norms that speak to it permit nothing here; \"" + n.id + "\" forbids it" + text(n.conditions);
  }
  return d;
}

// Decides a request on a target under the declared resolution.
inline Decision decide(const Register& r, const Target& t, const Request& q, const Situation& s) {
  std::optional<Specificity> best;
  std::vector<std::size_t> top;
  for (std::size_t i = 0; i < r.norms.size(); i++) {
    const auto sp = matches(r.norms[i], t, q.aim, s);
    if (!sp) continue;
    if (!best || *sp > *best) best = sp, top = {i};
    else if (*sp == *best) top.push_back(i);
  }
  if (top.empty()) {
    Decision d;
    const auto o = r.open.find(t.kind);
    if (o != r.open.end() && o->second.count(q.aim)) {
      d.permitted = d.open = true;
      d.reason = "no norm speaks to it, and holdings of kind " + t.kind + " are open to " + std::string(word(q.aim));
    } else {
      d.reason = "no norm permits " + detail::party_of(s) + " to " + detail::what(t, q) + ", and no open access is declared for its kind";
    }
    return d;
  }
  return resolve(r, top, *best, t, q, s);
}

// ---- The briefing's entries -------------------------------------------------------------------------------------------------------

struct Entry {
  std::size_t target = 0;           // the index of the target
  Aim aim = Aim::consult;
  Decision decision;
  long long remaining = -1;         // under a quota, what the party may still draw this round; -1: no quota
  std::vector<std::string> norms;   // the norms that decide, in words
  bool covered = true;              // whether the mandate the party acts under covers the act (set by whoever knows the mandate)
  std::string reaches;              // the provider the party's reading would reach (decision 0013), if any
};

// For a party in a situation, every (target, aim) on which some norm speaks or which open access opens, each with its decision:
// computed by one sweep over the norms that keeps, for each pair, the norms of greatest specificity met so far (Proposition 10).
// `reaches` is the provider the party's readings reach (decision 0013), decided with each reading.
inline std::vector<Entry> brief(const Register& r, const std::vector<Target>& targets, const Situation& s, const std::string& purpose = {},
                                const std::string& reaches = {}) {
  struct Bucket {
    std::optional<Specificity> best;
    std::vector<std::size_t> top;
  };
  std::vector<std::map<Aim, Bucket>> buckets(targets.size());
  for (std::size_t i = 0; i < r.norms.size(); i++) {
    const Norm& n = r.norms[i];
    for (std::size_t k = 0; k < targets.size(); k++) {
      const Target& t = targets[k];
      if (std::find(t.aims.begin(), t.aims.end(), n.aim) == t.aims.end()) continue;
      const auto sp = matches(n, t, n.aim, s);
      if (!sp) continue;
      Bucket& b = buckets[k][n.aim];
      if (!b.best || *sp > *b.best) b.best = sp, b.top = {i};
      else if (*sp == *b.best) b.top.push_back(i);
    }
  }
  std::vector<Entry> out;
  for (std::size_t k = 0; k < targets.size(); k++) {
    const Target& t = targets[k];
    for (Aim a : t.aims) {
      const Request q{a, least(t, a), purpose, {}, reads(a) ? reaches : std::string()};
      Entry e;
      e.target = k, e.aim = a, e.reaches = q.reaches;
      const auto b = buckets[k].find(a);
      if (b == buckets[k].end()) {
        const auto o = r.open.find(t.kind);
        if (o == r.open.end() || !o->second.count(a)) continue;
        e.decision = decide(r, t, q, s);  // open access: no norm speaks, so decide's answer is the open one
      } else {
        e.decision = resolve(r, b->second.top, *b->second.best, t, q, s);
        for (std::size_t i : b->second.top) e.norms.push_back(text(r.norms[i]));
      }
      if (e.decision.permitted && e.decision.index < r.norms.size()) {
        const Norm& n = r.norms[e.decision.index];
        if (n.conditions.quota >= 0) {
          const auto u = s.used.find(n.id);
          e.remaining = n.conditions.quota - (u == s.used.end() ? 0 : u->second);
        }
      }
      out.push_back(std::move(e));
    }
  }
  return out;
}

// The norms that bind a party to act (deontic must) whose attribute holds of it on some target, or, when no target is given, whose
// attribute does not depend on a holding.
inline std::vector<std::size_t> duties(const Register& r, const std::vector<Target>& targets, const Situation& s) {
  std::vector<std::size_t> out;
  for (std::size_t i = 0; i < r.norms.size(); i++) {
    const Norm& n = r.norms[i];
    if (n.deontic != Deontic::must) continue;
    bool bound = false;
    if (n.attribute.who != Who::role) bound = holds(n.attribute, Target{}, s);
    for (const Target& t : targets) bound = bound || holds(n.attribute, t, s);
    if (bound) out.push_back(i);
  }
  return out;
}

// ---- The written form -------------------------------------------------------------------------------------------------------------

namespace detail {
inline std::string text_of(const Json* j) { return j && j->is_string() ? j->text : std::string(); }
inline std::set<std::string> strings_of(const Json* j) {
  std::set<std::string> v;
  if (!j) return v;
  if (j->is_string()) v.insert(j->text);
  else if (j->is_array())
    for (const Json& i : j->items)
      if (i.is_string()) v.insert(i.text);
  return v;
}
}  // namespace detail

// Reads a visibility: "public", "members", {"office": "<position>"} or {"office": ["<position>", …]}. Anything else is noted in
// `unread` and read as members'.
inline Visibility read_visibility(const Json* j, std::vector<std::string>& unread, const std::string& label, Visibility otherwise = Visibility::members()) {
  if (!j) return otherwise;
  if (j->is_string() && j->text == "public") return Visibility::everyone();
  if (j->is_string() && j->text == "members") return Visibility::members();
  if (j->is_object() && j->has("office")) {
    Visibility v{Visibility::Level::offices, detail::strings_of(j->get("office"))};
    if (!v.offices.empty()) return v;
  }
  unread.push_back(label + " declares a visibility other than public, members or named offices");
  return Visibility::members();
}

// Reads the parts "holdings" and "access" of a constitution's written form (noetarium.constitution/1):
//
//   "holdings": {"shelf": {"kind": "data", "custodian": "steward", "review": "another origin", "visibility": "public",
//                          "provenance": {"digest": "…", "source": "…", "licence": "CC BY 4.0"}},
//                "writing pool": {"kind": "budget", "unit": "words", "quantity": 0, "custodian": "board"},
//                "ledger": {"kind": "reference", "custodian": {"party": "a bank"}, "within": "…"}},
//   "access": {"operatives transmit to their provider": true, "open": {"knowledge": ["consult"]}, "kinds": ["tool"],
//              "norms": [{"id": "…", "who": "contributor" | "anyone" | "any position" | {"kind" | "origin" | "position" | "role": "…"},
//                         "may" | "must" | "must not": "<aim>", "object": "<holding>" | {"holding": "…"} | {"kind": "…"},
//                         "under mandate": {"kinds": […], "matters": […]}, "quota": n, "need": "<standard>", "for": […],
//                         "transmit": "any" | ["<service>", …], "or else": "…"}]}
//
// A holding's visibility is "public", "members" (the default) or {"office": …}; a deposit takes its container's.
//
// A part of unexpected shape is noted in `unread`, which admission reports.
inline Register read(const Json* holdings_part, const Json* access_part) {
  using detail::strings_of;
  using detail::text_of;
  Register r;
  if (holdings_part && holdings_part->is_object())
    for (const auto& [id, v] : holdings_part->members) {
      if (!v.is_object()) {
        r.unread.push_back("the holding " + id + " is not described");
        continue;
      }
      Holding h;
      h.id = id;
      h.kind = text_of(v.get("kind"));
      h.unit = text_of(v.get("unit"));
      if (const Json* q = v.get("quantity"); q && q->is_number()) h.quantity = static_cast<long long>(q->number);
      if (const Json* c = v.get("custodian")) {
        if (c->is_string()) h.custodian = c->text, h.office = true;
        else if (c->is_object()) h.custodian = text_of(c->get("party")), h.office = false;
      }
      h.within = text_of(v.get("within"));
      h.review = text_of(v.get("review"));
      for (const auto* key : {"verdicts", "checks on"}) {
        const auto got = strings_of(v.get(key));
        (std::string(key) == "verdicts" ? h.verdicts : h.checks_on).assign(got.begin(), got.end());
      }  // decision 0012
      h.visibility = read_visibility(v.get("visibility"), r.unread, "the holding " + id);
      if (const Json* p = v.get("provenance"); p && p->is_object())
        h.provenance = Provenance{text_of(p->get("digest")), text_of(p->get("source")), text_of(p->get("licence"))};
      r.holdings[id] = h;
    }
  if (access_part && access_part->is_object()) {
    if (const Json* o = access_part->get("open"); o && o->is_object())
      for (const auto& [kind, list] : o->members)
        for (const auto& w : strings_of(&list)) {
          if (const auto a = aim_named(w)) r.open[kind].insert(*a);
          else r.unread.push_back("open access to " + kind + " names " + w + ", which is no aim");
        }
    r.kinds = strings_of(access_part->get("kinds"));
    if (const Json* t = access_part->get("operatives transmit to their provider")) {  // decision 0013
      if (t->is_bool()) r.operatives_transmit = t->truth;
      else r.unread.push_back("\"operatives transmit to their provider\" is declared true or false");
    }
    if (const Json* ns = access_part->get("norms"); ns && ns->is_array())
      for (const Json& x : ns->items) {
        Norm n;
        n.id = text_of(x.get("id"));
        const std::string label = n.id.empty() ? std::string("a norm of access") : "the norm \"" + n.id + "\"";
        if (const Json* w = x.get("who")) {
          if (w->is_string()) {
            if (w->text == "anyone") n.attribute = {Who::anyone, {}};
            else if (w->text == "any position") n.attribute = {Who::member, {}};
            else n.attribute = {Who::position, w->text};
          } else if (w->is_object() && w->members.size() == 1) {
            const auto& [key, val] = w->members.front();
            const std::string value = val.is_string() ? val.text : std::string();
            if (key == "kind") n.attribute = {Who::kind, value};
            else if (key == "origin") n.attribute = {Who::origin, value};
            else if (key == "position") n.attribute = {Who::position, value};
            else if (key == "role") n.attribute = {Who::role, value};
            else if (key == "party") n.attribute = {Who::party, value};
            else r.unread.push_back(label + " names whom it binds by " + key + ", which is no attribute");
          } else
            r.unread.push_back(label + " does not say whom it binds");
        }
        int deontics = 0;
        for (const auto& [key, d] : std::vector<std::pair<const char*, Deontic>>{{"may", Deontic::may}, {"must", Deontic::must}, {"must not", Deontic::must_not}})
          if (const Json* a = x.get(key)) {
            deontics++;
            n.deontic = d;
            if (const auto aim = aim_named(text_of(a))) n.aim = *aim;
            else r.unread.push_back(label + " names " + (text_of(a).empty() ? std::string("no aim") : text_of(a)) + ", which is no aim of access");
          }
        if (deontics != 1) r.unread.push_back(label + " declares " + (deontics ? "more than one deontic" : "no deontic (may, must, must not)"));
        if (const Json* o = x.get("object")) {
          if (o->is_string()) n.object = {Over::holding, o->text};
          else if (o->is_object() && o->has("holding")) n.object = {Over::holding, text_of(o->get("holding"))};
          else if (o->is_object() && o->has("kind")) n.object = {Over::kind, text_of(o->get("kind"))};
          else r.unread.push_back(label + " names an object of no known shape");
        }
        if (const Json* m = x.get("under mandate"); m && m->is_object()) {
          n.conditions.under_mandate = true;
          n.conditions.mandate_kinds = strings_of(m->get("kinds"));
          n.conditions.mandate_matters = strings_of(m->get("matters"));
        }
        if (const Json* q = x.get("quota")) {
          if (q->is_number()) n.conditions.quota = static_cast<long long>(q->number);
          else r.unread.push_back(label + " declares a quota that is not a number");
        }
        n.conditions.need = text_of(x.get("need"));
        n.conditions.purposes = strings_of(x.get("for"));
        if (const Json* t = x.get("transmit")) {
          n.conditions.transmit_declared = true;
          if (t->is_string() && t->text == "any") n.conditions.transmit_any = true;
          else n.conditions.transmit_to = strings_of(t);
          if (!n.conditions.transmit_any && n.conditions.transmit_to.empty()) r.unread.push_back(label + " declares a transmission to no service");
        }
        n.or_else = text_of(x.get("or else"));
        r.norms.push_back(n);
      }
  }
  return r;
}

// ---- Admission by form ------------------------------------------------------------------------------------------------------------

struct Fault {
  int condition = 0;  // the constitution's condition of form it falls under: 1 (closure) or 6 (custody of means)
  std::string reason;
};

// The faults of form of what a constitution declares of its holdings, given its positions. Under closure (condition 1): every norm
// has an identifier of its own, one deontic and an aim, binds a declared position, kind of party or role, and names a declared
// holding or kind; open access names aims. Under the custody of means (condition 6): every holding has a kind and a custodian that
// is a declared office or a named party, a metered holding a quantity of no less than nothing, an unmetered one none, a container
// that is declared and no cycle of containers, a review the ground knows, and a provenance digest by archeion's rule where given.
inline std::vector<Fault> faults(const Register& r, const std::map<std::string, std::vector<std::string>>& positions) {
  std::vector<Fault> out;
  for (const auto& u : r.unread) out.push_back({1, u});
  std::set<std::string> kinds = r.kinds;
  for (const auto& [id, h] : r.holdings) kinds.insert(h.kind);
  for (const auto& [kind, a] : r.open) kinds.insert(kind);
  for (const auto& [id, h] : r.holdings) {
    const std::string label = "the holding " + id;
    if (h.kind.empty()) out.push_back({6, label + " declares no kind"});
    if (h.custodian.empty()) out.push_back({6, label + " has no custodian"});
    else if (h.office && !positions.count(h.custodian)) out.push_back({6, label + " is kept by " + h.custodian + ", which is no office of the constitution"});
    if (h.metered() && h.quantity < 0) out.push_back({6, label + " holds less than nothing"});
    if (!h.metered() && h.quantity != 0) out.push_back({6, label + " declares a quantity and no unit"});
    if (!h.review.empty() && !reviews().count(h.review)) out.push_back({6, label + " declares a review the ground does not know, " + h.review});
    for (const auto& c : h.checks_on)
      if (std::find(h.verdicts.begin(), h.verdicts.end(), c) == h.verdicts.end()) out.push_back({6, label + " checks on the verdict " + c + ", which is none of its verdicts"});
    if (!h.checks_on.empty() && h.review.empty()) out.push_back({6, label + " checks on verdicts and declares no review"});
    if (!h.provenance.digest.empty() && !is_digest(h.provenance.digest)) out.push_back({6, label + " names a digest that is not SHA-256 in 64 lower-case hexadecimal digits"});
    for (const auto& o : h.visibility.offices)
      if (!positions.count(o)) out.push_back({1, label + " is shown to the office " + o + ", which the constitution lacks"});
    if (!h.within.empty()) {
      std::set<std::string> seen{id};
      std::string cursor = h.within;
      while (!cursor.empty()) {
        const auto c = r.holdings.find(cursor);
        if (c == r.holdings.end()) {
          out.push_back({6, label + " is kept within " + cursor + ", which is not declared"});
          break;
        }
        if (!seen.insert(cursor).second) {
          out.push_back({6, label + " is kept within a cycle of holdings"});
          break;
        }
        cursor = c->second.within;
      }
    }
  }
  std::set<std::string> ids;
  for (const Norm& n : r.norms) {
    const std::string label = n.id.empty() ? std::string("a norm of access") : "the norm \"" + n.id + "\"";
    if (n.id.empty()) out.push_back({1, "a norm of access has no identifier"});
    else if (!ids.insert(n.id).second) out.push_back({1, label + " is declared twice"});
    const Attribute& a = n.attribute;
    if ((a.who == Who::position || a.who == Who::origin) && !positions.count(a.value)) out.push_back({1, label + " binds the position " + a.value + ", which the constitution lacks"});
    if (a.who == Who::kind && !kernel::kind_named(a.value)) out.push_back({1, label + " binds " + a.value + ", which is no kind of party"});
    if (a.who == Who::role && !roles().count(a.value)) out.push_back({1, label + " binds the role " + a.value + ", which is neither custodian nor depositor"});
    if (n.object.over == Over::holding && !r.holdings.count(n.object.value)) out.push_back({1, label + " names the holding " + n.object.value + ", which is not declared"});
    if (n.object.over == Over::kind && !kinds.count(n.object.value)) out.push_back({1, label + " names the kind " + n.object.value + ", which is not declared"});
    if (n.conditions.quota < -1) out.push_back({1, label + " declares a quota below nothing"});
  }
  return out;
}

}  // namespace thesmos::holdings
