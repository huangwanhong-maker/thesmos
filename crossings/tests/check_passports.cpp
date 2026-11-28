// Checks of passports (decision 0007): a home issues a passport for an operative's mandate, and anyone who knows the home's head, or
// the head of a notary that witnessed it, verifies it from the document alone, written and read back; a forged proof, a head no
// notary witnessed, a witness by a notary the verifier does not know, an expired term, a credential whose digest differs and a host
// the mandate does not name are each refused with its reason; on 300 random homes every passport issued verifies and every alteration
// of a sealed part is refused (Proposition 11); the home refuses a mandate that names its credential otherwise than by digest, states
// a term in instants badly or widens the communities or the term of its parent, and an act outside its term in instants; reserving
// splits issued one for each host never pass the root's ceilings, on 200 random homes, nor do hosts that each charge only their own
// (Proposition 12), while without reserving two children of 80 under a root of 100 let two hosts admit 160; a draw under a reserving
// split is charged from it down; and a record made before decision 0007 seals exactly as Python's hashlib computes it under the
// envelope as it was written then (Proposition 13).

#include <thesmos/crossings/crossings.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace thesmos;
using acts::Act;
using chronicle::State;
using kernel::Kind;
using kernel::Parties;
using mandates::Mandate;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }

Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author);
  a.kind = std::move(kind);
  a.turn = turn;
  a.fields = std::move(fields);
  a.mandate = std::move(mandate);
  return a;
}
Act issuing(const Mandate& m, long long turn, std::string under = {}) {
  Act a = act(m.principal, "mandate", turn, {}, std::move(under));
  a.grant = m;
  return a;
}
std::string digest_of(const std::string& token) { return archeion::fixity::sha256_hex(token); }
std::string day(long long d) { return std::string("2026-12-") + (d < 10 ? "0" : "") + std::to_string(d) + "T00:00:00Z"; }

