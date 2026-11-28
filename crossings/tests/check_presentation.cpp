// Checks of presentation and recognition (decision 0009): a host founded from an empty registry declares its parties by its founding;
// the part passports admitted within closure and kept public;
// a passport is refused while the host's record knows neither the home's head nor a recognised notary's, refused from an issuer the
// host does not recognise, and admitted once its office of status has reported the home's head and an office has recognised the home at
// the level of evidence, the agent and its principal then known parties with their control and provision, holding no position, the
// chain's links mandates of another community; refusals for a forged proof, a passport naming another host, an expired term, a
// presenter who is not the holder, a mandate other than the passport's, a community that admits no passports, an issuer not named, a
// holder known with another controller; a passport witnessed by a notary admitted at a host that accepts only witnessed heads, once the
// notary is recognised; recognition refused without an office, at an unknown level, of oneself; reports of status read only from the
// office, a revocation at the home effective at the next report; on 300 random chronicles what a host admits after a revocation at the
// home is at most the slice (Proposition 19); on 200, two hosts never pass the root together, while without reserving they admit 160
// under a root of 100 (Proposition 20); on 100, a host's state is given again by its record alone, from an empty registry, and by
// records that differ only in their bodies (Proposition 21).

#include <thesmos/crossings/crossings.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace thesmos;
using acts::Act;
using chronicle::State;
using kernel::Kind;
using kernel::Parties;
using mandates::Mandate;
namespace x = thesmos::crossings;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}, std::string when = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate), a.when = std::move(when);
  return a;
}
std::string day(long long d) { return std::string("2026-12-") + (d < 10 ? "0" : "") + std::to_string(d) + "T00:00:00Z"; }
const std::string& workshop_text() {
  static const std::string t = slurp(std::string(THESMOS_DATA) + "/examples/workshop.json");
  return t;
}
// The illustrative constitution as a host: passports accepted as said, the holder's provider required, the steward the office of status
// and the office that recognises other communities.
constitutions::Constitution host_rules(const std::string& accepted, bool can_recognise = true) {
  auto doc = symploke::jsonld::parse(workshop_text());
  auto px = symploke::jsonld::parse(accepted.front() == '[' ? R"({"accepted from": )" + accepted + R"(, "require": ["provider"], "status by": "steward"})"
                                                            : R"({"accepted from": ")" + accepted + R"(", "require": ["provider"], "status by": "steward"})");
  doc.set("passports", px);
  if (can_recognise) doc.get("choice")->items.push_back(symploke::jsonld::parse(R"({"who": "steward", "may": ["recognise"]})"));
  return constitutions::read(symploke::jsonld::dump(doc));
}
// A host founded from an empty registry: its founding names it and declares its parties.
State host(const std::string& name, const constitutions::Constitution& c, const std::string& extra_parties = {}, const std::string& when = {}) {
  State s(Parties{}, c);
  Act f = act("sam", "found", 0, {{"community", name}, {"parties", R"([{"id": "sam", "kind": "person"}, {"id": "rae", "kind": "person"})" + extra_parties + "]"}}, {}, when);
  f.holders = {{"sam", "steward"}, {"rae", "reviewer"}};
  chronicle::step(s, f);
  return s;
}
// A home on the illustrative constitution, founded by its person, its operatives declared.
State home() {
  Parties ps;
  ps.add({"ana", Kind::person});
  ps.add({"ana/scribe", Kind::operative, "ana", "provider-a"});
  State s(ps, constitutions::read(workshop_text()));
  Act f = act("ana", "found", 0, {{"community", "ana-home"}});
  f.holders = {{"ana", "steward"}};
  chronicle::step(s, f);
  return s;
}
Mandate root_of(long long allowance = 100, bool instants = true) {
  Mandate m;
  m.id = "m-scribe", m.principal = "ana", m.holder = "ana/scribe";
  m.kinds = {"present", "say", "report", "mandate", "join"};
  m.allowance = {{"runs", allowance}};
  m.slice = {{"runs", allowance}};
  m.from = 0, m.to = 100000;
  if (instants) m.since = day(1), m.until = day(28);
  return m;
}
Mandate split_of(const std::string& host, long long ceiling, long long slice, bool instants = true) {
  Mandate m = root_of(100, instants);
  m.id = "m-scribe-" + host, m.parent = "m-scribe", m.reserves = true;
  m.kinds = {"present", "say"};
  m.allowance = {{"runs", ceiling}}, m.slice = {{"runs", slice}};
  m.communities = {host};
  if (instants) m.since = day(2), m.until = day(20);
  m.credential = archeion::fixity::sha256_hex("token for " + host);
  return m;
}
Act issuing(const Mandate& m, long long turn, std::string under = {}) {
  Act a = act(m.principal, "mandate", turn, {}, std::move(under));
  a.grant = m;
  return a;
}
x::Passport passport_of(const State& h, const std::string& id, const std::string& at = {}) {
  const auto i = x::issue("ana-home", h, id, at, {"model-7", "the scribe", {}, {}});
  if (!i.admitted) throw std::runtime_error("no passport: " + i.reason);
  return i.passport;
}
Act report_head(const x::Head& h, long long turn, const std::string& when = {}, const std::string& by = "sam") {
  return act(by, "report", turn, {{"head of", h.community}, {"size", std::to_string(h.size)}, {"root", archeion::fixity::hex(h.root)}}, {}, when);
}
Act recognising(const std::string& of, const std::string& level, long long turn, const std::string& when = {}) { return act("sam", "recognise", turn, {{"of", of}, {"level", level}}, {}, when); }
Act presenting(const x::Passport& p, long long turn, const std::string& when = {}) { return act(p.holder.id, "present", turn, {{"passport", x::write(p)}}, p.mandate().id, when); }
Act status(const std::string& mandate, const std::string& st, long long turn, const std::string& when = {}, const std::string& by = "sam") {
  return act(by, "report", turn, {{"mandate status", mandate}, {"status", st}}, {}, when);
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- 1. Parties declared by acts --------------------------------------------------------------------------------------------------
  {
    Parties ps;
    ps.add({"a/agent", Kind::operative, "a"});
    const auto same = ps.declare({"a/agent", Kind::operative, "a"});
    const auto other = ps.declare({"a/agent", Kind::operative, "b"});
    const auto kind = ps.declare({"a/agent", Kind::person});
    s.require("a party is declared once: declared again alike it is accepted, with another controller or of another kind it is refused", same.empty() && !other.empty() && !kind.empty(), other);
    const State h = host("atelier", host_rules("recognised"));
    s.require("a host founded from an empty registry holds the parties its founding declares, as acts, and the name its founding gives it",
              h.acts.size() == 1 && h.parties.has("sam") && h.parties.has("rae") && h.declared.size() == 2 && h.community == "atelier" && h.community_declared);
    State broken(Parties{}, host_rules("recognised"));
    Act f = act("sam", "found", 0, {{"parties", R"([{"id": "sam", "kind": "person"}, {"id": "sam", "kind": "community"}])"}});
    f.holders = {{"sam", "steward"}};
    const auto refused = chronicle::step(broken, f);
    s.require("a founding that declares a party twice otherwise is refused and declares nothing", !refused.admitted && !broken.parties.has("sam"), refused.reason);
    // The part passports within closure (condition 1), and always public.
    auto faults_of = [](const std::string& part, const std::string& visibility = {}) {
      auto doc = symploke::jsonld::parse(workshop_text());
      doc.set("passports", symploke::jsonld::parse(part));
      if (!visibility.empty()) doc.get("records")->set("visibility", symploke::jsonld::parse(visibility));
      std::vector<std::string> out;
      for (const auto& f : constitutions::admission(constitutions::read(symploke::jsonld::dump(doc)))) out.push_back(std::to_string(f.condition) + ": " + f.reason);
      return out;
    };
    const auto clean = faults_of(R"({"accepted from": ["ana-home"], "require": ["provider", "model"], "status by": "steward"})");
    const auto unknown = faults_of(R"({"accepted from": "anyone", "status by": "steward"})");
    const auto empty_list = faults_of(R"({"accepted from": [], "status by": "steward"})");
    const auto required = faults_of(R"({"accepted from": "recognised", "require": ["name"], "status by": "steward"})");
    const auto office = faults_of(R"({"accepted from": "recognised", "status by": "consul"})");
    const auto hidden = faults_of(R"({"accepted from": "recognised", "status by": "steward"})", R"({"passports": "members"})");
    const auto mute = faults_of(R"({"accepted from": "recognised", "status by": "reviewer"})");
    s.require("the part passports is admitted when it accepts issuers witnessed, recognised or named, requires a provider or a model, and names a declared office "
              "that may report; an unknown acceptance, an empty list, another requirement, an undeclared office, an office the choice rules do not let report and a "
              "passports part kept from the public are each found",
              clean.empty() && unknown.size() == 1 && empty_list.size() == 1 && required.size() == 1 && office.size() == 1 && mute.size() == 1 &&
                  mute.front().find("may not report") != std::string::npos && hidden.size() == 1 && hidden.front()[0] == '9',
              (clean.empty() ? std::string() : clean.front()) + " / " + (hidden.empty() ? std::string("no fault kept from the public") : hidden.front()));
  }

  // ---- 2. A passport presented at a host ------------------------------------------------------------------------------------------
  State hm = home();
  chronicle::step(hm, issuing(root_of(), 1));
  chronicle::step(hm, issuing(split_of("atelier", 60, 10), 2));
  chronicle::step(hm, issuing(split_of("forum", 30, 10), 3));
  const x::Passport pass = passport_of(hm, "m-scribe-atelier", day(3));
  const x::Passport elsewhere = passport_of(hm, "m-scribe-forum", day(3));
  const x::Head home_head = x::head("ana-home", hm);
  State h = host("atelier", host_rules("recognised"), {}, day(1));
  {
    const auto unknown = chronicle::step(h, presenting(pass, 1, day(4)));
    s.require("a passport is refused while the host's record knows neither the home's head nor a notary's that witnessed it",
              !unknown.admitted && unknown.reason.find("does not know the home's head") != std::string::npos, unknown.reason);
    chronicle::step(h, report_head(home_head, 1, day(4)));
    const auto unrecognised = chronicle::step(h, presenting(pass, 1, day(4)));
    s.require("with the home's head reported by the office of status, a passport from an issuer the host does not recognise is refused",
              !unrecognised.admitted && unrecognised.reason.find("does not so recognise ana-home") != std::string::npos, unrecognised.reason);
    chronicle::step(h, recognising("ana-home", "evidence", 1, day(4)));
    const std::size_t positions = h.positions.size();
    const auto admitted = chronicle::step(h, presenting(pass, 1, day(4)));
    s.require("once the host recognises the home at the level of evidence the passport is admitted: the agent and its principal become known parties with their "
              "control and provision, holding no position, and the chain's links mandates of another community",
              admitted.admitted && h.parties.has("ana/scribe") && h.parties.at("ana/scribe").controller == "ana" && h.parties.at("ana/scribe").provider == "provider-a" &&
                  h.parties.has("ana") && h.declared.count("ana/scribe") && h.positions.size() == positions && h.foreign.count("m-scribe") && h.foreign.count("m-scribe-atelier") &&
                  h.presented.count("ana/scribe"),
              admitted.reason);
    Act said = act("ana/scribe", "say", 2, {}, "m-scribe-atelier", day(5));
    said.charges = {{"m-scribe-atelier", "runs", 4}};
    const auto act_ok = chronicle::step(h, said);
    const auto late = chronicle::step(h, act("ana/scribe", "say", 3, {}, "m-scribe-atelier", day(21)));
    s.require("the agent acts under its passport at the host within its term, charged against its split, and not after its term in instants",
              act_ok.admitted && h.used["m-scribe-atelier"]["runs"] == 4 && !late.admitted && late.reason.find("term in instants") != std::string::npos, act_ok.reason + " / " + late.reason);
  }
  {
    State g = host("atelier", host_rules("recognised"), {}, day(1));
    chronicle::step(g, report_head(home_head, 1, day(4)));
    chronicle::step(g, recognising("ana-home", "evidence", 1, day(4)));
    x::Passport forged = pass;
    forged.chain.back().proof.path.front()[0] ^= 1;
    const auto f = chronicle::step(g, presenting(forged, 1, day(4)));
    const auto other = chronicle::step(g, presenting(elsewhere, 1, day(4)));
    const auto expired = chronicle::step(g, presenting(pass, 1, day(21)));
    s.require("refused: a forged proof, a passport naming another host, and a passport presented after its term",
              !f.admitted && f.reason.find("not under the home's head") != std::string::npos && !other.admitted && other.reason.find("does not name atelier") != std::string::npos &&
                  !expired.admitted && expired.reason.find("has ended") != std::string::npos,
              f.reason + " / " + other.reason + " / " + expired.reason);
    Act by_other = presenting(pass, 1, day(4));
    by_other.author = "sam";
    Act wrong_mandate = presenting(pass, 1, day(4));
    wrong_mandate.mandate = "m-scribe";
    const auto o1 = chronicle::step(g, by_other), o2 = chronicle::step(g, wrong_mandate);
    State none = host("atelier", constitutions::read(workshop_text()), {}, day(1));
    const auto o3 = chronicle::step(none, presenting(pass, 1, day(4)));
    State named = host("atelier", host_rules(R"(["another-home"])"), {}, day(1));
    chronicle::step(named, report_head(home_head, 1, day(4)));
    const auto o4 = chronicle::step(named, presenting(pass, 1, day(4)));
    s.require("refused: a presentation by another than the holder, under another mandate than the passport's, at a community that admits no passports, and from an "
              "issuer it does not name",
              !o1.admitted && o1.reason.find("presented by its holder") != std::string::npos && !o2.admitted && o2.reason.find("own mandate") != std::string::npos && !o3.admitted &&
                  o3.reason.find("admits no passports") != std::string::npos && !o4.admitted && o4.reason.find("does not accept passports issued by ana-home") != std::string::npos,
              o1.reason + " / " + o2.reason + " / " + o3.reason + " / " + o4.reason);
    State known = host("atelier", host_rules("recognised"), R"(, {"id": "ana/scribe", "kind": "operative", "controller": "zed"}, {"id": "zed", "kind": "person"})", day(1));
    chronicle::step(known, report_head(home_head, 1, day(4)));
    chronicle::step(known, recognising("ana-home", "evidence", 1, day(4)));
    const auto twice = chronicle::step(known, presenting(pass, 1, day(4)));
    s.require("a holder the host knows with another controller is refused, and nothing it would have declared is kept",
              !twice.admitted && twice.reason.find("another controller") != std::string::npos && !known.parties.has("ana") && !known.foreign.count("m-scribe"), twice.reason);
  }
  {
    // A head witnessed by a notary, at a host that accepts only witnessed heads.
    Parties ps;
    ps.add({"clerk", Kind::person});
    ps.add({"scrivener", Kind::person});
    State n(ps, constitutions::read(workshop_text()));
    Act f = act("clerk", "found", 0, {{"community", "registry"}});
    f.holders = {{"clerk", "steward"}, {"scrivener", "reviewer"}};
    chronicle::step(n, f);
    const auto w = x::witness(n, "registry", "scrivener", 1, home_head);
    x::Passport witnessed = pass;
    witnessed.witnesses.push_back(w.witness);
    State g = host("atelier", host_rules("witnessed"), {}, day(1));
    chronicle::step(g, report_head(x::head("registry", n), 1, day(4)));
    const auto before = chronicle::step(g, presenting(witnessed, 1, day(4)));
    chronicle::step(g, recognising("registry", "evidence", 1, day(4)));
    const auto after = chronicle::step(g, presenting(witnessed, 1, day(4)));
    State plain = host("atelier", host_rules("witnessed"), {}, day(1));
    chronicle::step(plain, report_head(home_head, 1, day(4)));
    chronicle::step(plain, recognising("ana-home", "evidence", 1, day(4)));
    const auto direct = chronicle::step(plain, presenting(pass, 1, day(4)));
    s.require("a host that accepts witnessed heads refuses a passport until it recognises the witnessing notary at the level of evidence, then admits it without knowing "
              "the home's head; and refuses one no notary witnessed, though it knows and recognises its home",
              w.admitted && !before.admitted && after.admitted && !direct.admitted && direct.reason.find("no such notary witnessed") != std::string::npos,
              before.reason + " / " + after.reason + " / " + direct.reason);
  }

  // ---- 3. Recognition and reports of status ------------------------------------------------------------------------------------------
  {
    State g = host("atelier", host_rules("recognised", false));
    const auto unempowered = chronicle::step(g, recognising("ana-home", "evidence", 1));
    State k = host("atelier", host_rules("recognised"));
    const auto bad_level = chronicle::step(k, recognising("ana-home", "friendly", 1));
    const auto self = chronicle::step(k, recognising("atelier", "decisions", 1));
    const auto by_rae = chronicle::step(k, act("rae", "recognise", 1, {{"of", "ana-home"}, {"level", "decisions"}}));
    const auto matter = chronicle::step(k, act("sam", "recognise", 1, {{"of", "ana-home"}, {"level", "undertakings"}, {"matter", "the catalogue"}}));
    s.require("recognition is refused where no office is empowered to recognise, at an unknown level, of the community itself, and by an office not empowered; it may "
              "be set for one matter apart",
              !unempowered.admitted && unempowered.reason.find("an office the choice rules empower") != std::string::npos && !bad_level.admitted && !self.admitted && !by_rae.admitted &&
                  matter.admitted && chronicle::recognition(k, "ana-home", "the catalogue") == "undertakings" && chronicle::recognition(k, "ana-home") == "none",
              unempowered.reason);
    const auto unknown = chronicle::step(h, status("m-nothing", "revoked", 3, day(6)));
    const auto by_member = chronicle::step(h, status("m-scribe-atelier", "revoked", 3, day(6), "rae"));
    const bool still = h.foreign.at("m-scribe-atelier").status == "presented";
    chronicle::step(hm, act("ana", "revoke", 4, {{"which", "m-scribe-atelier"}}));
    const auto revoked = chronicle::step(h, status("m-scribe-atelier", "revoked", 3, day(6)));
    const auto after = chronicle::step(h, act("ana/scribe", "say", 3, {}, "m-scribe-atelier", day(7)));
    const auto again = chronicle::step(h, presenting(pass, 3, day(7)));
    s.require("a report of status naming no mandate of another community is refused, a report by another than the office changes nothing, and the office's report of the home's "
              "revocation makes every later act under the passport, and its presentation again, refused",
              !unknown.admitted && !by_member.admitted && still && revoked.admitted && !after.admitted && after.reason.find("stands revoked at its home") != std::string::npos &&
                  !again.admitted,
              unknown.reason + " / " + after.reason + " / " + again.reason);
  }

  // ---- 4. Proposition 19: what a revocation at the home costs a host --------------------------------------------------------------
  {
    calx::random::Stream r(53, 4, 0, 0);
    int over = 0, revoked_runs = 0, admitted_runs = 0;
    std::string first;
    long long most_lost = 0;
    for (int trial = 0; trial < 300; trial++) {
      State ho = home();
      const long long budget = between(r, 30, 400), slice = between(r, 5, 40), cost = between(r, 1, slice);
      chronicle::step(ho, issuing(root_of(1000, false), 1));
      chronicle::step(ho, issuing(split_of("atelier", budget, slice, false), 2));
      const x::Passport p = passport_of(ho, "m-scribe-atelier");
      State hs = host("atelier", host_rules("recognised"));
      chronicle::step(hs, report_head(x::head("ana-home", ho), 0));
      chronicle::step(hs, recognising("ana-home", "evidence", 0));
      if (!chronicle::step(hs, presenting(p, 0)).admitted) {
        over++;
        continue;
      }
      const long long window = between(r, 1, 15), at = between(r, 3, 100);  // after the home's acts of issue, at turns 1 and 2
      long long lost = 0;
      for (long long t = 1; t <= 120; t++) {
        if (t == at && !chronicle::step(ho, act("ana", "revoke", t, {{"which", "m-scribe-atelier"}})).admitted && first.empty()) first = "a revocation refused at turn " + std::to_string(t);
        if (t % window == 0) {
          const bool stands = mandates::faults(ho.parties, ho.mandates, "m-scribe-atelier", ho.revoked).empty();
          chronicle::step(hs, status("m-scribe-atelier", stands ? "standing" : "revoked", t));
        }
        Act a = act("ana/scribe", "say", t, {}, "m-scribe-atelier");
        a.charges = {{"m-scribe-atelier", "runs", cost}};
        if (chronicle::step(hs, a).admitted) {
          admitted_runs++;
          if (t >= at) lost += cost;
        }
      }
      revoked_runs += lost > 0;
      most_lost = std::max(most_lost, lost);
      if (lost > std::min(slice, budget) && first.empty())
        first = "budget " + std::to_string(budget) + ", slice " + std::to_string(slice) + ", cost " + std::to_string(cost) + ", window " + std::to_string(window) + ", revoked at " +
                std::to_string(at) + ", lost " + std::to_string(lost);
      over += lost > std::min(slice, budget);
    }
    s.require("on 300 random chronicles of a home and a host, what the host admits after a revocation at the home, until its office reports it, is at most the "
              "split's slice and at most its budget (Proposition 19; " + std::to_string(revoked_runs) + " lost something, at most " + std::to_string(most_lost) + ")",
              over == 0 && revoked_runs > 20 && admitted_runs > 3000, first);
  }

  // ---- 5. Proposition 20: hosts that each charge their own split --------------------------------------------------------------------
  {
    calx::random::Stream r(59, 5, 0, 0);
    int over = 0, both = 0;
    for (int trial = 0; trial < 200; trial++) {
      State ho = home();
      const long long ceiling = between(r, 50, 300);
      chronicle::step(ho, issuing(root_of(ceiling, false), 1));
      const long long a = between(r, 1, ceiling), b = between(r, 1, ceiling);
      const bool has_a = chronicle::step(ho, issuing(split_of("atelier", a, a, false), 2)).admitted;
      const bool has_b = chronicle::step(ho, issuing(split_of("forum", b, b, false), 3)).admitted;
      long long total = 0;
      int hosts = 0;
      for (const auto& [name, held, cap] : std::vector<std::tuple<std::string, bool, long long>>{{"atelier", has_a, a}, {"forum", has_b, b}}) {
        if (!held) continue;
        State hs = host(name, host_rules("recognised"));
        chronicle::step(hs, report_head(x::head("ana-home", ho), 0));
        chronicle::step(hs, recognising("ana-home", "evidence", 0));
        if (!chronicle::step(hs, presenting(passport_of(ho, "m-scribe-" + name), 0)).admitted) continue;
        hosts++;
        for (int k = 1; k <= 40; k++) {
          Act c = act("ana/scribe", "say", k, {}, "m-scribe-" + name);
          c.charges = {{"m-scribe-" + name, "runs", between(r, 1, 30)}};
          chronicle::step(hs, c);
        }
        const long long here = hs.used["m-scribe-" + name]["runs"];
        over += here > cap;
        total += here;
      }
      both += hosts == 2;
      over += total > ceiling;
    }
    State ho = home();
    Mandate root = root_of(100, false);
    root.delegable = true;
    chronicle::step(ho, issuing(root, 1));
    long long admitted = 0;
    for (const char* name : {"atelier", "forum"}) {
      Mandate c = root;
      c.id = std::string("m-") + name, c.parent = "m-scribe", c.principal = "ana/scribe", c.delegable = false, c.communities = {name};
      c.kinds = {"present", "say"};
      c.allowance = {{"runs", 80}}, c.slice = {{"runs", 80}};
      c.credential = archeion::fixity::sha256_hex(name);
      chronicle::step(ho, issuing(c, 2, "m-scribe"));
    }
    for (const char* name : {"atelier", "forum"}) {
      State hs = host(name, host_rules("recognised"));
      chronicle::step(hs, report_head(x::head("ana-home", ho), 0));
      chronicle::step(hs, recognising("ana-home", "evidence", 0));
      chronicle::step(hs, presenting(passport_of(ho, std::string("m-") + name), 0));
      Act c = act("ana/scribe", "say", 1, {}, std::string("m-") + name);
      c.charges = {{std::string("m-") + name, "runs", 80}};
      admitted += chronicle::step(hs, c).admitted ? 80 : 0;
    }
    s.require("on 200 random homes, hosts that each admit their own split's passport and charge under it never pass the split, nor the root together (Proposition 20; " +
                  std::to_string(both) + " with two hosts)",
              over == 0 && both > 30);
    s.require("without reserving, two hosts admitting children of 80 under a root of 100, each knowing only its own, admit 160", admitted == 160);
  }

  // ---- 6. Proposition 21: the state from the record alone --------------------------------------------------------------------------
  {
    calx::random::Stream r(61, 6, 0, 0);
    State ho = home();
    chronicle::step(ho, issuing(root_of(1000, false), 1));
    chronicle::step(ho, issuing(split_of("atelier", 500, 500, false), 2));
    const x::Passport p = passport_of(ho, "m-scribe-atelier");
    const x::Head hh = x::head("ana-home", ho);
    int differ = 0, replays = 0, bodies = 0, presented = 0;
    for (int trial = 0; trial < 100; trial++) {
      const long long extra = between(r, 0, 4);
      std::string declared;
      for (long long i = 0; i < extra; i++) declared += R"(, {"id": "p)" + std::to_string(i) + R"(", "kind": "person"})";
      const auto rules = host_rules("recognised");
      State hs = host("atelier", rules, declared);
      for (long long t = 1; t <= 40; t++) {
        const long long pick = between(r, 0, 6);
        const std::string who = between(r, 0, 1) ? "sam" : (extra ? "p" + std::to_string(between(r, 0, extra - 1)) : "rae");
        Act a;
        if (pick == 0) a = report_head(hh, t);
        else if (pick == 1) a = recognising("ana-home", between(r, 0, 1) ? "evidence" : "exists", t);
        else if (pick == 2) a = presenting(p, t);
        else if (pick == 3) {
          a = act("ana/scribe", "say", t, {}, "m-scribe-atelier");
          a.charges = {{"m-scribe-atelier", "runs", between(r, 1, 5)}};
        } else if (pick == 4) a = act(who, "claim", t, {{"id", "k" + std::to_string(t)}, {"against", "the catalogue"}});
        else a = act(who, "say", t);
        a.body = "body " + std::to_string(between(r, 0, 99));
        chronicle::step(hs, a);
      }
      presented += hs.presented.count("ana/scribe") > 0;
      State again;
      const auto v = chronicle::replay(Parties{}, rules, hs.acts, hs.journal, &again);
      replays += v.holds();
      differ += chronicle::relational_digest(again) != chronicle::relational_digest(hs);
      State other(Parties{}, rules);
      for (Act a : hs.acts) {
        a.body += " (rewritten)";
        chronicle::step(other, a);
      }
      bodies += other.acts.size() == hs.acts.size() && chronicle::relational_digest(other) == chronicle::relational_digest(hs) && other.journal.head() != hs.journal.head();
    }
    s.require("on 100 random host chronicles, the parties declared by founding and presentation, each replays from an empty registry to the same relational state "
              "(Proposition 21; " + std::to_string(presented) + " with an agent presented)",
              replays == 100 && differ == 0 && presented > 50);
    s.require("and a chronicle that differs only in its bodies gives the same relational state under other seals (Proposition 1 kept)", bodies == 100);
  }
  return s.done();
}
