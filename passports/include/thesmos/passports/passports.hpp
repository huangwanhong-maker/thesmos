// Passports (decision 0007): the document noetarium.passport/1 that a home issues for a mandate it has entered, and its verification
// from the document alone.
//
// A passport carries every link of a mandate's chain as the act that issued it, each with its seal and its proof of inclusion under the
// home's head; the notaries' witnesses of that head, each an attestation in the notary's own chronicle with its proof under the notary's
// head; and the holder's declarations (its kind and, for an operative, its controller, provider and model). Anyone who knows the home's
// head, or the head of a notary that witnessed it, verifies the passport: every link is an act of the home, the chain stands, its term
// holds at the instant of the check, it names the host where it is shown, and the credential shown is the one whose digest the mandate
// names (Proposition 11 of the theory note). A passport grants no entry; entry is the host's.
//
// The document stands beneath the chronicle so that a host's fold can verify a passport presented to it (decision 0009); a passport
// is issued from a home's state, and a head witnessed in a notary's chronicle, by `crossings`.

#pragma once

#include <thesmos/acts/acts.hpp>
#include <thesmos/holdings/holdings.hpp>

#include <archeion/fixity/journal.hpp>
#include <archeion/fixity/sha256.hpp>

#include <apodeixis/kernel/verdict.hpp>

#include <symploke/jsonld/json.hpp>

#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace thesmos::passports {

using archeion::fixity::Digest;
using archeion::fixity::Inclusion;

// A community's head at a moment: the number of entries of its chronicle and the root of their Merkle tree (archeion, after RFC 6962).

struct Head {
  std::string community;
  std::size_t size = 0;
  Digest root{};
};

inline bool same(const Head& a, const Head& b) { return a.community == b.community && a.size == b.size && a.root == b.root; }

// A notary's witness of a head: the attestation it entered in its own chronicle, naming the community, the number of entries and the
// root, with the attestation's proof of inclusion under the notary's head. A verifier rebuilds the attestation from the head it is
// shown, so a witness attests that head and no other.
struct Witness {
  std::string notary;         // the notary's community
  std::string attester;       // the party of the notary that attested, and the mandate it acted under, if any
  std::string mandate;
  long long turn = 0;
  std::string when;
  std::string seal;           // the attestation's seal in the notary's chronicle
  Inclusion proof;            // its leaf under the notary's head
  Head at;                    // the notary's head when it witnessed
};

// The act by which a notary attests a head.
inline acts::Act attestation(const Head& h, const std::string& attester, long long turn, const std::string& when = {}, const std::string& mandate = {}) {
  acts::Act a;
  a.author = attester;
  a.kind = "attest";
  a.turn = turn;
  a.when = when;
  a.mandate = mandate;
  a.fields = {{"head of", h.community}, {"root", archeion::fixity::hex(h.root)}, {"size", std::to_string(h.size)}};
  return a;
}

inline bool attests(const Witness& w, const Head& h) {
  if (w.at.community != w.notary || w.proof.size != w.at.size) return false;
  return archeion::fixity::included(w.seal + "\x1e" + acts::envelope(attestation(h, w.attester, w.turn, w.when, w.mandate)), w.proof, w.at.root);
}

// One link of a passport's chain: the act of kind "mandate" that issued it, as the home entered it (its body left out), its seal, and
// its leaf's proof of inclusion under the passport's head.
struct Link {
  acts::Act act;
  std::string seal;
  Inclusion proof;
};

struct Passport {
  std::string issued;                // when it was assembled, ISO 8601 in UTC
  std::string home;                  // the community that issued it
  std::string node, status_at;       // where the home is served and where the mandate's status is kept: pointers, outside every seal
  Head head;                         // the home's head under which every link is included
  std::vector<Witness> witnesses;    // notaries' attestations of that head
  kernel::Party principal;           // the party that answers: the root's principal, with its kind
  kernel::Party holder;              // the mandate's holder, with its declared kind and, for an operative, controller and provider
  std::string model, label;          // the holder's declared model, and a label for persons to read
  std::vector<Link> chain;           // root first; the last link is the holder's mandate

