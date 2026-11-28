// Checks of holdings in the fold and the briefing, on the illustrative constitution examples/workshop.json (decision 0006):
//   - the founding holds what the constitution declares;
//   - a deposit within a holding that declares review stands pending until a party of another origin attests it, and the
//     depositor's own agent cannot; a modification by its depositor keeps the version before; a withdrawal by the custodian leaves a
//     marker; each refusal names its norm or the absence of any;
//   - a quota a round for the party that answers, a declared purpose, a loan that holds what is available until its term, an agent's
//     draw charged along its mandate's chain, a need attested by another origin and never by oneself, open access for a kind;
//   - Proposition 9 on 40 random chronicles of 300 acts: a metered holding's quantity is what was declared and deposited less what was
//     used and withdrawn, never negative; what is drawn beneath any link of a chain stays within that link's ceiling and equals the
//     draws made beneath it; a quota is never passed by the operatives of one party together; a refused act changes nothing;
//   - Proposition 1 with holdings: a replay gives the same relational state;
//   - transmission: what a norm permits to be sent outside is sent and recorded so, and what a norm is silent on is never sent;
//   - the briefing: the mission, what each party may do with each holding and the norm, the duties, the open work, and its holdings
//     part equal to decide over every holding and aim it may know of (Proposition 10);
//   - the profile: what the public may know of the workshop, what a member and the steward may know besides.

#include <thesmos/chronicle/briefing.hpp>
#include <thesmos/chronicle/chronicle.hpp>
#include <thesmos/chronicle/profile.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

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

Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate);
  return a;
}

Parties parties() {
  Parties ps;
  for (const char* p : {"steward", "reviewer-a", "reviewer-b", "mei", "resident"}) ps.add({p, Kind::person});
  ps.add({"mei/agent-1", Kind::operative, "mei"});
  ps.add({"lab-k", Kind::organisation});
  return ps;
}