const constitutions::Constitution& workshop() {
  static const constitutions::Constitution c = constitutions::read(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
  return c;
}

// A home on the illustrative constitution, founded by its person; its operatives declared with their controllers and providers.
State home() {
  Parties ps;
  ps.add({"ana", Kind::person});
  ps.add({"ana/scribe", Kind::operative, "ana", "provider-a"});
  ps.add({"ana/helper", Kind::operative, "ana/scribe", "provider-b"});
  ps.add({"zed", Kind::person});
  ps.add({"zed/agent", Kind::operative, "zed", "provider-a"});
  State s(ps, workshop());
  Act found = act("ana", "found", 0);
  found.holders = {{"ana", "steward"}};
  chronicle::step(s, found);
  return s;
}
// A notary community on the same constitution, whose reviewer attests heads.
State notary() {
  Parties ps;
  ps.add({"clerk", Kind::person});
  ps.add({"scrivener", Kind::person});
  ps.add({"passer", Kind::person});
  State s(ps, workshop());
  Act found = act("clerk", "found", 0);
  found.holders = {{"clerk", "steward"}, {"scrivener", "reviewer"}};
  chronicle::step(s, found);
  return s;
}

Mandate root_of(long long allowance = 100) {
  Mandate m;
  m.id = "m-scribe", m.principal = "ana", m.holder = "ana/scribe";
  m.kinds = {"present", "apply", "consult", "deposit", "commit", "deliver", "say", "report", "mandate"};
  m.matters = {"the catalogue"};
  m.allowance = {{"runs", allowance}};
  m.slice = {{"runs", 10}};
  m.from = 0, m.to = 1000;
  m.since = day(1), m.until = day(28);
  return m;
}
// A split of the root for one host: issued by the root's principal for the same holder, reserving its ceiling.
Mandate split_of(const std::string& host, long long ceiling, const std::string& token) {
  Mandate m = root_of();
  m.id = "m-scribe-" + host, m.parent = "m-scribe", m.reserves = true;
  m.kinds = {"present", "apply", "consult", "say"};
  m.allowance = {{"runs", ceiling}};
  m.slice = {{"runs", std::min<long long>(10, ceiling)}};
  m.communities = {host};
  m.since = day(2), m.until = day(20);
  m.credential = digest_of(token);
  return m;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  namespace x = crossings;

  // ---- 1. A passport issued and verified ----------------------------------------------------------------------------------------
  State h = home();
  s.require("the person issues the operative's root and a split of it for the host, reserving its ceiling",
            chronicle::step(h, issuing(root_of(), 1)).admitted && chronicle::step(h, issuing(split_of("atelier", 60, "token-atelier"), 2)).admitted);
  const auto issued = x::issue("ana-household", h, "m-scribe-atelier", day(3), {"model-7", "the scribe", "http://node.invalid/", "http://node.invalid/c/ana-household/mandates/m-scribe-atelier"});
  s.require("the home issues a passport of the split, carrying both links of its chain", issued.admitted && issued.passport.chain.size() == 2, issued.reason);
  const x::Passport& pass = issued.passport;
  const x::Head home_head = x::head("ana-household", h);
  const x::Presentation shown{day(5), "atelier", "token-atelier"};
  const auto direct = x::verify(pass, {{home_head}, {}}, shown);
  s.require("anyone who knows the home's head verifies the passport, its term, its host and its credential, from the document alone", direct.holds(), direct.reason);
  {
    const std::string text = x::write(pass);
    const auto back = x::read_passport(text);
    s.require("written as noetarium.passport/1 and read back, the passport is written the same and still verifies",
              back.read && x::write(back.passport) == text && x::verify(back.passport, {{home_head}, {}}, shown).holds(), back.faults.empty() ? "" : back.faults.front());
    const auto broken = x::read_passport(text.substr(0, text.size() / 2));
    std::string mangled = text;
    mangled.replace(mangled.find("noetarium.passport/1"), 20, "noetarium.passport/9");
    const auto wrong = x::read_passport(mangled);
    s.require("a passport that is not JSON is refused with its line and column, and one of another format is refused by name",
              !broken.read && broken.faults.front().find("line") != std::string::npos && !wrong.read && wrong.faults.front().find("format") != std::string::npos,
              broken.faults.empty() ? "" : broken.faults.front());
  }

  // ---- 2. Witnesses, forgeries, terms, hosts, credentials --------------------------------------------------------------------------
  {
    State n = notary();
    const auto w = x::witness(n, "registry", "scrivener", 1, home_head);
    s.require("a notary's reviewer witnesses the home's head in the notary's own chronicle", w.admitted, w.reason);
    x::Passport with = pass;
    with.witnesses.push_back(w.witness);
    const x::Head notary_head = x::head("registry", n);
    const auto by_notary = x::verify(with, {{}, {notary_head}}, shown);
    s.require("one who knows only the notary's head verifies the passport through its witness", by_notary.holds(), by_notary.reason);
    const auto unwitnessed = x::verify(pass, {{}, {notary_head}}, shown);
    s.require("a head no notary witnessed, unknown to the verifier, is refused", unwitnessed.fails() && unwitnessed.reason.find("no notary witnessed") != std::string::npos, unwitnessed.reason);
    State other = notary();
    chronicle::step(other, act("clerk", "say", 1));
    const auto stranger = x::verify(with, {{}, {x::head("elsewhere", other)}}, shown);
    x::Passport moved = with;
    moved.witnesses[0].turn += 1;
    const auto misattested = x::verify(moved, {{}, {notary_head}}, shown);
    s.require("a witness by a notary the verifier does not know is refused, and so is a witness altered after it was given",
              stranger.fails() && stranger.reason.find("knows") != std::string::npos && misattested.fails() && misattested.reason.find("attests") != std::string::npos,
              stranger.reason + " / " + misattested.reason);
    const auto refused = x::witness(n, "registry", "passer", 2, home_head);
    s.require("a witness the notary's own rules do not permit is refused", !refused.admitted, refused.reason);
  }
  {
    x::Passport forged = pass;
    forged.chain.back().proof.path.front()[0] ^= 1;
    const auto f = x::verify(forged, {{home_head}, {}}, shown);
    x::Passport widened = pass;
    widened.chain.back().act.grant->kinds.insert("amend");
    const auto g = x::verify(widened, {{home_head}, {}}, shown);
    s.require("a forged proof, and a grant widened after its sealing, are refused: the act is not under the home's head",
              f.fails() && f.reason.find("not under the home's head") != std::string::npos && g.fails(), f.reason + " / " + g.reason);
  }
  {
    const auto late = x::verify(pass, {{home_head}, {}}, {day(21), "atelier", "token-atelier"});
    const auto early = x::verify(pass, {{home_head}, {}}, {day(1), "atelier", "token-atelier"});
    s.require("a passport shown after its term in instants is refused as expired, and one shown before it as not begun",
              late.fails() && late.reason.find("has ended") != std::string::npos && early.fails() && early.reason.find("not begun") != std::string::npos, late.reason + " / " + early.reason);
    const auto wrong = x::verify(pass, {{home_head}, {}}, {day(5), "atelier", "token-forged"});
    const auto elsewhere = x::verify(pass, {{home_head}, {}}, {day(5), "forum", "token-atelier"});
    s.require("a credential whose digest differs from the mandate's is refused, and so is a host the mandate does not name",
              wrong.fails() && wrong.reason.find("credential") != std::string::npos && elsewhere.fails() && elsewhere.reason.find("forum") != std::string::npos,
              wrong.reason + " / " + elsewhere.reason);
  }

  // ---- 3. What the home refuses to issue -------------------------------------------------------------------------------------------
  {
    State r = home();
    chronicle::step(r, issuing(root_of(), 1));
    const auto bare = x::issue("ana-household", r, "m-scribe", day(3));
    chronicle::step(r, issuing(split_of("atelier", 60, "t"), 2));
    chronicle::step(r, act("ana", "revoke", 3, {{"which", "m-scribe-atelier"}}));
    const auto revoked = x::issue("ana-household", r, "m-scribe-atelier", day(3));
    Mandate theirs = root_of();
    theirs.id = "m-theirs", theirs.holder = "ana/helper", theirs.since.clear(), theirs.until.clear(), theirs.credential = digest_of("h");
    Mandate borrowed = theirs;
    borrowed.id = "m-borrowed", borrowed.holder = "zed/agent";
    chronicle::step(r, issuing(theirs, 4));
    chronicle::step(r, issuing(borrowed, 4));
    const auto helper = x::issue("ana-household", r, "m-theirs", day(3));
    const auto foreign = x::issue("ana-household", r, "m-borrowed", day(3));
    s.require("the home issues no passport for a mandate that names no credential, nor for a revoked one, nor for an operative whose declared controller issued no link of its chain",
              !bare.admitted && bare.reason.find("credential") != std::string::npos && !revoked.admitted && revoked.reason.find("revoked") != std::string::npos &&
                  !helper.admitted && !foreign.admitted && foreign.reason.find("zed") != std::string::npos,
              bare.reason + " / " + revoked.reason + " / " + helper.reason + " / " + foreign.reason);
  }
  {
    State r = home();
    chronicle::step(r, issuing(root_of(), 1));
    Mandate loose = split_of("atelier", 10, "t");
    loose.id = "m-loose", loose.credential = "5D41402ABC";
    Mandate badly = split_of("atelier", 10, "t");
    badly.id = "m-badly", badly.until = "2026-12-31";
    Mandate wide = split_of("atelier", 10, "t");
    wide.id = "m-wide", wide.until = "2027-01-15T00:00:00Z";
    State named = home();
    Mandate rooted = root_of();
    rooted.communities = {"atelier"};
    chronicle::step(named, issuing(rooted, 1));
    Mandate far = split_of("forum", 10, "t");
    const auto a = chronicle::step(r, issuing(loose, 2)), b = chronicle::step(r, issuing(badly, 2)), c = chronicle::step(r, issuing(wide, 2)), d = chronicle::step(named, issuing(far, 2));
    s.require("the home refuses a mandate that names its credential otherwise than by digest, states its term in instants badly, widens its parent's term in instants, or names a community its parent does not",
              !a.admitted && a.reason.find("digest") != std::string::npos && !b.admitted && b.reason.find("ISO 8601") != std::string::npos && !c.admitted &&
                  c.reason.find("term in instants") != std::string::npos && !d.admitted && d.reason.find("communities") != std::string::npos,
              a.reason + " / " + b.reason + " / " + c.reason + " / " + d.reason);
    Mandate unreserved = split_of("atelier", 10, "t");
    unreserved.id = "m-unreserved", unreserved.reserves = false;
    Mandate stranger = split_of("atelier", 10, "t");
    stranger.id = "m-stranger", stranger.principal = "zed";
    const auto e = chronicle::step(r, issuing(unreserved, 3)), f = chronicle::step(r, issuing(stranger, 3));
    s.require("a part of a mandate for its own holder is a split only when its parent's principal issues it and it reserves: unreserved, it is no delegation by the holder",
              !e.admitted && e.reason.find("not issued by the holder") != std::string::npos && !f.admitted, e.reason + " / " + f.reason);
    Act within = act("ana/scribe", "say", 4, {}, "m-scribe");
    within.when = day(10);
    Act after = act("ana/scribe", "say", 5, {}, "m-scribe");
    after.when = "2026-12-29T00:00:00Z";
    const auto inside = chronicle::step(r, within), outside = chronicle::step(r, after);
    s.require("the home admits an act made within the mandate's term in instants and refuses one made after it", inside.admitted && !outside.admitted &&
                                                                                                                         outside.reason.find("term in instants") != std::string::npos,
              inside.reason + " / " + outside.reason);
  }

  // ---- 4. Proposition 11 on random homes ---------------------------------------------------------------------------------------------
  {
    calx::random::Stream r(41, 7, 0, 0);
    const std::vector<std::string> kinds = {"present", "apply", "consult", "deposit", "commit", "deliver", "say", "report"};
    const std::vector<std::string> hosts = {"atelier", "forum", "harbour"};
    int issued_ok = 0, verified = 0, mutations = 0, caught = 0, chains_of_two = 0, delegations = 0;
    std::string first_miss;
    for (int trial = 0; trial < 300; trial++) {
      State t = home();
      Mandate root;
      root.id = "root", root.principal = "ana", root.holder = "ana/scribe";
      for (const auto& k : kinds)
        if (between(r, 0, 2)) root.kinds.insert(k);
      root.kinds.insert("say");
      root.matters = {"the catalogue"};
      if (between(r, 0, 1)) root.matters.insert("the glossary");
      root.allowance = {{"runs", between(r, 50, 500)}};
      root.from = between(r, 0, 2), root.to = between(r, 500, 900);
      const long long a = between(r, 1, 10), b = between(r, 18, 28);
      if (between(r, 0, 2)) root.since = day(a);
      if (between(r, 0, 2)) root.until = day(b);
      if (between(r, 0, 1))
        for (const auto& host : hosts)
          if (between(r, 0, 1)) root.communities.insert(host);
      const long long shape = between(r, 0, 2);  // 0: the root alone; 1: a split; 2: a delegation by the holder
      if (shape == 2) root.delegable = true, root.kinds.insert("mandate");
      if (shape == 0) root.credential = digest_of("token-" + std::to_string(trial));
      chronicle::step(t, issuing(root, 1));
      std::string leaf_id = "root";
      if (shape > 0) {
        Mandate child = root;
        child.id = "child", child.parent = "root", child.credential = digest_of("token-" + std::to_string(trial)), child.delegable = false;
        child.kinds.erase("mandate");
        child.allowance = {{"runs", between(r, 1, root.allowance["runs"])}};
        child.from = root.from + between(r, 0, 3), child.to = root.to - between(r, 0, 3);
        if (!root.since.empty() || between(r, 0, 1)) child.since = day(std::max(a, between(r, 1, 11)));
        if (!root.until.empty() || between(r, 0, 1)) child.until = day(std::min(b, between(r, 17, 28)));
        child.communities.clear();
        const std::string host = root.communities.empty() ? hosts[between(r, 0, 2)] : *root.communities.begin();
        child.communities.insert(host);
        if (shape == 1) child.reserves = true;
        else child.principal = "ana/scribe", child.holder = "ana/helper";
        const Act made = issuing(child, 2, shape == 2 ? "root" : "");
        const auto o = chronicle::step(t, made);
        if (!o.admitted) {
          if (first_miss.empty()) first_miss = "child refused: " + o.reason;
          continue;
        }
        leaf_id = "child";
        chains_of_two++;
        delegations += shape == 2;
      }
      const auto made = x::issue("ana-household", t, leaf_id, day(11));
      if (!made.admitted) {
        if (first_miss.empty()) first_miss = "not issued: " + made.reason;
        continue;
      }
      issued_ok++;
      const x::Head hd = x::head("ana-household", t);
      const Mandate& leaf = made.passport.mandate();
      const x::Presentation at{day(12), leaf.communities.empty() ? std::string() : *leaf.communities.begin(), "token-" + std::to_string(trial)};
      const auto v = x::verify(made.passport, {{hd}, {}}, at);
      if (v.holds()) verified++;
      else if (first_miss.empty()) first_miss = "not verified: " + v.reason;
      for (int k = 0; k < 13; k++) {
        x::Passport m = made.passport;
        x::Link& link = m.chain[static_cast<std::size_t>(between(r, 0, static_cast<long long>(m.chain.size()) - 1))];
        Mandate& g = *link.act.grant;
        switch (k) {
          case 0: g.kinds.insert("amend"); break;
          case 1: g.matters.insert("the treasury"); break;
          case 2: g.allowance["runs"] += 1; break;
          case 3: g.to += 1; break;
          case 4: g.until = g.until == day(26) ? day(25) : day(26); break;
          case 5: g.communities.insert("elsewhere"); break;
          case 6: g.credential = digest_of("another token"); break;
          case 7: g.reserves = !g.reserves; break;
          case 8: g.delegable = !g.delegable; break;
          case 9: link.act.turn += 1; break;
          case 10: link.act.fields["matter"] = "the catalogue"; break;
          case 11: link.seal = digest_of("another seal"); break;
          case 12: m.head.size += 1; break;
        }
        mutations++;
        caught += x::verify(m, {{hd}, {}}, at).fails();
      }
    }
    s.require("on 300 random homes every passport issued verifies, chains of one and of two links, splits and delegations among them (Proposition 11; " + std::to_string(issued_ok) +
                  " issued, " + std::to_string(chains_of_two) + " of two links, " + std::to_string(delegations) + " delegations)",
              issued_ok == 300 && verified == issued_ok && chains_of_two > 120 && delegations > 50, first_miss);
    s.require("and every alteration of a sealed part of a link, of a seal or of the head is refused (" + std::to_string(caught) + " of " + std::to_string(mutations) + ")",
              mutations == 13 * issued_ok && caught == mutations);
  }

  // ---- 5. Reservations: Proposition 12 -----------------------------------------------------------------------------------------------
  {
    calx::random::Stream r(17, 3, 0, 0);
    int over_home = 0, over_hosts = 0, ledger = 0, admitted_splits = 0, refused_splits = 0;
    for (int trial = 0; trial < 200; trial++) {
      State t = home();
      const long long ceiling = between(r, 50, 300);
      Mandate root = root_of(ceiling);
      root.since.clear(), root.until.clear();
      chronicle::step(t, issuing(root, 1));
      long long reserved = 0;
      std::vector<Mandate> splits;
      const long long tries = between(r, 1, 8);
      for (long long k = 0; k < tries; k++) {
        Mandate m = split_of("host-" + std::to_string(k), between(r, 1, ceiling), "t" + std::to_string(k));
        m.since.clear(), m.until.clear();
        if (chronicle::step(t, issuing(m, 2 + k)).admitted) reserved += m.allowance["runs"], splits.push_back(m), admitted_splits++;
        else refused_splits++;
      }
      over_home += reserved > ceiling;
      ledger += t.used["m-scribe"]["runs"] != reserved;
      long long admitted = 0;
      for (const Mandate& m : splits) {  // each host knows the passport's chain, and charges only under its own split
        mandates::Registry at_host{{"m-scribe", root}, {m.id, m}};
        mandates::Envelopes env(at_host);
        long long here = 0;
        for (int d = 0; d < 40; d++) {
          const long long amount = between(r, 1, 30);
          if (env.debit(m.id, "runs", amount)) here += amount;
        }
        over_hosts += here > m.allowance.at("runs");
        admitted += here;
      }
      over_hosts += admitted > ceiling;
    }
    s.require("on 200 random homes the reserving splits of a root never pass its ceiling, which records exactly what they reserve (Proposition 12; " + std::to_string(admitted_splits) +
                  " admitted, " + std::to_string(refused_splits) + " refused for want of room)",
              over_home == 0 && ledger == 0 && admitted_splits > 200 && refused_splits > 100);
    s.require("and hosts that each charge only under their own split admit, all together, no more than the root's ceiling", over_hosts == 0);
  }
  {
    State t = home();
    Mandate root = root_of(100);
    root.delegable = true;
    chronicle::step(t, issuing(root, 1));
    std::vector<Mandate> children;
    for (const char* host : {"atelier", "forum"}) {
      Mandate c = root;
      c.id = std::string("m-") + host, c.parent = "m-scribe", c.principal = "ana/scribe", c.delegable = false;
      c.kinds.erase("mandate");
      c.allowance = {{"runs", 80}}, c.communities = {host};
      if (chronicle::step(t, issuing(c, 2, "m-scribe")).admitted) children.push_back(c);
    }
    long long at_hosts = 0;
    for (const Mandate& c : children) {
      mandates::Registry at_host{{"m-scribe", root}, {c.id, c}};
      mandates::Envelopes env(at_host);
      at_hosts += env.debit(c.id, "runs", 80) ? 80 : 0;
    }
    s.require("without reserving, the home admits two children of 80 under a root of 100, and two hosts that each see only their own admit 160", children.size() == 2 && at_hosts == 160);
  }
  {
    State t = home();
    chronicle::step(t, issuing(root_of(100), 1));
    const bool first = chronicle::step(t, issuing(split_of("atelier", 60, "a"), 2)).admitted;
    Act draw = act("ana/scribe", "say", 3, {}, "m-scribe-atelier");
    draw.charges = {{"m-scribe-atelier", "runs", 10}};
    const bool drawn = chronicle::step(t, draw).admitted;
    const bool second = chronicle::step(t, issuing(split_of("forum", 50, "f"), 4)).admitted;
    const bool third = chronicle::step(t, issuing(split_of("harbour", 40, "h"), 5)).admitted;
    s.require("a draw under a reserving split is charged from the split down and never again above it: the root keeps the 60 reserved, a second split of 50 has no room and one of 40 has",
              first && drawn && !second && third && t.used["m-scribe"]["runs"] == 100 && t.used["m-scribe-atelier"]["runs"] == 10);
    mandates::Registry reg{{"m-scribe", root_of(100)}};
    Mandate part = split_of("atelier", 60, "a");
    mandates::Envelopes env(reg);
    const bool r1 = env.reserve(part);
    Mandate more = split_of("forum", 50, "f");
    const bool r2 = env.reserve(more);
    s.require("the envelopes' own reservation agrees with the fold's: 60 reserved, then 50 refused", r1 && !r2 && env.used("m-scribe", "runs") == 60);
  }

  // ---- 6. Records made before decision 0007: Proposition 13 --------------------------------------------------------------------------
  {
    Parties ps;
    ps.add({"ana", Kind::person});
    ps.add({"ana/s", Kind::operative, "ana"});
    State t(ps, workshop());
    Act found = act("ana", "found", 0);
    found.holders = {{"ana", "steward"}};
    Mandate m;
    m.id = "m1", m.principal = "ana", m.holder = "ana/s", m.kinds = {"say"}, m.allowance = {{"runs", 5}}, m.from = 1, m.to = 9;
    Act say = act("ana/s", "say", 2, {}, "m1");
    say.body = "hello";
    const bool all = chronicle::step(t, found).admitted && chronicle::step(t, issuing(m, 1)).admitted && chronicle::step(t, say).admitted;
    const std::string expected_envelope =
        "author:3:ana;kind:7:mandate;turn:1:1;when:0:;mandate:0:;grant:121:id:2:m1;principal:3:ana;holder:5:ana/s;kinds:7::3:say;;matters:0:;allowance.runs:1:5;term:3:1-9;"
        "delegable:2:no;parent:0:;;";
    // The seals as Python's hashlib computes them from the envelopes as written before decision 0007.
    const std::vector<std::string> seals = {"20f3efe4083c8c350c986542a5288d6fd5271fde3f5a12393de4b2c26f480017", "727e526533ec7ed4972f617e83bcb8441c66936737fe07c507d73f31cdf62bf4",
                                            "5f88239093dcafc7921dfdca5ca9230ecf61213466f58e96df9d5ac9a20db16f"};
    bool sealed = all && t.journal.entries.size() == 3;
    for (std::size_t k = 0; sealed && k < 3; k++) sealed = t.journal.entries[k].seal == seals[k];
    s.require("a mandate that sets none of the parts passports add is written exactly as before, and a record made before decision 0007 seals as Python's hashlib computes it then (Proposition 13)",
              sealed && acts::envelope(t.acts[1]) == expected_envelope, acts::envelope(t.acts[1]));
  }
  return s.done();
}