  const mandates::Mandate& mandate() const { return *chain.back().act.grant; }
};

// What a home declares of a passport's holder beyond its parties, and where the passport points.
struct Declarations {
  std::string model, label, node, status_at;
};

namespace detail {
// Whether an operative's declared controller issued some link of its chain: the root's principal, or a holder that delegated to it.
inline bool controller_in_chain(const std::string& controller, const std::vector<const mandates::Mandate*>& links) {
  for (const mandates::Mandate* link : links)
    if (link->principal == controller) return true;
  return false;
}
}  // namespace detail

struct Trust {
  std::vector<Head> homes, notaries;
};

// Where and how a passport is shown: the instant of the check, the community it is shown at, and the bearer credential shown with it.
// A part left empty is not checked.
struct Presentation {
  std::string at, host, credential;
};

// Whether a passport holds, from the document alone: every link is an act of mandate made by its principal and joined to the one
// before; the chain stands as the home's fold requires (its root answerable, each link narrowing the one above, each term in instants
// well formed); its holder and principal are those the passport names, and an operative's declared controller issued a link of it;
// every link is under the home's head; that head is one the verifier knows, or one a notary it knows witnessed; and, as shown, the term
// in instants holds, the mandate names the host, and the credential's digest is the one the mandate names. Whether the mandate was
// revoked after the head is its status at the home (Proposition 3), which no document can show.
inline apodeixis::kernel::Verdict verify(const Passport& p, const Trust& trust, const Presentation& shown = {}) {
  using apodeixis::kernel::Verdict;
  const std::string claim = "the passport " + p.home + " issued to " + p.holder.id + " holds";
  if (p.chain.empty()) return Verdict::no(claim, "the passport carries no mandate");
  mandates::Registry links;
  std::vector<const mandates::Mandate*> order;
  for (std::size_t k = 0; k < p.chain.size(); k++) {
    const acts::Act& a = p.chain[k].act;
    if (a.kind != "mandate" || !a.grant) return Verdict::no(claim, "link " + std::to_string(k + 1) + " of the chain is not an act of mandate", k);
    const mandates::Mandate& m = *a.grant;
    if (a.author != m.principal) return Verdict::no(claim, "the act that issued " + m.id + " is not made by its principal", k);
    if (k == 0 ? !m.parent.empty() : m.parent != p.chain[k - 1].act.grant->id)
      return Verdict::no(claim, "the links of the chain do not join: " + m.id + (k == 0 ? " is not a root" : " is not issued under the link before it"), k);
    if (!links.emplace(m.id, m).second) return Verdict::no(claim, "the chain names " + m.id + " twice", k);
  }
  for (const auto& link : p.chain) order.push_back(&links.at(link.act.grant->id));
  const mandates::Mandate& leaf = p.mandate();
  if (leaf.holder != p.holder.id) return Verdict::no(claim, "the mandate is held by " + leaf.holder + ", not by the holder the passport names");
  if (order.front()->principal != p.principal.id) return Verdict::no(claim, "the chain's root is issued by " + order.front()->principal + ", not by the principal the passport names");
  kernel::Parties ps;
  ps.add(p.principal);
  if (const auto f = mandates::faults(ps, links, leaf.id); !f.empty()) return Verdict::no(claim, "the chain carries no authority: " + f.front());
  if (!leaf.credential.empty() && !holdings::is_digest(leaf.credential)) return Verdict::no(claim, "the mandate names its credential otherwise than by a SHA-256 digest");
  if (p.holder.kind == kernel::Kind::operative && !detail::controller_in_chain(p.holder.controller, order))
    return Verdict::no(claim, "the operative's declared controller issued no link of its chain");
  std::set<std::size_t> places;
  for (std::size_t k = 0; k < p.chain.size(); k++) {
    const Link& l = p.chain[k];
    if (l.proof.size != p.head.size || !places.insert(l.proof.index).second ||
        !archeion::fixity::included(l.seal + "\x1e" + acts::envelope(l.act), l.proof, p.head.root))
      return Verdict::no(claim, "the act that issued " + l.act.grant->id + " is not under the home's head: it, or the passport, was changed", k);
  }
  bool direct = false, witnessed = false;
  for (const Head& h : trust.homes) direct = direct || same(h, p.head);
  if (!direct) {
    if (p.witnesses.empty()) return Verdict::no(claim, "the verifier does not know the home's head, and no notary witnessed it");
    std::string why = "no witness attests the home's head";
    for (const Witness& w : p.witnesses) {
      if (!attests(w, p.head)) continue;
      why = "no notary whose head the verifier knows witnessed the home's head";
      for (const Head& n : trust.notaries) witnessed = witnessed || same(n, w.at);
      if (witnessed) break;
    }
    if (!witnessed) return Verdict::no(claim, why);
  }
  if (!shown.at.empty()) {
    if (!mandates::is_instant(shown.at)) return Verdict::no(claim, "the instant of the check is not ISO 8601 in UTC (YYYY-MM-DDTHH:MM:SSZ)");
    if (!leaf.since.empty() && shown.at < leaf.since) return Verdict::no(claim, "the passport's term has not begun: it runs from " + leaf.since);
    if (!leaf.until.empty() && shown.at > leaf.until) return Verdict::no(claim, "the passport's term has ended: it ran to " + leaf.until);
  }
  if (!shown.host.empty() && !mandates::names(leaf, shown.host))
    return Verdict::no(claim, "the mandate does not name " + shown.host + " among the communities in which it may be used");
  if (!shown.credential.empty()) {
    if (leaf.credential.empty()) return Verdict::no(claim, "the mandate names no credential");
    if (archeion::fixity::sha256_hex(shown.credential) != leaf.credential) return Verdict::no(claim, "the credential shown is not the one whose digest the mandate names");
  }
  return Verdict::yes(claim, std::string("every link is an act of its home under a head ") + (direct ? "the verifier knows" : "a notary it knows witnessed") + ", and the chain stands",
                      p.chain.size());
}