const std::string kSource = "b46d8ac8550b8b4151f009d0a6fb4ada7d5e55a51f30251b946ec0a777a395ec";     // "a source: the 1901 street directory"
const std::string kCorrected = "24845039267c3eee8b993550798e9a150af4a2c4c3b7971e3d0674119ed08b2e";  // "a source corrected"
const std::string kSecond = "f45259442b5a866fca51e709037dda7eff89fbd529f1e015c5f673539469ade3";     // "the workshop catalogue, second edition"

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto c = constitutions::read(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
  s.require("the workshop declares seven holdings and eleven norms of access, and is admitted", c.holdings.holdings.size() == 7 && c.holdings.norms.size() == 11 &&
                                                                                              constitutions::admission(c).empty());
  State st(parties(), c);
  std::vector<Act> record;
  auto put = [&](const Act& a) {
    const auto o = chronicle::step(st, a);
    if (o.admitted) record.push_back(a);
    return o;
  };
  Act found = act("steward", "found", 0);
  found.holders = {{"steward", "steward"}, {"reviewer-a", "reviewer"}, {"reviewer-b", "reviewer"}, {"mei", "contributor"}, {"mei", "reviewer"}, {"resident", "contributor"}};
  put(found);
  s.require("the founding holds what the constitution declares", st.holdings.size() == 7 && st.holdings.at("review hours").quantity == 40 && st.holdings.at("guide").since == 0);

  // ---- Deposit, check, modification, withdrawal -------------------------------------------------------------------------------------
  Act m = act("mei", "mandate", 1);
  m.grant = Mandate{"m-agent", "mei", "mei/agent-1", {"deposit", "consult", "use", "attest", "say"}, {"catalogue"}, {{"tokens", 50000}}, 1, 40};
  put(m);
  const auto d1 = put(act("mei", "deposit", 1, {{"holding", "src-1"}, {"within", "sources"}, {"digest", kSource}, {"source", "the 1901 street directory"}, {"licence", "public domain"}}));
  const auto d2 = put(act("lab-k", "deposit", 1, {{"holding", "src-2"}, {"within", "sources"}, {"digest", kSource}}));
  const auto d3 = put(act("mei", "deposit", 1, {{"holding", "src-9"}, {"within", "sources"}, {"digest", "not a digest"}}));
  s.require("a contributor deposits a source within the sources, which awaits its check; a party holding no position is refused for want of a norm, and a deposit "
            "without a digest by archeion's rule is refused",
            d1.admitted && st.holdings.at("src-1").pending && st.holdings.at("src-1").depositor == "mei" && st.holdings.at("src-1").kind == "data" && !d2.admitted &&
                d2.reason.find("no norm permits lab-k") != std::string::npos && !d3.admitted && d3.reason.find("SHA-256") != std::string::npos,
            d2.reason + " / " + d3.reason);
  const auto k1 = put(act("mei/agent-1", "attest", 1, {{"holding", "src-1"}}, "m-agent"));
  const auto k2 = put(act("reviewer-a", "attest", 2, {{"holding", "src-1"}}));
  s.require("the depositor's own agent cannot check its deposit; a reviewer of another origin can", !k1.admitted && k1.reason.find("another origin") != std::string::npos && k2.admitted &&
                                                                                                      !st.holdings.at("src-1").pending && st.holdings.at("src-1").checked_by == "reviewer-a",
            k1.reason);
  const auto m1 = put(act("resident", "modify", 2, {{"holding", "src-1"}, {"digest", kCorrected}}));
  const auto m2 = put(act("mei", "modify", 2, {{"holding", "src-1"}, {"digest", kCorrected}}));
  s.require("its depositor may correct it, keeping the version before, and another contributor may not",
            !m1.admitted && m2.admitted && st.holdings.at("src-1").versions == std::vector<std::string>{kSource} && st.holdings.at("src-1").provenance.digest == kCorrected, m1.reason);
  const auto w1 = put(act("mei", "withdraw", 2, {{"holding", "src-1"}}));
  const auto w2 = put(act("steward", "withdraw", 2, {{"holding", "src-1"}}));
  const auto w3 = put(act("resident", "consult", 3, {{"holding", "src-1"}}));
  s.require("the custodian withdraws it and a marker stays; its depositor may not, and it is consulted no more",
            !w1.admitted && w2.admitted && st.holdings.at("src-1").withdrawn && !w3.admitted && w3.reason.find("was withdrawn") != std::string::npos, w1.reason);

  // ---- Quota, purpose, loan ---------------------------------------------------------------------------------------------------------
  const auto q1 = put(act("reviewer-a", "use", 3, {{"holding", "review hours"}, {"amount", "4"}, {"purpose", "review"}}));
  const auto q2 = put(act("reviewer-a", "use", 3, {{"holding", "review hours"}, {"amount", "3"}, {"purpose", "review"}}));
  const auto q3 = put(act("reviewer-a", "use", 4, {{"holding", "review hours"}, {"amount", "3"}}));
  const auto q4 = put(act("reviewer-a", "use", 4, {{"holding", "review hours"}, {"amount", "3"}, {"purpose", "review"}}));
  s.require("a reviewer draws 4 review hours, is refused 3 more in the round by the quota of 6, refused without the declared purpose, and draws 3 the round after",
            q1.admitted && !q2.admitted && q2.reason.find("quota is 6 hours a round, of which 4 is drawn") != std::string::npos && !q3.admitted &&
                q3.reason.find("declares no purpose") != std::string::npos && q4.admitted && st.holdings.at("review hours").quantity == 33,
            q2.reason + " / " + q3.reason);
  const auto l1 = put(act("steward", "lend", 4, {{"holding", "review hours"}, {"amount", "30"}, {"to", "a neighbouring workshop"}, {"until", "5"}}));
  const auto l2 = put(act("reviewer-b", "use", 5, {{"holding", "review hours"}, {"amount", "4"}, {"purpose", "review"}}));
  const auto l3 = put(act("reviewer-b", "use", 6, {{"holding", "review hours"}, {"amount", "4"}, {"purpose", "review"}}));
  s.require("a loan of 30 hours until round 5 leaves 3 available until its term, and all 33 after it", l1.admitted && !l2.admitted &&
                                                                                                         l2.reason.find("has 3 hours available") != std::string::npos && l3.admitted,
            l2.reason);

  // ---- An agent's draw along its chain ----------------------------------------------------------------------------------------------
  const auto t1 = put(act("mei/agent-1", "use", 6, {{"holding", "tokens"}, {"amount", "30000"}}, "m-agent"));
  const auto t2 = put(act("mei/agent-1", "use", 6, {{"holding", "tokens"}, {"amount", "30000"}}, "m-agent"));
  const auto t3 = put(act("mei/agent-1", "use", 7, {{"holding", "tokens"}, {"amount", "30000"}}, "m-agent"));
  const auto t4 = put(act("resident", "use", 7, {{"holding", "tokens"}, {"amount", "10"}}));
  s.require("an agent draws 30,000 tokens under its mandate, charged along its chain; is refused more in the round by the quota, and the round after by its "
            "mandate's allowance of 50,000; a person is refused, the norm binding operatives",
            t1.admitted && st.used["m-agent"]["tokens"] == 30000 && st.holdings.at("tokens").quantity == 170000 && !t2.admitted &&
                t2.reason.find("quota") != std::string::npos && !t3.admitted && t3.reason.find("allowance of tokens under m-agent") != std::string::npos && !t4.admitted,
            t2.reason + " / " + t3.reason);

  // ---- Need, open access, the catalogue ---------------------------------------------------------------------------------------------
  const auto n1 = put(act("resident", "use", 7, {{"holding", "relief"}, {"amount", "2"}}));
  put(act("reviewer-a", "assess", 7, {{"object", "resident"}, {"criterion", "pressing need"}, {"value", "in need"}}));
  const auto n2 = put(act("resident", "use", 7, {{"holding", "relief"}, {"amount", "2"}}));
  const auto n3 = put(act("resident", "use", 7, {{"holding", "relief"}, {"amount", "1"}}));
  put(act("steward", "assess", 7, {{"object", "steward"}, {"criterion", "pressing need"}, {"value", "in need"}}));
  const auto n4 = put(act("steward", "use", 7, {{"holding", "relief"}, {"amount", "1"}}));
  s.require("relief by need: refused until a reviewer attests the need, then 2 hours within the quota; a steward's attestation of its own need counts for nothing",
            !n1.admitted && n2.admitted && !n3.admitted && !n4.admitted && n4.reason.find("attests no need") != std::string::npos, n4.reason);
  const auto o1 = put(act("resident", "consult", 8, {{"holding", "guide"}}));
  const auto o2 = put(act("resident", "use", 8, {{"holding", "guide"}}));
  const auto o3 = put(act("mei", "modify", 8, {{"holding", "catalogue"}, {"digest", kSecond}}));
  const auto o4 = put(act("steward", "modify", 8, {{"holding", "catalogue"}, {"digest", kSecond}, {"source", "the workshop catalogue, second edition"}}));
  bool open = false, recorded = false;
  for (const auto& x : st.accesses) {
    open = open || (x.holding == "guide" && x.aim == "consult" && x.norm == "open access");
    recorded = recorded || (x.holding == "review hours" && x.author == "reviewer-a" && x.amount == 4 && x.purpose == "review" && x.norm == "review hours used");
  }
  s.require("the guide, of a kind declared open to consultation, is consulted with no norm and not used; the catalogue is revised by the steward alone",
            o1.admitted && open && !o2.admitted && !o3.admitted && o4.admitted, o2.reason + " / " + o3.reason);
  s.require("every access is recorded with who, which holding, what, how much, for what purpose and the norm that permitted it", recorded && st.accesses.size() == 11,
            std::to_string(st.accesses.size()));

  // ---- Transmission -----------------------------------------------------------------------------------------------------------------
  const auto x1 = put(act("reviewer-b", "consult", 8, {{"holding", "sources"}, {"transmit", "a model provider"}}));
  const bool sent = st.accesses.back().transmit == "a model provider";
  const auto x2 = put(act("reviewer-b", "consult", 8, {{"holding", "catalogue"}, {"transmit", "a model provider"}}));
  const auto x3 = put(act("reviewer-b", "consult", 8, {{"holding", "catalogue"}}));
  s.require("what a norm permits to be sent outside is sent and recorded so; the catalogue, whose norm is silent on transmission, is read within and never sent",
            x1.admitted && sent && !x2.admitted && x2.reason.find("sent outside the community") != std::string::npos && x3.admitted, x2.reason);

  // ---- Proposition 1 with holdings --------------------------------------------------------------------------------------------------
  State again;
  const auto v = chronicle::replay(parties(), c, record, st.journal, &again);
  s.require("a replay of the record gives the same relational state, holdings and accesses included",
            v.holds() && chronicle::relational_digest(again) == chronicle::relational_digest(st) && again.holdings.at("catalogue").versions.size() == 1, v.reason);

  // ---- The briefing -------------------------------------------------------------------------------------------------------------------
  put(act("resident", "deposit", 9, {{"holding", "src-3"}, {"within", "sources"}, {"digest", kSource}}));
  const auto rb = chronicle::brief(st, "reviewer-b", {}, "review");
  auto entry = [&](const chronicle::Briefing& b, const std::string& holding, holdings::Aim a) -> const holdings::Entry* {
    for (const auto& e : b.entries)
      if (b.targets[e.target].id == holding && !b.targets[e.target].fresh && e.aim == a) return &e;
    return nullptr;
  };
  bool check_src3 = false;
  for (const auto& w : rb.work) check_src3 = check_src3 || (w.kind == "check" && w.id == "src-3");
  const auto* hours = entry(rb, "review hours", holdings::Aim::use);
  s.require("a reviewer's briefing gives the mission, review hours to use for review with the norm and the quota left, and a deposit to check",
            rb.mission == c.purpose && hours && hours->decision.permitted && hours->decision.norm == "review hours used" && hours->remaining == 6 && check_src3,
            hours ? hours->decision.reason : "no entry for review hours");
  const auto res = chronicle::brief(st, "resident");
  bool own = false;
  for (const auto& w : res.work) own = own || w.id == "src-3";
  const auto ab = chronicle::brief(st, "mei/agent-1", "m-agent");
  const auto* tokens = entry(ab, "tokens", holdings::Aim::use);
  const auto sb = chronicle::brief(st, "steward");
  s.require("the depositor is not asked to check its own deposit; an agent is told it may draw tokens under its mandate; the steward is told its duty",
            !own && tokens && tokens->decision.permitted && tokens->decision.norm == "tokens drawn by agents" && sb.duties.size() == 1 && sb.duties[0].id == "catalogue kept current",
            tokens ? tokens->decision.reason : "no entry for tokens");
  long long differ = 0, pairs = 0;
  for (const std::string& p : {"steward", "reviewer-a", "reviewer-b", "mei", "resident", "lab-k", "mei/agent-1"})
    for (const std::string& purpose : {"", "review"}) {
      const std::string mandate = p == "mei/agent-1" ? "m-agent" : "";
      const auto b = chronicle::brief(st, p, mandate, purpose, 9);
      const auto x = chronicle::situation(st, p, mandate, 9);
      std::map<std::pair<std::size_t, holdings::Aim>, const holdings::Entry*> by;
      for (const auto& e : b.entries) by[{e.target, e.aim}] = &e;
      for (std::size_t k = 0; k < b.targets.size(); k++)
        for (holdings::Aim a : b.targets[k].aims) {
          pairs++;
          const auto d = holdings::decide(st.rules.holdings, b.targets[k], {a, holdings::least(b.targets[k], a), purpose}, x);
          const auto it = by.find({k, a});
          if ((it != by.end()) != (d.spoken || d.open)) differ++;
          else if (it != by.end()) differ += it->second->decision.permitted != d.permitted || it->second->decision.norm != d.norm;
        }
    }
  s.require("each party's briefing lists exactly the holdings and aims on which a norm speaks to it or open access applies, each with decide's verdict and norm "
            "(Proposition 10; " + std::to_string(pairs) + " pairs)",
            differ == 0, std::to_string(differ));

  // ---- The profile ----------------------------------------------------------------------------------------------------------------------
  put(act("steward", "request", 9, {{"id", "r1"}, {"matter", "catalogue"}}));
  auto tops = [](const chronicle::Profile& p) {
    std::set<std::string> n;
    for (const auto& h : p.holdings)
      if (h.within.empty()) n.insert(h.id);
    return n;
  };
  const auto pub = chronicle::profile(st), member = chronicle::profile(st, "mei"), keeper = chronicle::profile(st, "steward");
  const bool posted = !pub.work.empty() && pub.work.front().id == "r1" && pub.work.front().kind == "request";
  s.require("the public profile gives the mission, the admission policy and the route for claims, the public holdings alone and the work posted, and keeps the "
            "holders and the custody of means from the public",
            pub.purpose == c.purpose && pub.shown.count("boundary") && pub.shown.count("claims") && pub.withheld.count("holders") && pub.withheld.count("custody") &&
                pub.offices.empty() && tops(pub) == std::set<std::string>{"catalogue", "guide", "sources"} && posted);
  s.require("a member sees the holders and the members' holdings; the steward, the office the accounts are shown to, sees them and the custody of means",
            member.shown.count("holders") && !member.offices.empty() && member.withheld.count("custody") && tops(member).size() == 6 && !tops(member).count("accounts") &&
                keeper.shown.count("custody") && tops(keeper).size() == 7);
  const auto outsider = chronicle::brief(st, "lab-k");
  bool hidden = true;
  for (const auto& t : outsider.targets) hidden = hidden && t.visibility.level == holdings::Visibility::Level::everyone;
  s.require("an outsider's briefing names only what is public", hidden && !outsider.targets.empty());

  // ---- Proposition 9 on random chronicles -------------------------------------------------------------------------------------------
  {
    calx::random::Stream g(47, 1, 0, 0);
    long long quantity_bad = 0, ceiling_bad = 0, beneath_bad = 0, quota_bad = 0, refused_bad = 0, admitted_draws = 0, refused_n = 0;
    for (int run = 0; run < 40; run++) {
      Parties ps;
      for (int i = 0; i < 3; i++) {
        const std::string p = "p" + std::to_string(i);
        ps.add({p, Kind::person});
        ps.add({p + "/a", Kind::operative, p});
        ps.add({p + "/b", Kind::operative, p});
      }
      constitutions::Constitution k = c;
      const long long q0 = 20 + static_cast<long long>(g.below(60)), quota = 5 + static_cast<long long>(g.below(20));
      holdings::Holding pool;
      pool.id = "pool", pool.kind = "budget", pool.unit = "units", pool.quantity = q0, pool.custodian = "steward";
      k.holdings.holdings = {{"pool", pool}};
      holdings::Norm used, out, lent, in;
      used.id = "pool used", used.attribute = {holdings::Who::kind, "operative"}, used.aim = holdings::Aim::use, used.object = {holdings::Over::holding, "pool"};
      used.conditions.quota = quota, used.conditions.under_mandate = true, used.conditions.mandate_kinds = {"use"};
      out = used, out.id = "pool withdrawn", out.aim = holdings::Aim::withdraw, out.conditions = {};
      lent = out, lent.id = "pool lent", lent.aim = holdings::Aim::lend;
      in.id = "pool deposited", in.attribute = {holdings::Who::member, {}}, in.aim = holdings::Aim::deposit, in.object = {holdings::Over::holding, "pool"};
      k.holdings.norms = {used, out, lent, in};
      State r(ps, k);
      Act f = act("p0", "found", 0);
      f.holders = {{"p0", "contributor"}, {"p1", "contributor"}, {"p2", "contributor"}};
      chronicle::step(r, f);
      for (int i = 0; i < 3; i++) {
        const std::string p = "p" + std::to_string(i);
        const long long root = 10 + static_cast<long long>(g.below(50)), child = 1 + static_cast<long long>(g.below(static_cast<std::uint64_t>(root)));
        Act ma = act(p, "mandate", 0), mb = act(p + "/a", "mandate", 0, {}, "m-" + p + "-a");
        ma.grant = Mandate{"m-" + p + "-a", p, p + "/a", {"use", "withdraw", "lend", "deposit", "mandate"}, {}, {{"pool", root}}, 0, 1000, true};
        mb.grant = Mandate{"m-" + p + "-b", p + "/a", p + "/b", {"use", "withdraw", "lend", "deposit"}, {}, {{"pool", child}}, 0, 1000, false, "m-" + p + "-a"};
        chronicle::step(r, ma);
        chronicle::step(r, mb);
      }
      long long turn = 1;
      for (int n = 0; n < 300; n++) {
        turn += static_cast<long long>(g.below(3) == 0);
        const std::string p = "p" + std::to_string(g.below(3));
        const int who = static_cast<int>(g.below(3));
        const std::string author = who == 0 ? p : p + (who == 1 ? "/a" : "/b");
        const std::string kinds[] = {"use", "withdraw", "lend", "deposit"};
        Act a = act(author, kinds[g.below(4)], turn, {{"holding", "pool"}, {"amount", std::to_string(1 + g.below(15))}}, who == 0 ? "" : "m-" + p + (who == 1 ? "-a" : "-b"));
        if (a.kind == "lend") a.fields["to"] = "outside", a.fields["until"] = std::to_string(turn + static_cast<long long>(g.below(3)));
        const std::string before = chronicle::relational_digest(r);
        const std::size_t sealed = r.journal.entries.size();
        const auto o = chronicle::step(r, a);
        if (!o.admitted) {
          refused_n++;
          refused_bad += chronicle::relational_digest(r) != before || r.journal.entries.size() != sealed;
          continue;
        }
        admitted_draws += a.kind != "deposit";
        // (a) the quantity: declared, plus deposits, less use and withdrawal; never negative, nor what is available
        long long expect = q0;
        for (const auto& x : r.accesses) expect += x.aim == "deposit" ? x.amount : (x.aim == "use" || x.aim == "withdraw") ? -x.amount : 0;
        quantity_bad += r.holdings.at("pool").quantity != expect || expect < 0 || chronicle::available(r, "pool", turn) < 0;
        // (b) beneath every link: within its ceiling, and equal to the draws made under mandates whose chain holds it
        for (const auto& [id, mm] : r.mandates) {
          long long beneath = 0;
          for (const auto& x : r.acts)
            if (!x.mandate.empty() && x.kind != "deposit" && x.kind != "mandate" && x.kind != "found")
              for (const Mandate* link : mandates::chain(r.mandates, x.mandate))
                if (link->id == id) beneath += std::stoll(x.field("amount"));
          const long long charged = r.used.count(id) && r.used.at(id).count("pool") ? r.used.at(id).at("pool") : 0;
          ceiling_bad += charged > mm.allowance.at("pool");
          beneath_bad += charged != beneath;
        }
        // (c) the quota of "pool used" for the party that answers, its operatives together, in each round
        std::map<std::pair<std::string, long long>, long long> drawn;
        for (const auto& x : r.accesses)
          if (x.norm == "pool used") drawn[{x.bearer, x.turn}] += x.counted;
        for (const auto& [key, amount] : drawn) quota_bad += amount > quota;
      }
    }
    s.require("a metered holding's quantity is what was declared and deposited less what was used and withdrawn, never negative, nor what is available "
              "(Proposition 9; 40 random chronicles, " + std::to_string(admitted_draws) + " draws admitted, " + std::to_string(refused_n) + " acts refused)",
              quantity_bad == 0, std::to_string(quantity_bad));
    s.require("what is drawn beneath any link of a chain stays within its ceiling and equals the draws made beneath it", ceiling_bad == 0 && beneath_bad == 0,
              std::to_string(ceiling_bad) + " / " + std::to_string(beneath_bad));
    s.require("a quota a round is never passed by the operatives of one party together, and a refused act changes nothing", quota_bad == 0 && refused_bad == 0,
              std::to_string(quota_bad) + " / " + std::to_string(refused_bad));
  }
  return s.done();
}