// ---- The written form noetarium.passport/1 ---------------------------------------------------------------------------------------
//
//   {"format": "noetarium.passport/1", "issued": "<instant>",
//    "issuer": {"community", "node", "status at", "head": {"size", "root"},
//               "witnesses": [{"notary", "attester", "mandate", "turn", "when", "seal", "proof", "head": {"size", "root"}}]},
//    "principal": {"id", "kind"},
//    "holder": {"id", "kind", "controller", "provider", "model", "label"},
//    "mandate": {"id", "principal", "holder", "parent", "kinds", "matters", "communities", "allowance": {resource: n}, "slice": {resource: n},
//                "turns": {"from", "to"}, "term": {"from": <instant>, "to": <instant>}, "delegable", "reserves", "credential"},
//    "act": {"n", "seal", "author", "turn", "when", "mandate", "fields": {name: text}, "holders": [[party, position]],
//            "charges": [{"mandate", "resource", "amount"}], "envelope": <SHA-256 of the envelope's canonical text>},
//    "proof": {"leaf", "size", "path": [<digest>, ...]},
//    "chain": [{"mandate", "act", "proof"}, ...]}
//
// "mandate", "act" and "proof" are the holder's link; "chain" the links above it, root first. Everything under "mandate" and "act" is
// exactly what the act's envelope seals, so a verifier rebuilds the envelope from it; "node" and "status at" are pointers outside every
// seal. A part of unexpected shape is named among the faults of the reading, and a passport with any fault is not read.

namespace detail {

using Json = symploke::jsonld::Json;

inline Json whole(long long n) { return Json::num(static_cast<double>(n), std::to_string(n)); }
template <class Strings>
inline Json texts(const Strings& v) {
  Json out = Json::array();
  for (const auto& s : v) out.items.push_back(Json::string(s));
  return out;
}
inline Json amounts(const std::map<std::string, long long>& m) {
  Json out = Json::object();
  for (const auto& [k, n] : m) out.set(k, whole(n));
  return out;
}
inline Json head_json(const Head& h) {
  Json j = Json::object();
  j.set("size", whole(static_cast<long long>(h.size)));
  j.set("root", Json::string(archeion::fixity::hex(h.root)));
  return j;
}
inline Json proof_json(const Inclusion& p) {
  Json j = Json::object(), path = Json::array();
  for (const auto& d : p.path) path.items.push_back(Json::string(archeion::fixity::hex(d)));
  j.set("leaf", whole(static_cast<long long>(p.index)));
  j.set("size", whole(static_cast<long long>(p.size)));
  j.set("path", path);
  return j;
}
inline Json grant_json(const mandates::Mandate& m) {
  Json j = Json::object();
  for (const auto& [k, v] : std::vector<std::pair<const char*, const std::string*>>{{"id", &m.id}, {"principal", &m.principal}, {"holder", &m.holder}, {"parent", &m.parent}})
    j.set(k, Json::string(*v));
  j.set("kinds", texts(m.kinds));
  j.set("matters", texts(m.matters));
  if (!m.communities.empty()) j.set("communities", texts(m.communities));
  j.set("allowance", amounts(m.allowance));
  j.set("slice", amounts(m.slice));
  Json turns = Json::object();
  turns.set("from", whole(m.from));
  turns.set("to", whole(m.to));
  j.set("turns", turns);
  if (!m.since.empty() || !m.until.empty()) {
    Json term = Json::object();
    if (!m.since.empty()) term.set("from", Json::string(m.since));
    if (!m.until.empty()) term.set("to", Json::string(m.until));
    j.set("term", term);
  }
  j.set("delegable", Json::boolean(m.delegable));
  j.set("reserves", Json::boolean(m.reserves));
  if (!m.credential.empty()) j.set("credential", Json::string(m.credential));
  return j;
}
inline Json act_json(const Link& l) {
  const acts::Act& a = l.act;
  Json j = Json::object(), fields = Json::object();
  j.set("n", whole(static_cast<long long>(l.proof.index) + 1));
  j.set("seal", Json::string(l.seal));
  j.set("author", Json::string(a.author));
  j.set("turn", whole(a.turn));
  j.set("when", Json::string(a.when));
  j.set("mandate", Json::string(a.mandate));
  for (const auto& [k, v] : a.fields) fields.set(k, Json::string(v));
  j.set("fields", fields);
  if (!a.holders.empty()) {
    Json h = Json::array();
    for (const auto& [party, position] : a.holders) h.items.push_back(texts(std::vector<std::string>{party, position}));
    j.set("holders", h);
  }
  if (!a.charges.empty()) {
    Json c = Json::array();
    for (const acts::Charge& x : a.charges) {
      Json one = Json::object();
      one.set("mandate", Json::string(x.mandate));
      one.set("resource", Json::string(x.resource));
      one.set("amount", whole(x.amount));
      c.items.push_back(one);
    }
    j.set("charges", c);
  }
  j.set("envelope", Json::string(archeion::fixity::sha256_hex(acts::envelope(a))));
  return j;
}
inline Json link_json(const Link& l) {
  Json j = Json::object();
  j.set("mandate", grant_json(*l.act.grant));
  j.set("act", act_json(l));
  j.set("proof", proof_json(l.proof));
  return j;
}

// A reader that names every part of unexpected shape.
struct Reader {
  std::vector<std::string> faults;

  const Json* at(const Json* j, std::string_view k, const std::string& where) {
    const Json* v = j && j->is_object() ? j->get(k) : nullptr;
    if (!v) faults.push_back(where + " lacks \"" + std::string(k) + "\"");
    return v;
  }
  std::string text(const Json* j, std::string_view k, const std::string& where, bool required = true) {
    const Json* v = j && j->is_object() ? j->get(k) : nullptr;
    if (!v) {
      if (required) faults.push_back(where + " lacks \"" + std::string(k) + "\"");
      return {};
    }
    if (!v->is_string()) faults.push_back(where + "'s \"" + std::string(k) + "\" is not text");
    return v->is_string() ? v->text : std::string();
  }
  long long number(const Json* v, const std::string& where) {
    if (!v || !v->is_number()) {
      faults.push_back(where + " is not a whole number");
      return 0;
    }
    const std::string& lex = v->lexical;
    if (lex.empty()) return static_cast<long long>(v->number);
    std::size_t k = lex[0] == '-' ? 1 : 0;
    bool digits = k < lex.size() && lex.size() <= 18;
    for (std::size_t i = k; digits && i < lex.size(); i++) digits = lex[i] >= '0' && lex[i] <= '9';
    if (!digits) {
      faults.push_back(where + " is not a whole number");
      return 0;
    }
    return std::stoll(lex);
  }
  long long number(const Json* j, std::string_view k, const std::string& where) {
    const Json* v = j && j->is_object() ? j->get(k) : nullptr;
    return number(v, where + "'s \"" + std::string(k) + "\"");
  }
  std::vector<std::string> strings(const Json* j, std::string_view k, const std::string& where) {
    std::vector<std::string> out;
    const Json* v = j && j->is_object() ? j->get(k) : nullptr;
    if (!v) return out;
    if (!v->is_array()) {
      faults.push_back(where + "'s \"" + std::string(k) + "\" is not a list");
      return out;
    }
    for (const Json& i : v->items) {
      if (i.is_string()) out.push_back(i.text);
      else faults.push_back(where + "'s \"" + std::string(k) + "\" holds a value that is not text");
    }
    return out;
  }
  bool truth(const Json* j, std::string_view k, const std::string& where) {
    const Json* v = j && j->is_object() ? j->get(k) : nullptr;
    if (!v) return false;
    if (!v->is_bool()) faults.push_back(where + "'s \"" + std::string(k) + "\" is neither true nor false");
    return v->is_bool() && v->truth;
  }
  archeion::fixity::Digest digest(const std::string& text, const std::string& where) {
    if (!holdings::is_digest(text)) {
      faults.push_back(where + " is not a SHA-256 digest of 64 lower-case hexadecimal digits");
      return {};
    }
    return archeion::fixity::from_hex(text);
  }
  std::map<std::string, long long> amounts(const Json* j, std::string_view k, const std::string& where) {
    std::map<std::string, long long> out;
    const Json* v = j && j->is_object() ? j->get(k) : nullptr;
    if (!v) return out;
    if (!v->is_object()) {
      faults.push_back(where + "'s \"" + std::string(k) + "\" is not an object");
      return out;
    }
    for (const auto& [name, n] : v->members) out[name] = number(&n, where + "'s " + std::string(k) + " of " + name);
    return out;
  }
  Head head(const Json* j, const std::string& community, const std::string& where) {
    Head h;
    h.community = community;
    if (!j || !j->is_object()) {
      faults.push_back(where + " is not an object");
      return h;
    }
    h.size = static_cast<std::size_t>(number(j, "size", where));
    h.root = digest(text(j, "root", where), where + "'s root");
    return h;
  }
  Inclusion proof(const Json* j, const std::string& where) {
    Inclusion p;
    if (!j || !j->is_object()) {
      faults.push_back(where + " is not an object");
      return p;
    }
    p.index = static_cast<std::size_t>(number(j, "leaf", where));
    p.size = static_cast<std::size_t>(number(j, "size", where));
    for (const auto& d : strings(j, "path", where)) p.path.push_back(digest(d, where + "'s path"));
    return p;
  }
  mandates::Mandate grant(const Json* j, const std::string& where) {
    mandates::Mandate m;
    if (!j || !j->is_object()) {
      faults.push_back(where + " is not an object");
      return m;
    }
    m.id = text(j, "id", where), m.principal = text(j, "principal", where), m.holder = text(j, "holder", where), m.parent = text(j, "parent", where, false);
    for (const auto& k : strings(j, "kinds", where)) m.kinds.insert(k);
    for (const auto& k : strings(j, "matters", where)) m.matters.insert(k);
    for (const auto& k : strings(j, "communities", where)) m.communities.insert(k);
    m.allowance = amounts(j, "allowance", where), m.slice = amounts(j, "slice", where);
    if (const Json* t = at(j, "turns", where)) m.from = number(t, "from", where + "'s turns"), m.to = number(t, "to", where + "'s turns");
    if (const Json* t = j->get("term")) m.since = text(t, "from", where + "'s term", false), m.until = text(t, "to", where + "'s term", false);
    m.delegable = truth(j, "delegable", where), m.reserves = truth(j, "reserves", where);
    m.credential = text(j, "credential", where, false);
    return m;
  }
  Link link(const Json* grant_part, const Json* act_part, const Json* proof_part, const std::string& where) {
    Link l;
    l.act.kind = "mandate";
    l.act.grant = grant(grant_part, where + "'s mandate");
    const std::string w = where + "'s act";
    if (!act_part || !act_part->is_object()) faults.push_back(w + " is not an object");
    else {
      l.seal = text(act_part, "seal", w);
      digest(l.seal, w + "'s seal");
      l.act.author = text(act_part, "author", w), l.act.turn = number(act_part, "turn", w), l.act.when = text(act_part, "when", w, false), l.act.mandate = text(act_part, "mandate", w, false);
      if (const Json* f = act_part->get("fields")) {
        if (!f->is_object()) faults.push_back(w + "'s fields are not an object");
        else
          for (const auto& [k, v] : f->members) {
            if (v.is_string()) l.act.fields[k] = v.text;
            else faults.push_back(w + "'s field " + k + " is not text");
          }
      }
      if (const Json* h = act_part->get("holders"); h && h->is_array())
        for (const Json& x : h->items) {
          if (x.is_array() && x.items.size() == 2 && x.items[0].is_string() && x.items[1].is_string()) l.act.holders.push_back({x.items[0].text, x.items[1].text});
          else faults.push_back(w + " names a holder that is not a pair of a party and a position");
        }
      if (const Json* c = act_part->get("charges"); c && c->is_array())
        for (const Json& x : c->items) l.act.charges.push_back({text(&x, "mandate", w + "'s charge"), text(&x, "resource", w + "'s charge"), number(&x, "amount", w + "'s charge")});
    }
    l.proof = proof(proof_part, where + "'s proof");
    return l;
  }
};

}  // namespace detail

namespace detail {
inline Json witness_json(const Witness& w) {
  Json one = Json::object();
  one.set("notary", Json::string(w.notary));
  one.set("attester", Json::string(w.attester));
  one.set("mandate", Json::string(w.mandate));
  one.set("turn", whole(w.turn));
  one.set("when", Json::string(w.when));
  one.set("seal", Json::string(w.seal));
  one.set("proof", proof_json(w.proof));
  one.set("head", head_json(w.at));
  return one;
}
}  // namespace detail

// A witness written as a passport's issuer lists it, to be added to a passport's "witnesses".
inline std::string write(const Witness& w) { return symploke::jsonld::dump(detail::witness_json(w), 2); }

inline std::string write(const Passport& p) {
  using detail::Json;
  Json doc = Json::object(), issuer = Json::object(), witnesses = Json::array(), principal = Json::object(), holder = Json::object(), chain = Json::array();
  doc.set("format", Json::string("noetarium.passport/1"));
  doc.set("issued", Json::string(p.issued));
  issuer.set("community", Json::string(p.home));
  issuer.set("node", Json::string(p.node));
  issuer.set("status at", Json::string(p.status_at));
  issuer.set("head", detail::head_json(p.head));
  for (const Witness& w : p.witnesses) witnesses.items.push_back(detail::witness_json(w));
  issuer.set("witnesses", witnesses);
  doc.set("issuer", issuer);
  principal.set("id", Json::string(p.principal.id));
  principal.set("kind", Json::string(std::string(kernel::word(p.principal.kind))));
  doc.set("principal", principal);
  holder.set("id", Json::string(p.holder.id));
  holder.set("kind", Json::string(std::string(kernel::word(p.holder.kind))));
  holder.set("controller", Json::string(p.holder.controller));
  holder.set("provider", Json::string(p.holder.provider));
  holder.set("model", Json::string(p.model));
  holder.set("label", Json::string(p.label));
  doc.set("holder", holder);
  if (!p.chain.empty()) {
    doc.set("mandate", detail::grant_json(p.mandate()));
    doc.set("act", detail::act_json(p.chain.back()));
    doc.set("proof", detail::proof_json(p.chain.back().proof));
    for (std::size_t k = 0; k + 1 < p.chain.size(); k++) chain.items.push_back(detail::link_json(p.chain[k]));
  }
  doc.set("chain", chain);
  return symploke::jsonld::dump(doc, 2);
}

struct Reading {
  bool read = false;
  std::vector<std::string> faults;
  Passport passport;
};

// Reads a passport from its written form; text that is not JSON is refused with its line and column, and every part of unexpected
// shape is named.
inline Reading read_passport(std::string_view written) {
  using detail::Json;
  Reading r;
  Json doc;
  try {
    doc = symploke::jsonld::parse(written);
  } catch (const std::exception& e) {
    r.faults.push_back(std::string("the passport is not JSON: ") + e.what());
    return r;
  }
  if (!doc.is_object()) {
    r.faults.push_back("a passport is a JSON object");
    return r;
  }
  detail::Reader x;
  Passport& p = r.passport;
  if (x.text(&doc, "format", "the passport") != "noetarium.passport/1") x.faults.push_back("the passport's format is not noetarium.passport/1");
  p.issued = x.text(&doc, "issued", "the passport", false);
  if (const Json* issuer = x.at(&doc, "issuer", "the passport")) {
    p.home = x.text(issuer, "community", "the issuer"), p.node = x.text(issuer, "node", "the issuer", false), p.status_at = x.text(issuer, "status at", "the issuer", false);
    p.head = x.head(x.at(issuer, "head", "the issuer"), p.home, "the issuer's head");
    if (const Json* ws = issuer->get("witnesses"); ws && ws->is_array())
      for (std::size_t k = 0; k < ws->items.size(); k++) {
        const Json& w = ws->items[k];
        const std::string where = "witness " + std::to_string(k + 1);
        Witness one;
        one.notary = x.text(&w, "notary", where), one.attester = x.text(&w, "attester", where), one.mandate = x.text(&w, "mandate", where, false);
        one.turn = x.number(&w, "turn", where), one.when = x.text(&w, "when", where, false);
        one.seal = x.text(&w, "seal", where);
        x.digest(one.seal, where + "'s seal");
        one.proof = x.proof(x.at(&w, "proof", where), where + "'s proof");
        one.at = x.head(x.at(&w, "head", where), one.notary, where + "'s head");
        p.witnesses.push_back(one);
      }
  }
  for (auto [key, party] : {std::pair<const char*, kernel::Party*>{"principal", &p.principal}, {"holder", &p.holder}}) {
    const Json* j = x.at(&doc, key, "the passport");
    const std::string where = std::string("the ") + key;
    party->id = x.text(j, "id", where);
    const auto kind = kernel::kind_named(x.text(j, "kind", where));
    if (!kind) x.faults.push_back(where + " is of no declared kind");
    else party->kind = *kind;
    if (std::string_view(key) == "holder") {
      party->controller = x.text(j, "controller", where, false), party->provider = x.text(j, "provider", where, false);
      p.model = x.text(j, "model", where, false), p.label = x.text(j, "label", where, false);
    }
  }
  if (const Json* chain = doc.get("chain"); chain && chain->is_array())
    for (std::size_t k = 0; k < chain->items.size(); k++) {
      const Json& l = chain->items[k];
      p.chain.push_back(x.link(l.get("mandate"), l.get("act"), l.get("proof"), "link " + std::to_string(k + 1) + " of the chain"));
    }
  else if (doc.has("chain"))
    x.faults.push_back("the passport's chain is not a list");
  p.chain.push_back(x.link(x.at(&doc, "mandate", "the passport"), x.at(&doc, "act", "the passport"), x.at(&doc, "proof", "the passport"), "the holder's link"));
  r.faults = std::move(x.faults);
  r.read = r.faults.empty();
  return r;
}

}  // namespace thesmos::passports
